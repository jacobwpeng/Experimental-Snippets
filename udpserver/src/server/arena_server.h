/*
 * =====================================================================================
 *
 *       Filename:  arena_server.h
 *        Created:  08/22/14 16:22:50
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __ARENA_SERVER_H__
#define  __ARENA_SERVER_H__

#include <map>
#include <memory>
#include <string>
#include "list.h"
#include "rbtree.h"
#include "arena_message.pb.h"

namespace fx 
{ 
    namespace net { class UdpServer; class EventLoop; } 
    namespace base { class MMapFile; }
}

class ArenaConf;
class ArenaServer
{
    public:
        ArenaServer(const std::string& ip, int port);
        ~ArenaServer();

        int Init(const std::string& conf_path);
        void Start();

    private:
        static void SignalHandler(int signum);
        static fx::net::EventLoop * loop;
        void RegisterSignalHandler();
        int OnRead(const char* in, size_t len, std::string * out);
        int OnFindOpponent(const arenasvrd::Request & req, arenasvrd::Response * res);
        int OnUpdateSelf(const arenasvrd::Request & req, arenasvrd::Response * res);
        void ResetData(uint64_t iteration);

    private:
        struct ListNode
        {
            unsigned uin;
            unsigned points;
            unsigned rank;
        };
        typedef fx::base::container::List<ListNode> ListType;

        struct TreeNode
        {
            unsigned rank;
            ListType::NodeId list_node_id;
        };
        typedef fx::base::container::RBTree<unsigned, TreeNode> MapType;

        std::unique_ptr<fx::net::EventLoop> loop_; /* lives longer than server_ */
        std::unique_ptr<fx::net::UdpServer> server_;
        std::unique_ptr<ArenaConf> conf_;
        std::unique_ptr<MapType> active_;
        std::map<unsigned, std::unique_ptr<fx::base::MMapFile>> mmaps_;
        std::map<unsigned, std::unique_ptr<ListType>> lists_;
        const std::string ip_;
        const int port_;
        static const size_t kMaxPersonInList = 200u;
};

#endif   /* ----- #ifndef __ARENA_SERVER_H__  ----- */
