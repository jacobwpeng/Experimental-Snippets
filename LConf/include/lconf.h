/*
 * =====================================================================================
 *
 *       Filename:  lconf.h
 *        Created:  11/21/2013 09:41:48 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  lconf library api header
 *
 * =====================================================================================
 */
#ifndef  __LCONF_H__
#define  __LCONF_H__

#include <string>
#include "table_node.h"
using std::string;

class LConf
{
    public:
        typedef TableNode::NodeArray::iterator ArrayIterator;
        typedef TableNode::NodeArrayRange ArrayRange;
        LConf(const char* conf_path);
        LConf(const string& conf_path);
        string Lookup(const char* node_name) const;
        ArrayRange LookupArray(const char* array_name);
        ArrayRange LookupTable(const char* table_name);

        bool IsValid() const;
    private:
        int init();
        bool valid_;
        TableNode* root_;
        string conf_path_;
};

#endif   /* ----- #ifndef __LCONF_H__  ----- */
