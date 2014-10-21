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
#include <boost/date_time.hpp>
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
        conf->end_day_ = pt.get<unsigned>("global.switch_day.<xmlattr>.val");
        auto zero_point = pt.get<std::string>("global.zero_point.<xmlattr>.val");
        conf->zero_point_ = fx::base::time::StringToTime(zero_point.c_str());

        unsigned last_id = 0;
        unsigned last_max_value = 0;
        for (const ptree::value_type & child : pt.get_child("ranks"))
        {
            if (child.first != "rank") continue;
            const ptree & rank = child.second;
            unsigned id = rank.get<unsigned>("<xmlattr>.id");
            unsigned max_value = rank.get<unsigned>("<xmlattr>.max");

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
    auto iter = ranks_.upper_bound(points);
    assert (iter != ranks_.end());
    return iter->second;
}

bool ArenaConf::InSeasonTime() const
{
    auto now = boost::posix_time::second_clock::local_time();
    auto d = now.date();

    boost::posix_time::ptime start_time( boost::gregorian::date(d.year(), d.month(), 1), boost::posix_time::hours(6));
    boost::posix_time::ptime end_time( boost::gregorian::date(d.year(), d.month(), end_day_), boost::posix_time::hours(6));

    boost::posix_time::time_period tp(start_time, end_time);

    return tp.contains(now);
}

int ArenaConf::TimeLeftToNextSeason() const
{
    auto now = boost::posix_time::second_clock::local_time();
    auto d = now.date();

    boost::posix_time::ptime this_month_start_time (boost::gregorian::date(d.year(), d.month(), 1), boost::posix_time::hours(6));
    boost::posix_time::ptime next_month_start_time = this_month_start_time + boost::gregorian::months(1);

    if (now < this_month_start_time) return (this_month_start_time - now).total_seconds();
    else return (next_month_start_time - now).total_seconds();
}
