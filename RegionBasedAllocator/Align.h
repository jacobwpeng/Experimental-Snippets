/*
 * =============================================================================
 *
 *       Filename:  Align.h
 *        Created:  01/12/16 10:07:09
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#pragma once

#include <cstdint>

inline void* align(size_t __align, size_t __size, void*& __ptr,
                   size_t& __space) noexcept {
  const auto __intptr = reinterpret_cast<uintptr_t>(__ptr);
  const auto __aligned = (__intptr - 1u + __align) & -__align;
  const auto __diff = __aligned - __intptr;
  if ((__size + __diff) > __space)
    return nullptr;
  else {
    __space -= __diff;
    return __ptr = reinterpret_cast<void*>(__aligned);
  }
}
