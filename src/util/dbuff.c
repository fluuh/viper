/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include <viper/util.h>

vu_dbuff *vu_dbuff_create(size_t n, size_t size)
{
	size *= n;
	vu_dbuff *b = vu_malloc(sizeof(*b));
	b->size = 0;
	if(size == 0) {
		b->cap = 0;
		b->buff = (void*)0;
		return b;
	}
	b->cap = size;
	b->buff = vu_malloc_array(n, size);
	return b;
}

int vu_dbuff_write(vu_dbuff *b, const u8 *src, size_t size)
{
	// size check
	if(b->cap <= b->size + size) {
		// TODO: remove this check
		if(b->cap == 0) {
			b->cap = VP_BUFF_DEFAULT;
		} else {
			b->cap *= 2;
		}
		b->buff = vu_realloc(b->buff, b->cap);
		// run the check again
		return vu_dbuff_write(b, src, size);
	}
	memcpy(&b->buff[b->size], src, size);
	b->size += size;
	return 0;
}

int vu_dbuff_cwrite(vu_dbuff *b, const char c)
{
	if(b->cap <= b->size + 1) {

		if(b->cap == 0) {
			b->cap = VP_BUFF_DEFAULT;
		} else {
			b->cap *= 2;
		}
		b->buff = vu_realloc(b->buff, b->cap);
	}
	b->buff[b->size] = c;
	b->size++;
	return 0;
}

int vu_dbuff_free(vu_dbuff *b)
{
	vu_free(b->buff);
	return vu_free(b);
}
