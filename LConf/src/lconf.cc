#include <iostream>
#include <string>
#include "INode.h"
#include "table_node.h"
#include "scalar_node.h"
#include "lconf.h"
#include "lconf_detail.hpp"

using std::string;

LConf::LConf( const char* conf_path )
    :conf_path_(conf_path)
{
    this->init();
}

LConf::LConf( const string& conf_path )
    :conf_path_(conf_path)
{
    this->init();
}

string LConf::Lookup(const char* node_name) const
{
    INode* pNode = this->root_->Lookup(node_name);
    assert( pNode != NULL );
    ScalarNode* p = dynamic_cast<ScalarNode*>(pNode);
    assert( p != NULL );
    return p->value();
}

bool LConf::IsValid() const
{
    return this->valid_;
}

LConf::ArrayRange LConf::LookupArray(const char* array_name)
{
    INode* pNode = NULL;
    if( NULL == array_name )
    {
        pNode = this->root_;
    }
    else
    {
        pNode = this->root_->Lookup(array_name);
    }
    assert( pNode != NULL );
    TableNode* p = dynamic_cast<TableNode*>(pNode);
    assert(p != NULL);
    return p->ArrayChildrenRange();
}

int LConf::init()
{
    this->root_ = detail::read_conf(this->conf_path_.c_str());
    this->valid_ = (this->root_ != NULL);
    return !this->valid_;
}

