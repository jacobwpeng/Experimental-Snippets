/*
 * =============================================================================
 *
 *       Filename:  Page.cc
 *        Created:  01/11/16 22:59:52
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#include "Page.h"
#include "LinkedList.h"

Offset Page::Allocate(uintptr_t page_start) {
  if (free_list == kInvalidOffset) {
    if (free == kPageSize) {
      throw std::bad_alloc();
    }
    auto offset = free;
    free += 1 << pow_of_two_size;
    ++ref;
    return base() + free;
  } else {
    ++ref;
    return base() + SLL_pop(&free_list, page_start);
  }
}

void Page::Deallocate(uintptr_t page_start, Offset offset) {
  assert(status == PageStatus::InUse);
  assert(ref > 0);
  --ref;
  SLL_push(&free_list, page_start, offset);
}

Offset Page::base() const { return kPageSize * id; }
