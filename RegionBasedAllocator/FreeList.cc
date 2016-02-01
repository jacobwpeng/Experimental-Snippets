/*
 * =============================================================================
 *
 *       Filename:  FreeList.cc
 *        Created:  01/11/16 23:06:40
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#include "FreeList.h"
#include "LinkedList.h"
#include "Event.h"

namespace detail {
bool IsPowerOfTwo(uint32_t x) { return x && (x & (x - 1)) == 0; }
}

void FreeList::Init(Page* empty, Page* nonempty, uint32_t pow_of_two_size) {
  empty_ = empty->id;
  nonempty_ = nonempty->id;
  pow_of_two_size_ = pow_of_two_size;
  DLL_init(empty);
  DLL_init(nonempty);
}

Offset FreeList::Allocate(PageAllocator* allocator, PageHeap* heap) {
  auto nonempty = allocator->FindPage(nonempty_);
  if (DLL_empty(nonempty)) {
    auto page = allocator->Allocate();
    page->pow_of_two_size = pow_of_two_size_;
    DLL_prepend(nonempty, allocator, page);
  }
  assert(!DLL_empty(nonempty));
  auto page = DLL_head(nonempty, allocator);
  auto page_start = reinterpret_cast<uintptr_t>(heap->Get(page->id));
  FreeListEvent('N', pow_of_two_size_, page->id);
  return page->Allocate(page_start);
}

void FreeList::Deallocate(PageAllocator* allocator, PageHeap* heap,
                          Offset offset) {
  PageID page_id = offset >> kPageSizeShift;
  auto page = allocator->FindPage(page_id);
  assert(page->id == page_id);
  auto page_start = reinterpret_cast<uintptr_t>(heap->Get(page->id));
  page->Deallocate(page_start, offset);
  FreeListEvent('D', pow_of_two_size_, page->id);
  if (page->ref == 0) {
    allocator->Deallocate(page);
    DLL_remove(allocator, page);
  }
}
