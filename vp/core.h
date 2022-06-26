/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_CORE_H_
#define VP_CORE_H_

/* includes common.h */
#include "code.h"

typedef struct vp_obj_def {
	
} vp_obj;

typedef struct vp_func {
	vp_func_type ftype;
	vp_code code;
} vp_func;

typedef struct vp_fvec_def {
	size_t cap;
	size_t len;
	vp_func **func;
} vp_fvec;

typedef struct vp_ovec_def {
	size_t cap;
	size_t len;
	vp_obj **obj;
} vp_ovec;

typedef struct vp_nest_def {
	vp_fvec funcs;
	vp_ovec objs;
} vp_nest;

#endif
