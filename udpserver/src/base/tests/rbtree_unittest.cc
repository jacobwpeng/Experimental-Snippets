/*
 * =====================================================================================
 *
 *       Filename:  rbtree_unittest.cc
 *        Created:  08/24/14 13:18:08
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "rbtree.h"
#include <gtest/gtest.h>
#include <boost/scoped_ptr.hpp>

using fx::base::container::RBTree;

class RBTreeUnittest : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {

        }

        virtual void TearDown()
        {

        }
};

TEST_F(RBTreeUnittest, Create)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    EXPECT_TRUE (rb != NULL);
    EXPECT_EQ (0, rb->size());
    //RBTree header = 24Bytes, MemoryList header = 48Bytes, RBNode size = 24Bytes, Slice pointer = 8Bytes
    //capacity = floor((65536 - 24 - 48) / (24 + 8)) == 2045;
    EXPECT_EQ (2045, rb->capacity());
}

TEST_F(RBTreeUnittest, Put)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    for (unsigned i = 0; ; ++i)
    {
        bool ok = rb->Put(i, i);
        if (not ok) break;
        EXPECT_EQ (rb->size(), i+1);
    }

    EXPECT_EQ (rb->size(), rb->capacity());
    EXPECT_EQ (rb->depth(), 10);
}
