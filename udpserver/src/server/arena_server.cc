/*
 * =====================================================================================
 *
 *       Filename:  arena_server.cc
 *        Created:  08/22/14 16:24:07
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "arena_server.h"
#include <functional>
#include <glog/logging.h>

#include "file_system.h"
#include "udp_server.h"
#include "event_loop.h"
#include "arena_conf.h"
#include "mmap_file.h"

using fx::net::UdpServer;
using fx::net::EventLoop;

fx::net::EventLoop * ArenaServer::loop = NULL;

template<typename Container>
std::unique_ptr<Container> RestoreOrCreate(std::unique_ptr<fx::base::MMapFile> & file)
{
    std::unique_ptr<Container> res;
    if (not file->newly_created())
    {
        res = Container::RestoreFrom(file->start(), file->size());
        if (res) return res;

        /* Restore failed */
        LOG(WARNING) << "Restore from " << file->path() << " failed, try create";
    }

    if (res == NULL)
    {
        res = Container::CreateFrom(file->start(), file->size());
        if (res) return res;

        LOG(WARNING) << "Create from " << file->path() << " failed";
    }

    return NULL;
}

ArenaServer::ArenaServer(const std::string& ip, int port)
    :ip_(ip), port_(port)
{
}

ArenaServer::~ArenaServer()
{
}

int ArenaServer::Init(const std::string& conf_path)
{
    conf_.reset (ArenaConf::ParseConf(conf_path));
    if (conf_ == NULL) return -1;

    for (unsigned i = 0; i <= conf_->RankNumber(); ++i)
    {
        char buf[256];
        int count = ::snprintf(buf, sizeof(buf), "%s/arenasvrd_mmap_%u", conf_->mmap_path().c_str(), i);
        CHECK(count > 0 and static_cast<size_t>(count) < sizeof(buf)) << "snprintf failed";
        std::string path(buf, count);
        std::unique_ptr<fx::base::MMapFile> file(new fx::base::MMapFile(path, conf_->mmap_size(), fx::base::MMapFile::create_if_not_exists));

        if (not file->Inited())
        {
            LOG(ERROR) << "Create MMapFile failed, path[" << path << "], i[" << i << "]";
            return -2;
        }
        // i = 0 for rbtree
        if (i == 0)
        {
            active_ = RestoreOrCreate<MapType>(file);
            if (active_ == NULL) return -3;
        }
        else
        {
            auto list = RestoreOrCreate<ListType>(file);
            if (list == NULL) return -4;
            lists_[i] = std::move(list);
        }
        mmaps_[i] = std::move(file);
    }
    return 0;
}

void ArenaServer::Start()
{
    using namespace std::placeholders;
    assert (conf_ != NULL);
    loop_.reset (new fx::net::EventLoop);
    server_.reset (new UdpServer(loop_.get(), ip_, port_));
    server_->set_read_callback(std::bind(&ArenaServer::OnRead, this, _1, _2, _3));
    server_->Start();
    ArenaServer::loop = loop_.get();            /* for signal handler */
    RegisterSignalHandler();
    loop_->set_period_functor(std::bind(&ArenaServer::ResetData, this, _1));
    loop_->Run();
    LOG(INFO) << "ArenaServer Exit.";
}

void ArenaServer::SignalHandler(int signum)
{
    if (signum == SIGQUIT or signum == SIGINT or signum == SIGTERM)
    {
        loop->Quit();
    }
}

void ArenaServer::RegisterSignalHandler()
{
    signal(SIGQUIT, ArenaServer::SignalHandler);
    signal(SIGINT, ArenaServer::SignalHandler);
    signal(SIGTERM, ArenaServer::SignalHandler);
}

int ArenaServer::OnRead(const char* in, size_t len, std::string * out)
{
    arenasvrd::Request request;
    if (request.ParseFromArray(in, len) == false)
    {
        LOG(WARNING) << "Parse Requeset failed, len[" << len << "]";
        return -1;
    }
    arenasvrd::Response response;
    int ret = 0;
    LOG(INFO) << request.ShortDebugString();
    switch (request.type())
    {
        case arenasvrd::Request_Type_FIND_OPPONENT:
            ret = OnFindOpponent(request, &response);
            break;

        case arenasvrd::Request_Type_UPDATE_SELF:
            ret = OnUpdateSelf(request, &response);
            break;

        default:
            LOG(WARNING) << "Invalid request.type[" << request.type() << "]";
            return -2;
    }

    if (ret != 0)
    {
        LOG(WARNING) << "Process function return " << ret << ", request.type[" << request.type() << "]";
        return ret;
    }
    LOG(INFO) << response.ShortDebugString();
    response.SerializeToString(out);
    return 0;
}

