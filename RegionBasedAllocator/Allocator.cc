/*
 * =============================================================================
 *
 *       Filename:  Allocator.cc
 *        Created:  01/11/16 23:21:59
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#include <iostream>
#include "Allocator.h"

namespace detail {
uint32_t RoundUpToPowerOfTwo(uint32_t sz) {
  uint32_t x = 1;
  while (x < sz) {
    x *= 2;
  }
  return x;
}

uint32_t RoundUpToPageSize(uint32_t sz) {
  uint32_t x = kPageSize;
  while (x < sz) {
    x += kPageSize;
  }
  return x;
}
}

std::unique_ptr<Allocator> Allocator::Create(void* start, uint32_t sz) {
  auto s = reinterpret_cast<uintptr_t>(start);
  if (s & kPageSizeMask) {  // PageSize aligned
    std::cout << "start is not PageSize aligned\n";
    return nullptr;
  }
  if (sz < kAllocatorReservedSize) {
    std::cout << "region size is less than kAllocatorReservedSize\n";
    return nullptr;
  }
  auto left = sz - kAllocatorReservedSize;
  s += kAllocatorReservedSize;
  // 1 : 127
  auto allocator_region_size = detail::RoundUpToPageSize(left >> 7);
  auto page_allocator =
      PageAllocator::Create(reinterpret_cast<void*>(s), allocator_region_size);
  if (page_allocator == nullptr) {
    std::cout << "Create page allocator failed\n";
    return nullptr;
  }
  s += allocator_region_size;
  auto heap_region_size = left - allocator_region_size;
  auto heap = PageHeap::Create(reinterpret_cast<void*>(s), heap_region_size);
  if (heap == nullptr) {
    std::cout << "Create page heap failed\n";
    return nullptr;
  }
  std::unique_ptr<Allocator> allocator(new Allocator);
  allocator->start_ = start;
  allocator->size_ = sz;
  allocator->heap_ = std::move(heap);
  allocator->page_allocator_ = std::move(page_allocator);
  for (int i = 0; i < kSizeClassNum; ++i) {
    allocator->InitFreeList(&allocator->list_[i], i);
  }
  return allocator;
}

Offset Allocator::Allocate(uint32_t sz) {
  if (sz > kPageSize) return kInvalidOffset;
  // std::cout << "Required size: " << sz << '\n';
  auto rounded = detail::RoundUpToPowerOfTwo(sz);
  // std::cout << "Required size after round: " << rounded << '\n';
  auto cls = SizeClass(rounded);
  // std::cout << "Required size class: " << cls << '\n';
  return list_[cls].Allocate(page_allocator_.get(), heap_.get());
}

void Allocator::Deallocate(Offset offset) {
  auto page_id = heap_->GetPageIDByOffset(offset);
  std::cout << "Deallocate offset: " << offset << ", page id: " << page_id
            << '\n';
  auto page = page_allocator_->FindPage(page_id);
  assert(page);
  assert(page->id == page_id);
  assert(page->pow_of_two_size >= 2);
  auto cls = page->pow_of_two_size - 2;
  std::cout << "Deallocate cls: " << cls << '\n';
  assert(cls < kSizeClassNum);
  list_[cls].Deallocate(page_allocator_.get(), heap_.get(), offset);
}

void* Allocator::OffsetToPtr(Offset offset) {
  // auto page_id = heap_->GetPageIDByOffset(offset);
  // auto start = heap_->Get(page_id);
  // return reinterpret_cast<uintptr_t>(start) + offset
}

int Allocator::SizeClass(uint32_t pow_of_two) const {
  assert(pow_of_two <= kPageSize);
  if (pow_of_two <= kMinClassSize) {
    return 0;
  } else {
    pow_of_two /= kMinClassSize;
    int cls = -1;
    while (pow_of_two) {
      ++cls;
      pow_of_two >>= 1;
    }
    assert(cls < kSizeClassNum);
    return cls;
  }
}

int Allocator::ClassSize(uint32_t cls) const {
  assert(cls < kSizeClassNum);
  int sz = 4;
  while (cls) {
    sz *= 2;
    --cls;
  }
  return sz;
}

void Allocator::InitFreeList(FreeList* list, int cls) {
  auto empty = page_allocator_->Allocate();
  auto nonempty = page_allocator_->Allocate();
  list->Init(empty, nonempty, cls + 2);
}
