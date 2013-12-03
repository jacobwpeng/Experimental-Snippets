/*
 * =====================================================================================
 *
 *       Filename:  scalar_node.h
 *        Created:  12/01/2013 08:34:32 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */


#ifndef  __SCALAR_NODE__
#define  __SCALAR_NODE__

#include "node.h"

class ScalarNode : public INode
{
    public:
        ScalarNode( const string& name = "");
        virtual INode* Lookup(const char* child_name);
        void set_value(const string& value);
        string value() const;

    private:
        string value_;
};

#endif   /* ----- #ifndef __SCALAR_NODE__  ----- */
