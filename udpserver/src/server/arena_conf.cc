/*
 * =====================================================================================
 *
 *       Filename:  arena_conf.cc
 *        Created:  08/27/14 16:01:23
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <memory>
#include "arena_conf.h"
#include "system_time.h"
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <glog/logging.h>

ArenaConf::ArenaConf()
{

}

ArenaConf * ArenaConf::ParseConf(const std::string& cont_path)
{
    std::auto_ptr<ArenaConf> conf(new ArenaConf);
    using boost::property_tree::ptree;

    ptree pt;
    try
    {
        boost::property_tree::read_xml(cont_path, pt, boost::property_tree::xml_parser::no_comments);
        conf->mmap_path_ = pt.get<std::string>("global.mmap.<xmlattr>.path");
        conf->mmap_size_ = pt.get<unsigned>("global.mmap.<xmlattr>.size");

        unsigned last_id = 0;
        unsigned last_max_value = 0;
        BOOST_FOREACH (const ptree::value_type & child, pt.get_child("ranks"))
        {
            if (child.first != "rank") continue;
            const ptree & rank = child.second;
            unsigned id = rank.get<unsigned>("<xmlattr>.id");
            unsigned max_value = rank.get<unsigned>("<xmlattr>.max_value");

            if (last_id + 1 != id)
            {
                LOG(ERROR) << "id is not contiguous";
                return NULL;
            }
            last_id = id;

            if (max_value <= last_max_value)
            {
                LOG(ERROR) << "max_value is not linear increasing";
                return NULL;
            }
            last_max_value = max_value;
            conf->ranks_[max_value] = id;
        }

        BOOST_FOREACH (const ptree::value_type & child, pt.get_child("seasons"))
        {
            if (child.first != "season") continue;
            const ptree & season = child.second;

            std::string start = season.get<std::string>("<xmlattr>.start");
            std::string end = season.get<std::string>("<xmlattr>.end");

            time_t start_time = fx::base::time::StringToTime(start.c_str());
            if (start_time == -1)
            {
                LOG(ERROR) << "invalid start_time[" << start << "]";
                return NULL;
            }

            time_t end_time = fx::base::time::StringToTime(end.c_str());
            if (end_time == -1)
            {
                LOG(ERROR) << "invalid end_time[" << end << "]";
                return NULL;
            }

            conf->seasons_[end_time] = start_time;
        }
    }
    catch(boost::property_tree::ptree_error & error)
    {
        LOG(ERROR) << "ParseConf failed, " << error.what();
        return NULL;
    }
    return conf.release();
}

unsigned ArenaConf::GetRankByPoints(unsigned points) const
{
    BOOST_AUTO (iter, ranks_.upper_bound(points));
    assert (iter != ranks_.end());
    return iter->second;
}

bool ArenaConf::InSeasonTime() const
{
    time_t now = fx::base::time::Now() / (1000000);
    BOOST_AUTO (iter, seasons_.lower_bound(now));
    assert (iter != seasons_.end());
    return now >= iter->second;
}
