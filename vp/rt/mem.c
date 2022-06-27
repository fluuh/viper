/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "mem.h"

vp_mem *vp_mem_create(size_t limit)
{
	vp_mem *mem = vmalloc(sizeof(*mem));
	mem->limit = limit;
	return mem;
}

void vp_mem_destroy(vp_mem *mem)
{
	vfree(mem);
}

vp_ptr vp_mem_pmalloc(vp_mem *mem, void *initial, size_t size)
{
	return 0;
}
