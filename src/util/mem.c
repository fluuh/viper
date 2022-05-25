/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdlib.h>

#include <viper/util.h>

void *vu_malloc(size_t size)
{
	if (size == 0) {
		return (void *)0;
	}
	return malloc(size);
}

void *vu_zalloc(size_t size)
{
	void *m = vu_malloc(size);
	// TODO: zeroing
	return m;
}

void *vu_malloc_array(size_t size, size_t num) { return vu_malloc(size * num); }

void *vu_realloc(void *p, size_t size)
{
	if (size == 0) {
		free(p);
		return (void *)0;
	}
	return realloc(p, size);
}

int vu_free(void *p)
{
	free(p);
	return 0;
}