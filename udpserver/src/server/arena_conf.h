/*
 * =====================================================================================
 *
 *       Filename:  arena_conf.h
 *        Created:  08/27/14 15:59:07
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __ARENA_CONF_H__
#define  __ARENA_CONF_H__

#include <map>
#include <ctime>
#include <string>
#include <boost/noncopyable.hpp>

class ArenaConf : boost::noncopyable
{
    public:
        static ArenaConf * ParseConf(const std::string & conf_path);

        std::string mmap_path() const { return mmap_path_; }
        size_t mmap_size() const { return mmap_size_; }
        size_t RankNumber() const { return ranks_.size(); }
        unsigned GetRankByPoints(unsigned points) const;
        bool InSeasonTime() const;
        int TimeLeftToNextSeason() const;

    private:
        ArenaConf();
        std::map<unsigned, unsigned> ranks_;     /* max_value -> rank */
        unsigned end_day_;
        time_t zero_point_;
        std::string mmap_path_;
        size_t mmap_size_;
};

#endif   /* ----- #ifndef __ARENA_CONF_H__  ----- */
