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
#include <iostream>
#include <gtest/gtest.h>
#include <boost/scoped_ptr.hpp>
#include "mmap_file.h"

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

    std::unique_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    EXPECT_TRUE (rb != NULL);
    EXPECT_EQ (0, rb->size());
    //RBTree header = 32, MemoryList header = 48Bytes, RBNode size = 24Bytes, Slice pointer = 8Bytes
    //max_size = floor((65536 - 32 - 48) / (24 + 8)) == 2045;
    EXPECT_EQ ((kBufSize - fx::base::MemoryList::kHeaderSize - MapType::kHeaderSize) / (24 + 8), rb->max_size());
}

TEST_F(RBTreeUnittest, insert)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    std::unique_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    unsigned kMaxRandomNumber = 1000000;
    auto res = rb->insert(std::make_pair(kMaxRandomNumber + 1, kMaxRandomNumber + 2));
    EXPECT_EQ (rb->size(), 1);
    for (unsigned i = 2; ; ++i)
    {
        unsigned key, val;
        key = val = rand() % kMaxRandomNumber;
        res = rb->insert(std::make_pair(key, val));
        if (res.first == rb->end()) break;
    }

    EXPECT_EQ (rb->size(), rb->max_size());

    res = rb->insert(std::make_pair(kMaxRandomNumber + 1, kMaxRandomNumber + 3));
    EXPECT_TRUE (res.first != rb->end());      /* valid iter */
    EXPECT_FALSE (res.second);                           /* overwrite exists value */
    res = rb->insert(std::make_pair(kMaxRandomNumber + 2, kMaxRandomNumber));
    EXPECT_TRUE (res.first == rb->end());
    EXPECT_FALSE (res.second);                       /* try to insert new key when no space left */
}

TEST_F(RBTreeUnittest, operator_bracket)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    std::unique_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    (*rb)[0] = 2;
    EXPECT_EQ(1u, rb->size());
    auto iter = rb->find(0);
    ASSERT_TRUE(iter != rb->end());
    EXPECT_EQ(iter->second, 2u);

    (*rb)[0] = 3;
    EXPECT_EQ(iter->second, 3u);
    EXPECT_EQ(1u, rb->size());

    auto space_left = rb->max_size() - rb->size();
    for (auto idx = 0; idx != space_left; ++idx)
    {
        (*rb)[idx + 1] = idx;
    }
    EXPECT_EQ(rb->max_size(), rb->size());

    EXPECT_THROW((*rb)[space_left + 1] = space_left, std::bad_alloc);
}

TEST_F(RBTreeUnittest, find)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    std::unique_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    MapType::iterator rb_iter = rb->find(0);
    EXPECT_EQ (rb_iter, rb->end());

    std::map<unsigned, unsigned> m;
    while (1)
    {
        unsigned key, val;
        key = rand();
        val = rand();
        auto res = rb->insert(std::make_pair(key, val));
        if (res.first == rb->end()) break;
        m[key] = val;
    }

    EXPECT_EQ (m.size(), rb->size());
    std::map<unsigned, unsigned>::const_iterator diff = m.end();
    for (std::map<unsigned, unsigned>::const_iterator iter = m.begin();
            iter != m.end();
            ++iter)
    {
        if (iter->first != iter->second) { diff = iter; }
        rb_iter = rb->find(iter->first);
        ASSERT_TRUE (rb_iter != rb->end());
        EXPECT_EQ (iter->second, rb_iter->second);
    }
    ASSERT_NE (diff, m.end());
    rb_iter = rb->find(diff->first);
    rb_iter->second = diff->first;
    EXPECT_EQ(rb->find(diff->first)->first, diff->first);
}

TEST_F(RBTreeUnittest, erase)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    {
        std::unique_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
        ASSERT_TRUE (rb != NULL);
        auto res = rb->insert(std::make_pair(8, 1));
        EXPECT_TRUE (res.second);
        res = rb->insert(std::make_pair(4, 1));
        EXPECT_TRUE (res.second);
        res = rb->insert(std::make_pair(1, 1));
        EXPECT_TRUE (res.second);
        res = rb->insert(std::make_pair(2, 1));
        EXPECT_TRUE (res.second);
        res = rb->insert(std::make_pair(3, 1));
        EXPECT_TRUE (res.second);
        res = rb->insert(std::make_pair(9, 1));
        EXPECT_TRUE (res.second);

        EXPECT_EQ (rb->size(), 6);

        EXPECT_EQ (1u, rb->erase(8));
        res = rb->insert(std::make_pair(7, 1));
        EXPECT_EQ(true, res.second);

        EXPECT_EQ (1u, rb->erase(4));
        res = rb->insert(std::make_pair(0, 1));
        EXPECT_TRUE (res.second);

        EXPECT_EQ (1, rb->erase(1));
        res = rb->insert(std::make_pair(6, 1));
        EXPECT_TRUE (res.second);
        EXPECT_EQ (1, rb->erase(2));
    }
    {
        std::unique_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
        ASSERT_TRUE (rb != NULL);

        rb->insert(std::make_pair(1, 1));
        rb->insert(std::make_pair(2, 2));
        rb->insert(std::make_pair(3, 3));

        EXPECT_EQ(3u, rb->size());
        auto iter = rb->find(2);
        EXPECT_NE(iter, rb->end());
        EXPECT_EQ(1u, rb->erase(iter));
        iter = rb->begin();
        EXPECT_EQ(iter->first, 1);
        ++iter;
        EXPECT_EQ(iter->first, 3);
        ++iter;
        EXPECT_EQ(iter, rb->end());
    }
}

