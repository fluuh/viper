/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_UTIL_H_
#define VP_UTIL_H_

#include "common.h"
#include <stdlib.h>

void *vu_malloc(size_t size);
void *vu_zalloc(size_t size);
void *vu_malloc_array(size_t n, size_t size);
void *vu_realloc(void *p, size_t size);
int vu_free(void *p);

typedef struct vu_dbuff {
	size_t cap;
	size_t size;
	u8 *buff;
} vu_dbuff;

vu_dbuff *vu_dbuff_create(size_t n, size_t size);
int vu_dbuff_write(vu_dbuff *b, const u8 *src, size_t size);
int vu_dbuff_cwrite(vu_dbuff *b, const char c);
int vu_dbuff_free(vu_dbuff *b);

#endif
