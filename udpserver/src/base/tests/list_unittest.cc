/*
 * =====================================================================================
 *
 *       Filename:  list_unittest.cc
 *        Created:  08/25/14 22:49:30
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <ctime>
#include <cstdlib>
#include <vector>
#include <gtest/gtest.h>
#include <memory>

#include "list.h"

using fx::base::container::List;

class ListUnittest : public ::testing::Test
{
    protected:
        using ListType = List<int>;
        virtual void SetUp()
        {
            ::srand(::time(NULL));
            list = std::move(ListType::CreateFrom(buf, kBufSize));
        }

        virtual void TearDown()
        {
            memset(buf, 0xff, sizeof(buf));
        }

        static const size_t kBufSize = 1 << 16;
        char buf[kBufSize];
        std::unique_ptr<ListType> list;
};

TEST_F(ListUnittest, Create)
{
    EXPECT_TRUE (list != NULL);
}

TEST_F(ListUnittest, CreateFailed)
{
    typedef List<unsigned> ListType;
    const size_t kBufSize = ListType::kHeaderSize + fx::base::MemoryList::kHeaderSize - 1;
    char buf[kBufSize];

    std::unique_ptr<ListType> list = ListType::CreateFrom(buf, kBufSize);
    EXPECT_TRUE (list == NULL);
}

TEST_F(ListUnittest, CreateEmpty)
{
    typedef List<unsigned> ListType;
    const size_t kBufSize = ListType::kHeaderSize + fx::base::MemoryList::kHeaderSize;
    char buf[kBufSize];

    std::unique_ptr<ListType> list = ListType::CreateFrom(buf, kBufSize);
    EXPECT_TRUE (list != NULL);
    EXPECT_EQ (list->size(), 0);
    EXPECT_EQ (list->capacity(), 0);
}

TEST_F(ListUnittest, push_back)
{
    ASSERT_TRUE (list != NULL);
    EXPECT_EQ (list->size(), 0);
    //ElementSize = 4 + 2 * 4, ExtraSpace = 2 * 4;
    //capacity = (65536 - List::kHeaderSize - MemoryList::kHeaderSize) / (20) == 5455
    EXPECT_EQ (list->capacity(), (kBufSize - ListType::kHeaderSize - fx::base::MemoryList::kHeaderSize) / 20);

    while (ListType::kInvalidNodeId != list->push_back(rand()));
    EXPECT_EQ (list->size(), list->capacity());
}

TEST_F(ListUnittest, push_front)
{
    ASSERT_TRUE (list != NULL);

    while (ListType::kInvalidNodeId != list->push_front(rand()));
    EXPECT_EQ (list->size(), list->capacity());
}

TEST_F(ListUnittest, pop_back)
{
    ASSERT_TRUE (list != NULL);

    std::vector<unsigned> v;
    while (1)
    {
        unsigned val = rand();
        if (ListType::kInvalidNodeId == list->push_front(val)) break;
        else v.push_back(val);
    }
    ASSERT_EQ (list->size(), v.size());
    for (size_t i = 0; i != v.size(); ++i)
    {
        EXPECT_EQ (v[i], list->pop_back());
        EXPECT_EQ (list->size(), v.size() - i - 1);
    }
    EXPECT_EQ (list->size(), 0);
}

TEST_F(ListUnittest, pop_front)
{
    ASSERT_TRUE (list != NULL);

    std::vector<unsigned> v;
    while (1)
    {
        unsigned val = rand();
        if (ListType::kInvalidNodeId == list->push_back(val)) break;
        else v.push_back(val);
    }
    ASSERT_EQ (list->size(), v.size());
    for (size_t idx = 0; idx != v.size(); ++idx)
    {
        EXPECT_EQ (v[idx], list->pop_front());
        EXPECT_EQ (list->size(), v.size() - idx - 1);
    }
    EXPECT_EQ (list->size(), 0);
}

TEST_F(ListUnittest, back)
{
    ASSERT_TRUE (list != NULL);
    list->push_back(1);
    list->push_back(2);
    EXPECT_EQ (2, list->pop_back());
    EXPECT_EQ (1, list->back());
    EXPECT_EQ (1, list->pop_back());
}

TEST_F(ListUnittest, Unlink)
{
    ASSERT_TRUE (list != NULL);
    ListType::NodeId first = list->push_back(1);
    ListType::NodeId second = list->push_back(2);
    ListType::NodeId thrid = list->push_back(3);

    EXPECT_EQ (2, list->Unlink(second));
    EXPECT_EQ (2, list->size());
    EXPECT_EQ (1, list->pop_front());
    EXPECT_EQ (3, list->pop_back());
}

TEST_F(ListUnittest, erase)
{
    ASSERT_TRUE (list != NULL);
    list->push_back(1);
    ASSERT_EQ (list->size(), 1u);

    auto res = list->begin();
    ASSERT_EQ (*res, 1);
    res = list->erase(res);
    EXPECT_EQ (list->size(), 0u);
    EXPECT_EQ (res, list->end());
}

TEST_F(ListUnittest, multi_erase)
{
    ASSERT_TRUE (list != NULL);
    const int kMaxElement = 1024;
    for (int i = 0; i != kMaxElement; ++i)
    {
        list->push_back(i);
    }

    ASSERT_EQ (list->size(), static_cast<size_t>(kMaxElement));

    auto pos = list->begin();
    const int kForward = 256;
    for (int i = 0; i != kForward; ++i, ++pos);

    ASSERT_EQ (*pos, kForward);

    auto res = list->erase(list->begin(), pos);
    EXPECT_EQ (*res, kForward + 1);
    EXPECT_EQ (list->size(), static_cast<size_t>(kMaxElement - kForward));
}

TEST_F(ListUnittest, iterator)
{
    ASSERT_TRUE (list != NULL);
    list->push_back(1);
    list->push_back(2);
    list->push_back(3);
    list->push_back(4);
    ASSERT_EQ (list->size(), 4u);
    auto iter = list->begin();
    for (int i = 1; i <= list->size() and iter != list->end(); ++i, ++iter)
    {
        ASSERT_EQ(i, *iter);
    }

    iter = list->begin();
    ++iter;
    ++iter;

    auto second = list->begin();
    ++second;

    EXPECT_EQ (*iter, 3);
    auto res = list->erase(second);
    EXPECT_EQ (res, iter);
    EXPECT_EQ (*iter, 3);

    EXPECT_EQ (*list->begin(), 1);
    EXPECT_EQ (*++list->begin(), 3);
    EXPECT_EQ (*++++list->begin(), 4);

    *list->begin() = 2;
    EXPECT_EQ (*list->begin(), 2);
    EXPECT_EQ (list->front(), 2);
}

TEST_F(ListUnittest, const_iterator)
{
    ASSERT_TRUE (list != NULL);
    list->push_back(9);
    list->push_back(5);
    list->push_back(2);
    list->push_back(7);
    ASSERT_EQ (list->size(), 4u);

    const ListType * clist = list.get();

    auto first = clist->begin();
    auto last = clist->end();

    ASSERT_EQ (*clist->begin(), 9);
    ASSERT_EQ (*++clist->begin(), 5);
    ASSERT_EQ (*++++clist->begin(), 2);
    ASSERT_EQ (*++++++clist->begin(), 7);
}

TEST_F(ListUnittest, RestoreFrom)
{
    std::vector<unsigned> v;
    {
        ASSERT_TRUE (list != NULL);
        while (1)
        {
            unsigned val = rand();
            if (ListType::kInvalidNodeId == list->push_back(val)) break;
            else v.push_back(val);
        }
    }

    //old capacity = (65536 - 24 - 48) / (20), padding = 4
    char newbuf[kBufSize * 2 + 4];                  /* larger space */
    ::memcpy(newbuf, buf, sizeof(buf));
    std::unique_ptr<ListType> list = ListType::RestoreFrom(newbuf, sizeof(newbuf));
    ASSERT_TRUE (list != NULL);
    EXPECT_EQ (list->size(), v.size());
    EXPECT_EQ (list->capacity(), (sizeof(newbuf) - ListType::kHeaderSize - fx::base::MemoryList::kHeaderSize) / 20);
    for (size_t idx = 0; idx != v.size(); ++idx)
    {
        EXPECT_EQ (v[idx], list->pop_front());
        EXPECT_EQ (list->size(), v.size() - idx - 1);
    }
}
