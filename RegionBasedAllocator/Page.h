/*
 * =============================================================================
 *
 *       Filename:  Page.h
 *        Created:  01/11/16 22:58:46
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#pragma once

using Offset = int32_t;
using PageID = int32_t;
static const Offset kInvalidOffset = -1;
static const PageID kInvalidPageID = -1;
static const uint32_t kPageSizeShift = 14;
static const uint32_t kPageSize = 1 << kPageSizeShift;
static const uint32_t kPageSizeMask = (kPageSize - 1);
static const Offset kMaxOffset = kPageSize;

enum PageStatus : uint8_t {
  InUse = 1,
  Free = 2,
};

struct Page {
  PageStatus status;
  uint8_t pow_of_two_size;
  uint32_t ref;
  PageID id;
  PageID next;
  PageID prev;
  Offset free;
  Offset free_list;
  Offset Allocate(uintptr_t page_start);
  void Deallocate(uintptr_t page_start, Offset offset);
  Offset base() const;
};
