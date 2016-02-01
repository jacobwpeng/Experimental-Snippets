/*
 * =============================================================================
 *
 *       Filename:  PageAllocator.h
 *        Created:  01/11/16 23:03:45
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#pragma once

#include "Page.h"

class PageAllocator {
 public:
  static std::unique_ptr<PageAllocator> Create(void* start, size_t size);
  Page* Allocate();
  Page* FindPage(PageID page);
  void Deallocate(Page* page);
  size_t capacity() const { return max_; }

 private:
  Page* InitAllocatedPage(Page* page, PageID page_id);
  void* start_;
  size_t size_;
  Offset max_;
  Offset next_;
  Offset free_list_;
};
