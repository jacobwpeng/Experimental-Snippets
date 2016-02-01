/*
 * =============================================================================
 *
 *       Filename:  Event.h
 *        Created:  01/12/16 11:23:18
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#pragma once

#include <cstdio>
#include "Page.h"

inline void FreeListEvent(char op, uint32_t pow_of_two_size, PageID page_id) {
  printf("Event %c, pow_of_two_size: %u, page id: %u\n", op, pow_of_two_size,
         page_id);
}

inline void PageEvent(char op, PageID page_id) {
  printf("Event %c, page id: %u\n", op, page_id);
}
