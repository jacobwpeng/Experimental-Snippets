/*
 * =====================================================================================
 *
 *       Filename:  test.cc
 *        Created:  12/06/2013 10:08:15 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <gtest/gtest.h>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;

#ifndef CONF_PATH
#define CONF_PATH "/home/work/repos/test/test-ptree/conf"
#endif

TEST(ptree, test_read_attributes)
{
    static const char* filename = CONF_PATH"/exchange_gifts.xml";
    using boost::property_tree::ptree;
    ptree pt;
    read_xml(filename, pt);

    unsigned max_day_request_num = pt.get<unsigned>("root.base_conf.max_day_request_num.<xmlattr>.val");
    ASSERT_EQ( max_day_request_num, 30 );

    BOOST_FOREACH(ptree::value_type &v, pt.get_child("root.goods_conf.goods_list"))
    {
        unsigned goods_id = v.second.get<unsigned>("<xmlattr>.id");
        unsigned goods_count = v.second.get<unsigned>("<xmlattr>.count");
    }
}
