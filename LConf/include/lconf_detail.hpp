/*
 * =====================================================================================
 *
 *       Filename:  lconf_detail.hpp
 *        Created:  12/01/2013 09:48:33 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __LCONF_DETAIL__
#define  __LCONF_DETAIL__

#include <iostream>
#include <cassert>
#include <lua.hpp>
#include "INode.h"
#include "table_node.h"
#include "scalar_node.h"

namespace detail
{
    static const char* TOP_LEVEL_TABLE_NAME = "conf";
    inline unsigned abs_idx(lua_State* L, int idx);
    INode* ReadNode(lua_State* L, int key_idx, int value_idx);
    TableNode* read_table(lua_State* L, int table_idx, const char* table_name);
    TableNode* read_conf(const char* conf_path);

    inline unsigned abs_idx(lua_State* L, int idx)
    {
        unsigned abs_idx = idx;
        if( idx < 0 )
            abs_idx = lua_gettop(L) + 1 + idx ;
        return abs_idx;
    }

    INode* ReadNode(lua_State* L, int key_idx, int value_idx)
    {
        key_idx = abs_idx(L, key_idx);
        value_idx = abs_idx(L, value_idx);

        lua_pushvalue(L, key_idx);              /* copy the key  */
        string name = lua_tostring(L, -1);
        lua_pop(L, 1);                         /* pop the copyed key */
        INode* pNode = NULL;
        if( lua_istable(L, value_idx) )
        {
            pNode = read_table(L, value_idx, name.c_str());
        }
        else if( lua_isstring(L, value_idx) )
        {
            ScalarNode* pScalarNode = new ScalarNode(name);
            pScalarNode->set_value( lua_tostring(L, value_idx) );
            pNode = pScalarNode;
        }
        else
        {
            /* other type is not allowed */
            assert(false);
        }
        assert(pNode != NULL);
        return pNode;
    }

    TableNode* read_table(lua_State* L, int table_idx, const char* table_name)
    {
        table_idx = abs_idx(L, table_idx);
        unsigned idx = 1;
        TableNode* pTable = new TableNode(table_name);
        /* first push all array part element */
        while(1)
        {
            lua_rawgeti(L, table_idx, idx);
            if( lua_isnil(L, -1) )
            {
                lua_pop(L, 1);
                break;
            }
            else if( lua_isstring(L, -1) )
            {
                /* both string and numbers */
                ScalarNode* pNode = new ScalarNode();
                pNode->set_value(lua_tostring(L, -1));
                pTable->AppendArrayChild( pNode );
            }
            else if( lua_istable(L, -1) )
            {
                INode* pTableNode = read_table(L, -1, NULL);
                pTable->AppendArrayChild( pTableNode );
            }
            else
            {
                /* do not allow other type in conf file */
                assert(false);
            }
            lua_pop(L, 1);
            ++idx;
        }
        /* keep processing hash part */
        lua_pushnil(L);
        while(lua_next(L, table_idx))
        {
            /* key is number, we already processed it! */
            if( lua_type(L, -2) == LUA_TNUMBER )
            {
                if( lua_tonumber(L, -2) > (idx-1) )
                {
                    /* key is number but not in range[1, #table] */
                    INode* node = ReadNode(L, -2, -1);
                    pTable->AppendChild(node);
                }
                else
                {
                    /*  we have processed this key in array part */
                }
            }
            else
            {
                INode* node = ReadNode(L, -2, -1);
                pTable->AppendChild(node);
            }
            lua_pop(L, 1);
        }
        return pTable;
    }

    TableNode* read_conf(const char* conf_path)
    {
        lua_State* L;
        L = luaL_newstate();
        luaopen_base(L);
        int ret = luaL_dofile(L, conf_path);
        assert(ret == 0 );
        if( ret != 0 )
        {
            return NULL;
        }
        lua_getglobal(L, TOP_LEVEL_TABLE_NAME);
        int table_idx = lua_gettop(L);
        TableNode* root = read_table(L, table_idx, TOP_LEVEL_TABLE_NAME);
        lua_close(L);
        return root;
    }
}

#endif   /* ----- #ifndef __LCONF_DETAIL__  ----- */
