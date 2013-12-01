/*
 * =====================================================================================
 *
 *       Filename:  scalar_node.cc
 *        Created:  12/01/2013 08:35:52 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "scalar_node.h"

ScalarNode::ScalarNode(const string& name)
{
    this->is_scalar_ = true;
    this->name_ = name;
}

INode* ScalarNode::Lookup(const char* node_name)
{
    return this->name_ == node_name ? this : NULL;
}

void ScalarNode::set_value(const string& value)
{
    this->value_ = value;
}

string ScalarNode::value() const
{
    return this->value_;
}
