/*
 * =====================================================================================
 *
 *       Filename:  INode.h
 *        Created:  12/01/2013 07:18:10 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  Base class for all conf settings
 *
 * =====================================================================================
 */


#ifndef  __INODE__
#define  __INODE__

#include <string>
using std::string;

class INode
{
    public:
        /*-----------------------------------------------------------------------------
         *  根据节点名称查找对应节点
         *-----------------------------------------------------------------------------*/
        virtual INode* Lookup(const char* child_name) = 0;
        virtual string name() const {return this->name_;}
        virtual bool IsTable() const { return !this->is_scalar_; }
        virtual bool IsScalar() const { return this->is_scalar_; }
        virtual ~INode() {}
    protected:
        string name_;
        bool is_scalar_;
};

#endif   /* ----- #ifndef __INODE__  ----- */
