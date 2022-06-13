/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_STATE_H_
#define VP_STATE_H_

#define VI_PAGE_SIZE 1 << 16
#define VI_PAGES_MAX 256

struct vi_state {
	size_t mem_max;
	size_t mem_size;
	size_t mem_cap;
	void **mem_pages;
	size_t num_objs;
	vp_ptr *objs;
};

/*
 * Create a state.
 *
 * `initial`: the number of pages that will be allocated initially.
 * `limit`: the maximum number of pages that can be allocated.
 * `objs`: an array of objects to initialize.
 * `num_objs`: the number of objects
 */
struct vi_state *vi_state_create(size_t initial, size_t limit,
                                 struct vi_obj *objs[], u32 num_objs);

void vi_state_delete(struct vi_state *state);

/* basic allocation functions */
vp_ptr vi_state_malloc(struct vi_state *state, size_t size);
vp_ptr vi_state_free(struct vi_state *state, vp_ptr p);

#endif