TEST_F(RBTreeUnittest, clear)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    std::unique_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);
    const unsigned kMaxRandomNumber = 10000;

    while (1)
    {
        unsigned key, val;
        key = val = rand() % kMaxRandomNumber;
        auto res = rb->insert(std::make_pair(key, val));
        if (res.first == rb->end()) break;
    }
    ASSERT_EQ (rb->size(), rb->max_size());
    auto res = rb->insert(std::make_pair(kMaxRandomNumber + 1, 1));
    ASSERT_TRUE (res.first == rb->end());
    rb->clear();
    EXPECT_EQ (rb->size(), 0);
    res = rb->insert(std::make_pair(kMaxRandomNumber + 1, 1));
    EXPECT_TRUE (res.second);
}

TEST_F(RBTreeUnittest, iterator)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    std::unique_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    rb->insert(std::make_pair(2, 2));
    rb->insert(std::make_pair(1, 1));
    rb->insert(std::make_pair(3, 3));
    rb->insert(std::make_pair(4, 4));

    ASSERT_EQ (rb->size(), 4u);

    auto iter = rb->begin();
    EXPECT_EQ(iter->first, 1u);
    ++iter;
    EXPECT_EQ(iter->first, 2u);
    ++iter;
    EXPECT_EQ(iter->first, 3u);
    ++iter;
    EXPECT_EQ(iter->first, 4u);

    iter = rb->find(3);
    ASSERT_TRUE (iter != rb->end());
    EXPECT_EQ(iter->second, 3);

    auto count = rb->erase(2);
    EXPECT_EQ(count, 1);
    //test if iter has been invalidated
    EXPECT_EQ(iter->first, 3);
    EXPECT_EQ(iter->second, 3);
}


TEST_F(RBTreeUnittest, const_iterator)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    std::unique_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
    ASSERT_TRUE (rb != NULL);

    const MapType * const_tree = rb.get();
    EXPECT_TRUE (const_tree->begin() == const_tree->end());
    rb->insert(std::make_pair(1, 2));
    rb->insert(std::make_pair(3, 4));
    EXPECT_TRUE (const_tree->begin() != const_tree->end());
    MapType::const_iterator iter = const_tree->find(3);
    ASSERT_TRUE(iter != const_tree->end());
    EXPECT_EQ (iter->second, 4);
    iter = rb->begin();
}

TEST_F(RBTreeUnittest, RestoreFrom)
{
    const size_t kBufSize = 1 << 16;
    char buf[kBufSize];
    typedef RBTree<unsigned, unsigned> MapType;

    std::map<unsigned, unsigned> m;
    {
        std::unique_ptr< MapType > rb(MapType::CreateFrom(buf, kBufSize));
        ASSERT_TRUE (rb != NULL);

        while (1)
        {
            unsigned key, val;
            key = rand();
            val = rand();
            auto res = rb->insert(std::make_pair(key, val));
            if (res.first == rb->end()) break;
            m[key] = val;
        }
        EXPECT_EQ (m.size(), rb->size());
    }
    char newbuf[kBufSize * 2];                  /* larger buffer */
    ::memcpy(newbuf, buf, sizeof(buf));
    {
        std::unique_ptr< MapType > rb(MapType::RestoreFrom(newbuf, sizeof(newbuf)));
        ASSERT_TRUE (rb != NULL);
        EXPECT_EQ (m.size(), rb->size());
        for (std::map<unsigned, unsigned>::const_iterator iter = m.begin();
                iter != m.end();
                ++iter)
        {
            auto rb_iter = rb->find(iter->first);
            ASSERT_TRUE (rb_iter != rb->end());
            EXPECT_EQ (iter->second, iter->second);
        }
    }
}
