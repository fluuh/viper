/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_NEST_H_
#define VP_NEST_H_

#include "common.h"
#include "func.h"
#include "obj.h"

// this could be defined in func.h, but since
// it's only used by nest it isn't shared.
typedef struct vn_import {
	i32 id;
	const char *name;
	vp_func_type type;
} vn_import;

typedef struct vn_nest {
	u32 num_funcs;
	u32 cap_funcs;
	vp_func **funcs;
	u32 num_imports;
	u32 cap_imports;
	vn_import **imports;
	u32 num_objs;
	u32 cap_objs;
	vp_obj **objs;
} vn_nest;

vn_nest *vn_nest_alloc(u32 num_funcs, u32 num_imports, u32 num_objs);
vn_import *vn_import_create(const char *name,
                            vp_type ret, vp_type* args, u8 num_args);
int vn_import_free(vn_import *import);
int vn_verify_func(vn_nest *nest, vp_func* f);
int vn_verify(vn_nest *nest);
// this does not free the functions
int vn_nest_free_partial(vn_nest *nest);
int vn_nest_free(vn_nest *nest);


#endif