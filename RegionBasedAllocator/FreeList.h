/*
 * =============================================================================
 *
 *       Filename:  FreeList.h
 *        Created:  01/11/16 23:06:17
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#pragma once
#include "Page.h"
#include "PageAllocator.h"
#include "PageHeap.h"

class FreeList {
 public:
  void Init(Page* empty, Page* nonempty, uint32_t pow_of_two_size);
  Offset Allocate(PageAllocator* allocator, PageHeap* heap);
  void Deallocate(PageAllocator* allocator, PageHeap* heap, Offset offset);

 private:
  uint32_t pow_of_two_size_;
  PageID empty_;
  PageID nonempty_;
};
