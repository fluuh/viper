/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_STATE_H_
#define VP_STATE_H_

#include "nest.h"

typedef struct ve_mblock ve_mblock;

struct ve_mblock {
	ve_mblock *next;
	ve_mblock *prev;
	void *block;
	size_t size;
};

typedef struct ve_allocator {
	size_t allocated;
	size_t max;
	size_t num_blocks;
	size_t cap_blocks;
	ve_mblock *blocks;
} ve_allocator;

ve_allocator *ve_allocator_init(size_t max);
void ve_allocator_free(ve_allocator *alloc);
void *ve_malloc(ve_allocator *alloc, size_t size);
void ve_free(ve_allocator *alloc, void *mem);

// typedef is in common.h
struct vp_state {
	vn_nest *nest;
	ve_allocator *alloc;
	size_t num_objs;
	void **objs;
};

vp_state *vp_state_init(vn_nest *nest);
void vp_state_free(vp_state *state);

#endif