void ArenaServer::ResetData(uint64_t iteration)
{
    const unsigned kPeriod = 100;

    if (iteration & 0xff) return;               /* run every 256 iteration */

    auto seconds_left = conf_->TimeLeftToNextSeason();
    const int kHalfHourSeconds = 30 * 60;
    if (seconds_left >= kHalfHourSeconds) return;                /* reset in the last 30 mins */

    active_->clear();
    for (auto & l : lists_)
    {
        l.second->clear();
    }
}

int ArenaServer::OnFindOpponent(const arenasvrd::Request & req, arenasvrd::Response * res)
{
    auto uin = req.self();
    if (uin < 10000) return -1;
    if (req.has_points() == false) return -2;

    auto rank = conf_->GetRankByPoints(req.points());
    auto iter = lists_.find(rank);
    assert (iter != lists_.end());

    if (conf_->InSeasonTime() == false)
    {
        res->set_status(arenasvrd::Response_Status_NOT_IN_SEASON_TIME);
        return 0;
    }

    auto others_in_rank = [&](decltype(iter) iter) { 
                            auto & list = iter->second; 
                            return list->size() > 0 and (list->size() > 1 or list->back().uin != uin); 
                        };
    while (not others_in_rank(iter))
    {
        if (iter == lists_.begin()) break;
        else --iter;
    }

    if (not others_in_rank(iter))
    {
        res->set_status(arenasvrd::Response_Status_NO_OPPONENT);
        return 0;
    }

    if (iter->second->back().uin == uin)
    {
        auto & list = iter->second;
        assert (list->size() > 1);
        auto self = list->pop_back();            /* pop self out temporarily */
        auto opponent = list->back();
        auto list_node_id = list->push_back(self);
        auto self_pos = active_->find(uin);
        assert (self_pos->first == uin);
        assert (list_node_id != ListType::kInvalidNodeId);
        assert (self_pos != active_->end());
        self_pos->second.list_node_id = list_node_id;
        res->set_status(arenasvrd::Response_Status_OK);
        res->set_opponent (opponent.uin);
    }
    else
    {
        auto opponent = iter->second->back();
        res->set_status(arenasvrd::Response_Status_OK);
        res->set_opponent (opponent.uin);
    }
    return 0;
}

int ArenaServer::OnUpdateSelf(const arenasvrd::Request & req, arenasvrd::Response * res)
{
    auto uin = req.self();
    if (uin < 10000) return -1;
    if (req.has_points() == false) return -2;
    auto point = req.points();

    if (conf_->InSeasonTime() == false)
    {
        res->set_status(arenasvrd::Response_Status_NOT_IN_SEASON_TIME);
        return 0;
    }

    auto current_rank = conf_->GetRankByPoints(point);
    auto self_pos = active_->find(uin);
    ListNode list_node;
    if (self_pos != active_->end())
    {
        /* unlink from old rank list */
        auto node = self_pos->second;
        auto old_rank = node.rank;
        auto rank_pos = lists_.find(old_rank);
        assert (rank_pos != lists_.end());
        list_node = rank_pos->second->Unlink(node.list_node_id);
        auto victim_iter = active_->find(list_node.uin);
        assert (list_node.uin == uin);
    }

    /* fill node info */
    list_node.uin = uin;
    list_node.rank = current_rank;
    list_node.points = point;

    auto rank_pos = lists_.find(current_rank);
    assert (rank_pos != lists_.end());

    /* check if new rank list has enough space */
    if (rank_pos->second->size() >= kMaxPersonInList)      /* reach upper limit */
    {
        /* remove Least Recently Used node */
        auto front = rank_pos->second->pop_front();
        size_t count = active_->erase(front.uin);
        assert (count == 1);
    }

    auto list_node_id = rank_pos->second->push_back(list_node);
    assert (list_node_id != ListType::kInvalidNodeId);
    //CAUTION : make sure your MapType DOESNOT invalidated iter by the previous pertential erase operation
    //self_pos = active_->find(uin);
    if (self_pos != active_->end())
    {
        /* update info in active */
        self_pos->second.rank = current_rank;
        self_pos->second.list_node_id = list_node_id;
    }
    else
    {
        /* new node for this uin */
        TreeNode node;
        node.rank = current_rank;
        node.list_node_id = list_node_id;
        auto res = active_->insert(std::make_pair(uin, node));
        assert (res.second);
        (void) res;
    }
    res->set_status(arenasvrd::Response_Status_OK);
    return 0;
}
