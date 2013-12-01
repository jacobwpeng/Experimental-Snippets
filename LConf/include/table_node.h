/*
 * =====================================================================================
 *
 *       Filename:  table_node.h
 *        Created:  12/01/2013 08:34:06 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */


#ifndef  __TABLE_NODE__
#define  __TABLE_NODE__

#include "INode.h"
#include <vector>
using std::pair;
using std::vector;

class TableNode : public INode
{
    public:
        typedef vector<INode*> NodeArray;
        typedef pair<NodeArray::iterator, NodeArray::iterator> NodeArrayRange;
        TableNode(const char* name = NULL);
        virtual ~TableNode() {}

        /* donot lookup in array part */
        virtual INode* Lookup(const char* child_name);
        virtual vector<INode*> Children();
        virtual INode* Children(const char* child_name);
        virtual INode* ArrayChildren(unsigned idx);
        virtual unsigned ArrayChildrenLen() const;
        virtual NodeArrayRange ArrayChildrenRange();
        virtual void AppendChild(INode* child);
        virtual void AppendArrayChild(INode* child);
    private:
        unsigned array_part_len_;
        NodeArray children_;

};

#endif   /* ----- #ifndef __TABLE_NODE__  ----- */
