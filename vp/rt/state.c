/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "state.h"

vp_state *vp_state_create(vp_nest *nest)
{
	vp_state *s = vmalloc(sizeof(*s));
	s->nest = nest;
	s->mem = vp_mem_create(0);
	s->n_objs = nest->obj.len;
	s->obj = vmalloc(sizeof(*s->obj) * s->n_objs);
	for(int i = 0; i < s->n_objs; i++) {
		vp_obj *obj = nest->obj.data[i];
		s->obj[i] = vp_mem_pmalloc(s->mem, obj->initial, obj->size);
	}
	return s;
}

void vp_state_free(vp_state *s)
{
	vp_mem_destroy(s->mem);
	vfree(s->obj);
	vfree(s);
}
