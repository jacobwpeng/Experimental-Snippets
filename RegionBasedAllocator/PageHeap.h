/*
 * =============================================================================
 *
 *       Filename:  PageHeap.h
 *        Created:  01/11/16 23:05:26
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#pragma once
#include "Page.h"

class PageHeap {
 public:
  static std::unique_ptr<PageHeap> Create(void* start, uint32_t size);
  void* Get(PageID page_id);
  PageID GetPageIDByOffset(Offset offset);
  uint32_t capacity() const { return max_; }

 private:
  void* start_;
  uint32_t size_;
  PageID max_;
};
