/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_FUNC_H_
#define VP_FUNC_H_

#include "common.h"
#include <stdlib.h>

typedef struct vp_label {
	size_t pos;
#ifdef VP_DEBUG
	const char *name;
#endif
} vp_label;

typedef struct vp_func {
	enum {
		vp_func_normal,
		vp_func_native,
	} ftype;
	char *name;
	i32 id;
	vp_native_func native;
	size_t size_code;
	size_t cap_code;
	u8 *code;
	u16 r32;
	u16 r64;
	u16 rxx;
	u16 rresv; // unused
	vp_func_type type;
} vp_func;

// name is optional
vp_func *vp_func_create(const char *name, vp_type ret, vp_type *args,
                        size_t num_args);
vp_func *vp_func_create_native(const char *name, vp_type ret, vp_type *args,
                               size_t num_args, vp_native_func func);
// this also frees the code
int vp_func_free(vp_func *fn);

#endif