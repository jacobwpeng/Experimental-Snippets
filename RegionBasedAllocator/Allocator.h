/*
 * =============================================================================
 *
 *       Filename:  Allocator.h
 *        Created:  01/11/16 23:19:05
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#pragma once

#include "Page.h"
#include "FreeList.h"
#include "PageAllocator.h"
#include "PageHeap.h"

class Allocator {
 public:
  static std::unique_ptr<Allocator> Create(void* start, uint32_t sz);
  Offset Allocate(uint32_t sz);
  void Deallocate(Offset offset);
  void* OffsetToPtr(Offset offset);

 private:
  static const uint32_t kAllocatorReservedSize = kPageSize;
  static const uint32_t kMinClassSize = 4;
  static const uint32_t kMaxClassSize = kPageSize;
  static const int kSizeClassNum = 11;
  int SizeClass(uint32_t pow_of_two) const;
  int ClassSize(uint32_t cls) const;
  void InitFreeList(FreeList* list, int cls);
  void* start_;
  uint32_t size_;
  std::unique_ptr<PageHeap> heap_;
  std::unique_ptr<PageAllocator> page_allocator_;
  FreeList list_[kSizeClassNum];
};
