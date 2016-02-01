/*
 * =============================================================================
 *
 *       Filename:  main.cc
 *        Created:  01/10/16 21:42:00
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#include <bits/stdc++.h>
#include "Allocator.h"
#include "Align.h"

struct RedEnvelope {
  static const int kMaxPartsNum = 250;
  uint64_t id;
  uint64_t time;
  uint32_t money[kMaxPartsNum];
  uint32_t uins[kMaxPartsNum];
};

int main() {
  std::vector<char> buf;
  buf.resize(100 * (1 << 20));  // 100MiB

  void* start = buf.data();
  size_t size = buf.size();
  align(kPageSize, 99 * (1 << 20), start, size);
  auto allocator = Allocator::Create(start, size);
  if (!allocator) {
    std::cout << "Create allocator failed\n";
    return -1;
  }
  while (1) {
    auto offset = allocator->Allocate(1);
    allocator->Deallocate(offset);
  }
}
