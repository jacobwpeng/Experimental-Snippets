/*
 * =============================================================================
 *
 *       Filename:  PageHeap.cc
 *        Created:  01/11/16 23:05:53
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#include "PageHeap.h"

std::unique_ptr<PageHeap> PageHeap::Create(void* start, uint32_t size) {
  auto start_address = reinterpret_cast<uintptr_t>(start);
  printf("%lx\n", start_address);
  assert((start_address & kPageSizeMask) == 0);
  std::unique_ptr<PageHeap> heap(new PageHeap);
  heap->start_ = start;
  heap->size_ = size;
  heap->max_ = size >> kPageSizeShift;
  return heap;
}

void* PageHeap::Get(PageID page_id) {
  assert(page_id != kInvalidPageID);
  assert(page_id >= 0);
  assert(page_id < max_);
  auto start_address = reinterpret_cast<uintptr_t>(start_);
  auto ptr = start_address + page_id * kPageSize;
  return reinterpret_cast<void*>(ptr);
}

PageID PageHeap::GetPageIDByOffset(Offset offset) {
  auto page_id = offset >> kPageSizeShift;
  assert(page_id < max_);
  return page_id;
}
