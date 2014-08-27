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
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include "list.h"
#include "rbtree.h"
#include "arena_message.pb.h"

namespace fx 
{ 
    namespace net { class UdpServer; class EventLoop; } 
    namespace base 
    { 
        class MMapFile;
    }
}

class ArenaConf;
class ArenaServer : boost::noncopyable
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
        int OnQuerySeason(const arenasvrd::Request & req, arenasvrd::Response * res);
        int OnFindOpponent(const arenasvrd::Request & req, arenasvrd::Response * res);
        int OnUpdateSelf(const arenasvrd::Request & req, arenasvrd::Response * res);

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

        boost::scoped_ptr<fx::net::EventLoop> loop_; /* lives longer than server_ */
        boost::scoped_ptr<fx::net::UdpServer> server_;
        boost::scoped_ptr<ArenaConf> conf_;
        typedef fx::base::container::RBTree<unsigned, TreeNode> TreeType;
        boost::scoped_ptr<TreeType> active_;
        std::map<unsigned, fx::base::MMapFile*> mmaps_;
        std::map<unsigned, ListType*> lists_;
        const std::string ip_;
        const int port_;
        const size_t kMaxPersonInList;
};

#endif   /* ----- #ifndef __ARENA_SERVER_H__  ----- */
