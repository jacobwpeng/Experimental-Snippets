/*
 * =============================================================================
 *
 *       Filename:  PageAllocator.cc
 *        Created:  01/11/16 23:04:04
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#include "PageAllocator.h"
#include "Align.h"
#include "LinkedList.h"
#include "Event.h"

std::unique_ptr<PageAllocator> PageAllocator::Create(void* start, size_t size) {
  auto s = reinterpret_cast<uintptr_t>(start);
  assert((s & kPageSizeMask) == 0);  // PageSize aligned
  align(alignof(Page), sizeof(Page), start, size);
  std::unique_ptr<PageAllocator> allocator(new PageAllocator);
  allocator->start_ = start;
  allocator->size_ = size;
  allocator->max_ = size / sizeof(Page);
  allocator->free_list_ = kInvalidOffset;
  allocator->next_ = 0;
  return allocator;
}

Page* PageAllocator::Allocate() {
  PageID page_id = kInvalidPageID;
  auto start = reinterpret_cast<uintptr_t>(start_);
  if (free_list_ == kInvalidOffset) {
    if (next_ == max_) {
      throw std::bad_alloc();
    }
    page_id = next_++;
  } else {
    page_id = SLL_pop(&free_list_, start);
  }
  auto ptr = start + page_id * sizeof(Page);
  auto page = reinterpret_cast<Page*>(ptr);
  PageEvent('N', page_id);
  return InitAllocatedPage(page, page_id);
}

Page* PageAllocator::FindPage(PageID page_id) {
  assert(page_id < next_);
  assert(page_id != kInvalidPageID);
  assert(page_id >= 0);
  auto start = reinterpret_cast<uintptr_t>(start_);
  auto ptr = start + page_id * sizeof(Page);
  return reinterpret_cast<Page*>(ptr);
}

void PageAllocator::Deallocate(Page* page) {
  assert(page);
  assert(page->id < next_);
  assert(page->id != kInvalidPageID);
  assert(page->id >= 0);
  auto start = reinterpret_cast<uintptr_t>(start_);
  auto page_address = reinterpret_cast<uintptr_t>(page);
  assert(page_address >= start);
  assert(page_address < start + size_);
  auto expected_address = start + page->id * sizeof(Page);
  assert(expected_address == page_address);
  assert(page->ref == 0);
  assert(page->status == PageStatus::InUse);
  page->status = PageStatus::Free;
  SLL_push(&free_list_, start, page->id);
  PageEvent('D', page->id);
}

Page* PageAllocator::InitAllocatedPage(Page* page, PageID page_id) {
  page->status = PageStatus::InUse;
  page->ref = 0;
  page->id = page_id;
  page->next = kInvalidPageID;
  page->prev = kInvalidPageID;
  page->free = 0;
  page->free_list = kInvalidOffset;
  return page;
}
