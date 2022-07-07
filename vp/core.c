/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "core.h"

/* initialize an empty vector */
static vp_vec *vec_init(vp_vec *vec, size_t init)
{
	if (init == 0) {
		init = VP_VEC_INIT;
	}
	vec->cap = init;
	vec->len = 0;
	return vec;
}

/* only frees array */
static void vec_free(vp_vec *vec)
{
	vfree(vec->data);
}

static void *vec_get(vp_vec *vec, size_t i)
{
	if (i >= vec->len) {
		return NULL;
	}
	return vec->data[i];
}

static size_t vec_insert(vp_vec *vec, void *data)
{
	if (vec->len >= vec->cap) {
		vec->cap *= 2;
		vec->data = vrealloc(vec->data, vec->cap);
	}
	vec->data[vec->len] = data;
	return vec->len++;
}

/* vp_obj */

vp_obj *vp_obj_create(void *initial, size_t size)
{
	vp_obj *obj = vmalloc(sizeof(*obj));
	/* the caller most likely won't keep initial anyway */
	obj->initial = initial;
	obj->size = size;
	return obj;
}

void vp_obj_free(vp_obj *obj)
{
	vfree(obj->initial);
	vfree(obj);
}

/* vp_sig */

vp_sig *vp_sig_create(int n_rets, int n_args, vp_type types[])
{
	vp_sig *sig =
	    vmalloc(sizeof(*sig) + sizeof(vp_type) * (n_rets + n_args));
	sig->n_rets = n_rets;
	sig->n_args = n_args;
	memcpy(&sig->types, types, sizeof(vp_type) * (n_rets + n_args));
	return sig;
}

void vp_sig_free(vp_sig *sig)
{
	vfree(sig);
}

/* vp_func */

vp_func *vp_func_create(vp_sig *sig, vp_code *code)
{
	vp_func *func = vmalloc(sizeof(*func));
	func->sig = sig;
	func->code = code;
	return func;
}

void vp_func_free(vp_func *func)
{
	vp_sig_free(func->sig);
	vp_code_free(func->code);
	vfree(func);
}

/* vp_nest */

vp_nest *vp_nest_create()
{
	vp_nest *nest = vmalloc(sizeof(*nest));
	if (vec_init(&nest->obj, 0) == NULL ||
	    vec_init(&nest->func, 0) == NULL) {
		return NULL;
	}
	return nest;
}

void vp_nest_free(vp_nest *nest)
{
	for (int i = 0; i < nest->func.len; i++) {
		/* vp_func_free(nest->func.data[i]); */
	}
	for (int i = 0; i < nest->obj.len; i++) {
		/* vp_obj_free(nest->obj.data[i]); */
	}
	vec_free(&nest->func);
	vec_free(&nest->obj);
	vfree(nest);
}

vp_obj *vp_obj_get(vp_nest *nest, vp_id id)
{
	return vec_get(&nest->obj, id);
}

vp_func *vp_func_get(vp_nest *nest, vp_id id)
{
	return vec_get(&nest->func, id);
}

vp_id vp_obj_insert(vp_nest *nest, vp_obj *obj)
{
	obj->id = (vp_id)vec_insert(&nest->obj, obj);
	return obj->id;
}

vp_id vp_func_insert(vp_nest *nest, vp_func *func)
{
	func->id = (vp_id)vec_insert(&nest->func, func);
	return func->id;
}
