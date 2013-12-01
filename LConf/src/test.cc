/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  11/21/2013 03:01:40 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  test app for lconf
 *
 * =====================================================================================
 */

//#include <map>
//#include <lua.hpp>
//#include <boost/optional.hpp>
//#include <boost/format.hpp>
//
//using namespace std;
//using boost::optional;
//using boost::str;
//using boost::format;
//
//map<string, optional<string>> confs;
//
//int Usage(int argc, char* argv[])
//{
//    cerr << "Usage : " << argv[0] << " lua_file " << '\n';
//    return -1;
//}
//
//string make_prefix(lua_State* L, int key_idx, const string& cur_prefix)
//{
//    string new_prefix;
//    int abs_idx = lua_gettop(L) + 1 + key_idx;
//    if( lua_isnumber(L, abs_idx) )
//    {
//        int num = static_cast<int>(lua_tonumber(L, abs_idx));
//        if( cur_prefix.empty() ) new_prefix = to_string(num);
//        else new_prefix = str( format("%s[%d]") % cur_prefix % num);
//    }
//    else if( lua_isstring(L, abs_idx) )
//    {
//        lua_pushvalue(L, abs_idx);
//        string key_name = lua_tostring(L, -1);
//        lua_pop(L, 1);
//        if( cur_prefix.empty() ) new_prefix = key_name;
//        else new_prefix = str(format("%s.%s") % cur_prefix % key_name);
//    }
//    else
//    {
//        BOOST_ASSERT(false);
//    }
//    return new_prefix;
//}
//
//void read_table(lua_State* L, int table_idx, const string& prefix)
//{
//    if( table_idx < 0 )
//        table_idx = lua_gettop(L) + 1 + table_idx;
//    lua_pushnil(L);
//    while( lua_next(L, table_idx) )
//    {
//        /* value is not a table, so we just add it to the confs */
//        if( lua_isstring(L, -1) )
//        {
//            string key = make_prefix(L, -2, prefix);
//            confs[key].reset( lua_tostring(L, -1) );
//        }
//        else if( lua_istable(L, -1) )
//        {
//            string new_prefix = make_prefix(L, -2, prefix);
//            read_table(L, -1, new_prefix);
//        }
//        else
//        {
//            cerr << "Unknown type" << '\n';
//        }
//        lua_pop(L, 1);
//    }
//}
//
//void read_conf(lua_State* L)
//{
//    lua_getglobal(L, "conf");
//    int table_idx = lua_gettop(L);
//    read_table(L, table_idx, "conf");
//    for( const auto& x : confs )
//    {
//        cout << x.first;
//        if( x.second )
//            cout << " = " << x.second.get() << '\n';
//        else
//            cout << "not exists!" << '\n';
//    }
//    cout << "********************************************************************************" << '\n';
//}
//
//optional<string> get_conf(const char* attr)
//{
//    optional<string> res;
//    if( confs[attr] )
//        res = confs[attr];
//    return res;
//}
//
//void get_conf_array(const char* attr, unsigned start_idx, vector<string>* pResults)
//{
//    assert( start_idx >= 1 && pResults != NULL);
//    pResults->clear();
//    unsigned idx = start_idx;
//    while( 1 )
//    {
//        string key = str(format("%s[%u]") % attr % idx);
//        cout << key << endl;
//        optional<string> res = get_conf(key.c_str());
//        if( res ) pResults->push_back( res.get() );
//        else break;
//        ++idx;
//    }
//}
//
//void get_conf_array(const char* attr, vector<string>* pResults)
//{
//    get_conf_array(attr, 1, pResults);
//}
#include <cassert>
#include <iostream>
#include "lconf.h"
#include "scalar_node.h"
using std::cout;

int main(int argc, char* argv[])
{
    if( argc != 3 )
    {
        cout << "Usage : " << argv[0] << " lua_file attr" << '\n';
        return -1;
    }
    LConf conf(argv[1]);
    if( conf.IsValid() )
    {
        //cout << conf.Lookup(argv[2]) << '\n';
        cout << conf.Lookup("costs.walker31") << '\n';
        LConf::ArrayRange range = conf.LookupArray("infos");
        LConf::ArrayIterator iter = range.first;
        while( iter != range.second )
        {
            ScalarNode* p = dynamic_cast<ScalarNode*>( (*iter)->Lookup("name") );
            assert(p);
            cout << "name = " << p->name() << ", value = " << p->value() << '\n';
            ++iter;
        }

    }
    return 0;
}

