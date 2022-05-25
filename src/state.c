/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include <viper/state.h>
#include <viper/util.h>

static void ve_free_block(ve_allocator *alloc, ve_mblock *block)
{
	ve_mblock *next = block->next;
	ve_mblock *prev = block->prev;

	// Fix references around it
	if (block->next != (void *)0) {
		block->next->prev = prev;
	}
	if (block->prev != (void *)0) {
		block->prev->next = next;
	} else {
		// next is now first
		alloc->blocks = next;
	}

	// Free the block
	vu_free(block->block);
	vu_free(block);
}

ve_allocator *ve_allocator_init(size_t max)
{
	if (max == 0) {
		max = -1;
	}
	ve_allocator *alloc = vu_malloc(sizeof(*alloc));
	alloc->cap_blocks = VP_BUFF_DEFAULT;
	alloc->num_blocks = 0;
	alloc->blocks = (void *)0;
	alloc->allocated = 0;
	alloc->max = max;
	return alloc;
}

void ve_allocator_free(ve_allocator *alloc)
{
	// Free unfreed blocks
	while (alloc->blocks != (void *)0) {
		ve_free_block(alloc, alloc->blocks);
	}
	// Free the allocator
	vu_free(alloc);
}

void *ve_malloc(ve_allocator *alloc, size_t size)
{
	if (alloc->allocated + size >= alloc->max) {
		// out of space
		return (void *)0;
	}
	alloc->allocated += size;
	ve_mblock *block = vu_malloc(sizeof(*block));
	block->size = size;
	block->prev = (void *)0;
	block->next = alloc->blocks;
	if (alloc->blocks != (void *)0) {
		alloc->blocks->prev = block;
	}
	alloc->blocks = block;
	// Allocate the memory
	block->block = vu_malloc(size);
	return block->block;
}

void ve_free(ve_allocator *alloc, void *mem)
{
	int found = 0;
	ve_mblock *block = alloc->blocks;
	while (block != (void *)0) {
		if (block->block == mem) {
			found = 1;
			break;
		}
		block = block->next;
	}
	if (!found) {
		// invalid free
		return;
	}
	alloc->allocated -= block->size;
	ve_free_block(alloc, block);
}

vp_state *vp_state_init(vn_nest *nest)
{
	vp_state *state = vu_malloc(sizeof(*state));
	state->alloc = ve_allocator_init(0);
	state->nest = nest;
	state->num_objs = nest->num_objs;
	state->objs = vu_malloc_array(nest->num_objs, sizeof(*state->objs));
	for (int i = 0; i < state->num_objs; i++) {
		size_t size = nest->objs[i]->size;
		state->objs[i] = ve_malloc(state->alloc, size);
		memcpy(state->objs[i], nest->objs[i]->init, size);
	}
	state->frames = vu_malloc_array(VP_BUFF_DEFAULT, 8);
	state->fp = state->frames;
	state->fp->size = 0;
	return state;
}

void vp_state_free(vp_state *state)
{
	ve_allocator_free(state->alloc);
	vu_free(state->objs);
	vu_free(state->frames);
	vu_free(state);
}