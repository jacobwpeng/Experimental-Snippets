/*
 * =============================================================================
 *
 *       Filename:  LinkedList.cc
 *        Created:  01/11/16 23:03:00
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#include "LinkedList.h"
#include "PageAllocator.h"

void DLL_init(Page* page) {
  page->prev = page->id;
  page->next = page->id;
}

bool DLL_empty(Page* list) { return list->next == list->id; }

Page* DLL_head(Page* list, PageAllocator* allocator) {
  assert(!DLL_empty(list));
  return allocator->FindPage(list->next);
}

void DLL_prepend(Page* list, PageAllocator* allocator, Page* page) {
  page->prev = list->id;
  page->next = list->next;
  list->next = page->id;
  if (page->next != kInvalidPageID) {
    auto old_next = allocator->FindPage(page->next);
    old_next->prev = page->id;
  }
}

void DLL_remove(PageAllocator* allocator, Page* page) {
  assert(page->next != kInvalidPageID);
  auto prev = allocator->FindPage(page->prev);
  prev->next = page->next;
  auto next = allocator->FindPage(page->next);
  next->prev = page->prev;
}
