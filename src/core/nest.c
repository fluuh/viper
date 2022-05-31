/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "core.h"

struct vi_nest *vi_nest_create(u32 num_funcs, u32 num_objs)
{
	struct vi_nest *nest = vi_malloc(sizeof(*nest));
	nest->num_funcs = num_funcs;
	nest->funcs = vi_malloc(sizeof(*nest->funcs) * num_funcs);
	nest->num_objs = num_objs;
	nest->objs = vi_malloc(sizeof(*nest->objs) * num_objs);
	return nest;
}

void vi_nest_delete(struct vi_nest *nest)
{
	for (int i = 0; i < nest->num_funcs; i++) {
		vi_func_delete(nest->funcs[i]);
	}
	for (int i = 0; i < nest->num_objs; i++) {
		vi_obj_delete(nest->objs[i]);
	}
	vi_nest_free(nest);
}

void vi_nest_free(struct vi_nest *nest)
{
	vi_free(nest->funcs);
	vi_free(nest->objs);
	vi_free(nest);
	return;
}

int vi_obj_insert(struct vi_nest *nest, struct vi_obj *obj, u32 id)
{
	if (nest->num_objs < id) {
		return 1;
	}
	obj->id = id;
	obj->nest = nest;
	nest->objs[id] = obj;
	return 0;
}

int vi_func_insert(struct vi_nest *nest, struct vi_func *func, u32 id)
{
	if (nest->num_funcs < id) {
		return 1;
	}
	func->id = id;
	func->nest = nest;
	nest->funcs[id] = func;
	return 0;
}