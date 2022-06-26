/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_CORE_H_
#define VP_CORE_H_

/* includes common.h */
#include "code.h"

/* an identifier for an object or function
 * it does not keep track of which one it is
 */
typedef uint32_t vp_id;

typedef struct vp_obj_def {
	vp_id id;
	size_t size;
	void *initial;
} vp_obj;

/* allocate and initialize an object
 * takes ownership of initial
 */
vp_obj *vp_obj_create(void *initial, size_t size);
void vp_obj_free(vp_obj *obj);

typedef struct vp_sig_def {
	int n_rets;
	int n_args;
	/* first rets, then args */
	vp_type types[];
} vp_sig;

/* allocate and initialize a function signature */
vp_sig *vp_sig_create(int n_rets, int n_args, vp_type types[]);
/* free a signature, should probably just use vfree instead */
void vp_sig_free(vp_sig *sig);

typedef struct vp_func_def {
	vp_id id;
	vp_sig *sig;
	vp_code *code;
} vp_func;

/* allocate and initialize function */
vp_func *vp_func_create(vp_sig *sig, vp_code *code);
void vp_func_free(vp_func *func);

/* bad for type safety, but it works */
typedef struct vp_vec_def {
	size_t cap;
	size_t len;
	void **data;
} vp_vec;

typedef struct vp_nest_def {
	vp_vec func;
	vp_vec obj;
} vp_nest;

/* allocate and initialize a nest */
vp_nest *vp_nest_create(void);

void vp_nest_free(vp_nest *nest);

/* get an object from id */
vp_obj *vp_obj_get(vp_nest *nest, vp_id id);
/* get a function from id */
vp_func *vp_func_get(vp_nest *nest, vp_id id);

/* insert an object into vp_ovec */
vp_id vp_obj_insert(vp_nest *nest, vp_obj *obj);
/* insert a function into vp_fvec */
vp_id vp_func_insert(vp_nest *nest, vp_func *func);

#endif
