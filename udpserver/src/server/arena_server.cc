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
#include <sstream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/typeof/typeof.hpp>
#include <glog/logging.h>

#include "udp_server.h"
#include "event_loop.h"
#include "arena_conf.h"
#include "mmap_file.h"

using fx::net::UdpServer;
using fx::net::EventLoop;

fx::net::EventLoop * ArenaServer::loop = NULL;

ArenaServer::ArenaServer(const std::string& ip, int port)
    :ip_(ip), port_(port), kMaxPersonInList(20)
{
}

ArenaServer::~ArenaServer()
{
}

int ArenaServer::Init(const std::string& conf_path)
{
    conf_.reset (ArenaConf::ParseConf(conf_path));
    if (conf_ == NULL) return -1;

    std::ostringstream oss;
    oss << conf_->mmap_path() << "/arenasvrd_mmap_%u";
    std::string path_fmt = oss.str();
    for (unsigned i = 0; i <= conf_->RankNumber(); ++i)
    {
        std::string path = boost::str(boost::format(path_fmt) % i);
        std::auto_ptr<fx::base::MMapFile> file(new fx::base::MMapFile(path, conf_->mmap_size(), fx::base::MMapFile::create_if_not_exists));

        if (not file->Inited())
        {
            LOG(ERROR) << "Create MMapFile failed, path[" << path << "], i[" << i << "]";
            return -2;
        }
        // i = 0 for rbtree
        if (i == 0)
        {
            active_.reset (TreeType::RestoreFrom(file->start(), file->size()));
            if (active_ == NULL)
            {
                active_.reset (TreeType::CreateFrom(file->start(), file->size()));
                if (active_ == NULL)
                {
                    LOG(ERROR) << "Create RBTree failed.";
                    return -3;
                }
            }
        }
        else
        {
            std::auto_ptr<ListType> list(ListType::RestoreFrom(file->start(), file->size()));
            if (list.get() == NULL)
            {
                list.reset (ListType::CreateFrom(file->start(), file->size()));
                if (list.get() == NULL)
                {
                    LOG(ERROR) << "Create List failed, i[" << i << "]";
                    return -4;
                }
            }
            lists_[i] = list.release();
        }
        mmaps_[i] = file.release();
    }
    return 0;
}

void ArenaServer::Start()
{
    assert (conf_ != NULL);
    loop_.reset (new fx::net::EventLoop);
    server_.reset (new UdpServer(loop_.get(), ip_, port_));
    server_->set_read_callback(boost::bind(&ArenaServer::OnRead, this, _1, _2, _3));
    server_->Start();
    ArenaServer::loop = loop_.get();            /* for signal handler */
    RegisterSignalHandler();
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
        //case arenasvrd::Request_Type_QUERY_SEASON:
        //    ret = OnQuerySeason(request, &response);
        //    break;

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

int ArenaServer::OnQuerySeason(const arenasvrd::Request & req, arenasvrd::Response * res)
{
    (void)req;
    (void)res;
    //assert (res);
    //res->set_status(arenasvrd::Response_Status_OK);
    //res->set_season_remaining_time (conf_->SeasonRemainingTime(fx::base::time::Now()));
    return 0;
}

int ArenaServer::OnFindOpponent(const arenasvrd::Request & req, arenasvrd::Response * res)
{
    unsigned uin = req.self();
    if (uin < 10000) return -1;
    if (req.has_points() == false) return -2;

    unsigned rank = conf_->GetRankByPoints(req.points());
    BOOST_AUTO (iter, lists_.find(rank));
    assert (iter != lists_.end());

    if (conf_->InSeasonTime() == false)
    {
        res->set_status(arenasvrd::Response_Status_NOT_IN_SEASON_TIME);
    }
    else
    {
        while (iter->second->size() == 0        /* empty */
                or (iter->second->size() == 1 and iter->second->Back().uin == uin) /* self only */
                )
        {
            if (iter == lists_.begin()) break;
            else --iter;
        }
        if (iter->second->size() == 0)
        {
            res->set_status(arenasvrd::Response_Status_NO_OPPONENT);
        }
        else if (iter->second->Back().uin == uin)
        {
            assert (iter->second->size() >= 2);
            ListNode self = iter->second->PopBack();            /* pop self out temporarily */
            ListNode opponent = iter->second->Back();
            iter->second->PushBack(self);
            res->set_status(arenasvrd::Response_Status_OK);
            res->set_opponent (opponent.uin);
        }
        else
        {
            ListNode opponent = iter->second->Back();
            res->set_status(arenasvrd::Response_Status_OK);
            res->set_opponent (opponent.uin);
        }
    }
    return 0;
}

int ArenaServer::OnUpdateSelf(const arenasvrd::Request & req, arenasvrd::Response * res)
{
    unsigned uin = req.self();
    if (uin < 10000) return -1;
    if (req.has_points() == false) return -2;
    unsigned point = req.points();

    if (conf_->InSeasonTime() == false)
    {
        res->set_status(arenasvrd::Response_Status_NOT_IN_SEASON_TIME);
        return 0;
    }

    unsigned current_rank = conf_->GetRankByPoints(point);
    DLOG(INFO) << "[active]Get " << uin;
    DLOG(INFO) << "current_rank " << current_rank;
    TreeType::iterator iter = active_->Get(uin);
    ListNode list_node;
    if (iter != active_->end())
    {
        /* unlink from old rank list */
        TreeNode node = iter.Value();
        unsigned old_rank = node.rank;
        BOOST_AUTO (list_iter, lists_.find(old_rank));
        assert (list_iter != lists_.end());
        list_node = list_iter->second->Unlink(node.list_node_id);
        assert (list_node.uin == uin);
    }

    /* fill node info */
    list_node.uin = uin;
    list_node.rank = current_rank;
    list_node.points = point;

    BOOST_AUTO (list_iter, lists_.find(current_rank));
    assert (list_iter != lists_.end());

    /* check new rank list has enough space */
    if (list_iter->second->size() >= kMaxPersonInList)      /* reach upper limit */
    {
        /* remove Least Recently Used node */
        ListNode front = list_iter->second->PopFront();
        DLOG(INFO) << "[active]Delete " << front.uin;
        size_t count = active_->Delete(front.uin);
        assert (count == 1);
    }

    ListType::NodeId list_node_id = list_iter->second->PushBack(list_node);
    if (iter != active_->end())
    {
        /* update info in active */
        iter.Value().rank = current_rank;
    }
    else
    {
        /* new node for this uin */
        TreeNode node;
        node.rank = current_rank;
        node.list_node_id = list_node_id;
        DLOG(INFO) << "[active]Put " << uin;
        bool ok = active_->Put(uin, node);
        assert (ok);
        (void) ok;
    }
    res->set_status(arenasvrd::Response_Status_OK);
    return 0;
}
