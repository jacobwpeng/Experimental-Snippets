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
#include <ctime>
#include <cstdlib>
#include <map>
#include <gtest/gtest.h>
#include <boost/scoped_ptr.hpp>

using fx::base::container::RBTree;

class RBTreeUnittest : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            ::srand(::time(NULL));
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
    //RBTree header = 32, MemoryList header = 48Bytes, RBNode size = 24Bytes, Slice pointer = 8Bytes
    //capacity = floor((65536 - 32 - 48) / (24 + 8)) == 2045;
    EXPECT_EQ (2045, rb->capacity());
}

TEST_F(RBTreeUnittest, Put)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    unsigned kMaxRandomNumber = 1000000;
    rb->Put(kMaxRandomNumber + 1, kMaxRandomNumber + 2);
    EXPECT_EQ (rb->size(), 1);
    for (unsigned i = 2; ; ++i)
    {
        unsigned key, val;
        key = val = rand() % 1000000;
        bool ok = rb->Put(key, val);
        if (not ok) break;
    }

    EXPECT_EQ (rb->size(), rb->capacity());
    EXPECT_LE (rb->depth(), 10);

    bool ok = rb->Put(kMaxRandomNumber + 1, kMaxRandomNumber + 3);
    EXPECT_TRUE (ok);                           /* overwrite exists value */
    ok = rb->Put(kMaxRandomNumber + 2, kMaxRandomNumber);
    EXPECT_TRUE (not ok);                       /* try to insert new key when no space left */
}

TEST_F(RBTreeUnittest, Get)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    MapType::iterator rb_iter = rb->Get(0);
    EXPECT_EQ (rb_iter, rb->end());

    std::map<unsigned, unsigned> m;
    while (1)
    {
        unsigned key, val;
        key = rand();
        val = rand();
        if (not rb->Put(key, val)) break;
        m[key] = val;
    }

    EXPECT_EQ (m.size(), rb->size());
    EXPECT_LE (rb->depth(), 10);
    std::map<unsigned, unsigned>::const_iterator diff = m.end();
    for (std::map<unsigned, unsigned>::const_iterator iter = m.begin();
            iter != m.end();
            ++iter)
    {
        if (iter->first != iter->second) { diff = iter; }
        MapType::iterator rb_iter = rb->Get(iter->first);
        ASSERT_NE (rb_iter, rb->end());
        EXPECT_EQ (iter->second, rb_iter.Value());
    }
    ASSERT_NE (diff, m.end());
    rb_iter = rb->Get(diff->first);
    rb_iter.Value() = diff->first;
    EXPECT_EQ(rb->Get(diff->first).Value(), diff->first);
}

TEST_F(RBTreeUnittest, Delete)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    bool ok = rb->Put(2191195, 1001);
    ASSERT_TRUE (ok);
    ok = rb->Put(2178079, 0);
    ASSERT_TRUE (ok);
    ASSERT_EQ (rb->size(), 2);

    size_t d = rb->Delete(2191195);
    EXPECT_EQ (d, 1);
    EXPECT_EQ (rb->size(), 1);

    d = rb->Delete(2191195);
    EXPECT_EQ (d, 0);
    EXPECT_EQ (rb->size(), 1);

    d = rb->Delete(2178079);
    EXPECT_EQ (d, 1);
    EXPECT_EQ (rb->size(), 0);

    d = rb->Delete(7890);
    EXPECT_EQ (d, 0);
    EXPECT_EQ (rb->size(), 0);
}

TEST_F(RBTreeUnittest, Clear)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    while (1)
    {
        unsigned key, val;
        key = val = rand();
        bool ok = rb->Put(key, val);
        if (not ok) break;
    }
    ASSERT_EQ (rb->size(), rb->capacity());
    ASSERT_TRUE (false == rb->Put(1, 2));
    rb->Clear();
    EXPECT_EQ (rb->size(), 0);
    EXPECT_EQ (rb->depth(), 0);
    EXPECT_TRUE (rb->Put(1, 2));
}

TEST_F(RBTreeUnittest, iterator)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    EXPECT_EQ (rb->begin(), rb->end());
    for (unsigned i = 0; ; ++i)
    {
        if (rb->Put(i, i) == false) break;
    }
    EXPECT_EQ (2045, rb->size());

    unsigned idx = 0;
    for (MapType::iterator iter = rb->begin(); iter != rb->end(); ++iter)
    {
        ASSERT_EQ (idx, iter.Key());
        ASSERT_EQ (idx, iter.Value());
        iter.Value() = idx + 1;
        ++idx;
    }

    idx = 0;
    for (MapType::iterator iter = rb->begin(); iter != rb->end(); ++iter)
    {
        ASSERT_EQ (idx, iter.Key());
        ASSERT_EQ (idx+1, iter.Value());
        ++idx;
    }
}

TEST_F(RBTreeUnittest, const_iterator)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    const MapType * const_tree = rb.get();
    EXPECT_EQ (const_tree->begin(), const_tree->end());
    rb->Put(1, 2);
    rb->Put(3, 4);
    EXPECT_NE (const_tree->begin(), const_tree->end());
    MapType::const_iterator iter = const_tree->Get(3);
    EXPECT_EQ (iter.Value(), 4);
    iter = rb->begin();
}

TEST_F(RBTreeUnittest, RestoreFrom)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    std::map<unsigned, unsigned> m;
    {
        boost::scoped_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
        ASSERT_TRUE (rb != NULL);

        while (1)
        {
            unsigned key, val;
            key = rand();
            val = rand();
            if (not rb->Put(key, val)) break;
            m[key] = val;
        }
        EXPECT_EQ (m.size(), rb->size());
        EXPECT_LE (rb->depth(), 10);
    }
    char newbuf[kBufSize * 2];                  /* larger buffer */
    ::memcpy(newbuf, buf, sizeof(buf));
    {
        boost::scoped_ptr< MapType > rb(MapType::RestoreFrom(newbuf, sizeof(newbuf)));
        ASSERT_TRUE (rb != NULL);
        EXPECT_EQ (m.size(), rb->size());
        for (std::map<unsigned, unsigned>::const_iterator iter = m.begin();
                iter != m.end();
                ++iter)
        {
            EXPECT_EQ (iter->second, rb->Get(iter->first).Value());
        }
    }
}
