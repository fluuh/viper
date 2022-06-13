/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_CORE_NEST_H_
#define VP_CORE_NEST_H_

struct vi_nest {
	u32 num_funcs;
	struct vi_func **funcs;
	u32 num_objs;
	struct vi_obj **objs;
};

struct vi_nest *vi_nest_create(u32 num_funcs, u32 num_objs);
void vi_nest_delete(struct vi_nest *nest);
// free nest, doesn't free funcs and objs
void vi_nest_free(struct vi_nest *nest);
int vi_obj_insert(struct vi_nest *nest, struct vi_obj *obj, u32 id);
int vi_func_insert(struct vi_nest *nest, struct vi_func *func, u32 id);

#endif