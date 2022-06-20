/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VIPER_CORE_H_
#define VIPER_CORE_H_

typedef struct vi_nest *vp_nest;

typedef struct vp_funcType {
	vp_type ret;
	int num_args;
	vp_type args[];
} vp_funcType;

typedef u32 vp_func;
int vp_func_create(vp_nest nest, vp_funcType *type, vp_func *ret);
int vp_func_delete(vp_nest nest, vp_func func);

typedef u32 vp_obj;
/*
 * Create a new object.
 * `initial` can be NULL.
 */
int vp_obj_create(vp_nest nest, void *initial, size_t size);
int vp_obj_delete(vp_nest nest, vp_obj obj);

size_t vp_obj_size(vp_nest nest, vp_obj obj);
void *vp_obj_initial(vp_nest nest, vp_obj obj);

typedef struct {
	enum {
		vp_export_func,
		vp_export_obj,
	} ty;
	union {
		vp_func fn;
		vp_func obj;
	};
} vp_export;

int vp_get(vp_nest nest, const char *name, vp_export *x);

#endif
