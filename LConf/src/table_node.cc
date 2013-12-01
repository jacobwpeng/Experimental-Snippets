/*
 * =====================================================================================
 *
 *       Filename:  table_node.cc
 *        Created:  12/01/2013 08:46:13 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <cassert>
#include "table_node.h"

TableNode::TableNode(const char* name)
    :array_part_len_(0)
{
    this->is_scalar_ = false;
    if( name ) this->name_.assign( name );
}

INode* TableNode::Lookup(const char* child_name)
{
    string name( child_name );
    size_t pos = name.find_first_of('.');
    if( pos == string::npos )
    {
        return this->Children( child_name );
    }
    string key = name.substr(0, pos);
    string left = name.substr(pos + 1);
    INode* child = this->Children(key.c_str());
    if(child) return child->Lookup(left.c_str());
    else return NULL;
}

INode* TableNode::Children(const char* child_name)
{
    /* todo : 干掉遍历一遍 */
    for(NodeArray::iterator iter = this->children_.begin(); iter != this->children_.end(); ++iter)
    {
        if( (*iter)->name() == child_name ) return *iter;
    }
    return NULL;
}

unsigned TableNode::ArrayChildrenLen() const
{
    return this->array_part_len_;
}

TableNode::NodeArrayRange TableNode::ArrayChildrenRange()
{
    if( this->ArrayChildrenLen() == 0 )
    {
        return make_pair(this->children_.end(), this->children_.end());
    }
    else
    {
        NodeArray::iterator iter = this->children_.begin();
        advance(iter, this->array_part_len_);
        return make_pair(this->children_.begin(), iter);
    }
}

INode* TableNode::ArrayChildren(unsigned idx)
{
    if( idx >= this->array_part_len_ ) return NULL;
    assert( this->children_.size() >= this->array_part_len_ );
    return this->children_[ idx ];
}

TableNode::NodeArrayRange TableNode::ChildrenRange()
{
    return make_pair( this->children_.begin(), this->children_.end() );
}

void TableNode::AppendChild(INode* child)
{
    this->children_.push_back( child );
}

void TableNode::AppendArrayChild(INode* child)
{
    ++array_part_len_;
    this->AppendChild(child);
}
