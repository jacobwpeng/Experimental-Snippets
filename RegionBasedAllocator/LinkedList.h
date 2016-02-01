/*
 * =============================================================================
 *
 *       Filename:  LinkedList.h
 *        Created:  01/11/16 23:01:19
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#pragma once
#include "Page.h"

class PageAllocator;

struct InplaceLinkNode {
  Offset offset;
};

inline void SLL_init(void* l) {
  auto list = reinterpret_cast<InplaceLinkNode*>(l);
  list->offset = kInvalidOffset;
}

inline bool SLL_empty(void* l) {
  auto list = reinterpret_cast<InplaceLinkNode*>(l);
  return list->offset == kInvalidOffset;
}

inline void SLL_push(void* l, uintptr_t start, Offset offset) {
  auto list = reinterpret_cast<InplaceLinkNode*>(l);
  auto node = reinterpret_cast<InplaceLinkNode*>(start + offset);
  node->offset = list->offset;
  list->offset = offset;
}

inline Offset SLL_pop(void* l, uintptr_t start) {
  auto list = reinterpret_cast<InplaceLinkNode*>(l);
  assert(!SLL_empty(list));
  auto offset = list->offset;
  auto node = reinterpret_cast<InplaceLinkNode*>(start + offset);
  list->offset = node->offset;
  return offset;
}

void DLL_init(Page* page);
bool DLL_empty(Page* page);
Page* DLL_head(Page* list, PageAllocator* allocator);
void DLL_prepend(Page* list, PageAllocator* allocator, Page* page);
void DLL_remove(PageAllocator* allocator, Page* page);
