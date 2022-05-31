/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

<<<<<<< HEAD
#include <viper/core.h>
#include <viper/util.h>

vp_func *vp_func_create(vp_type ret, vp_type *args,
                        size_t num_args)
{
	vp_func *fn = vu_malloc(sizeof(*fn));
	// the code is provided by the caller
	fn->ftype = vp_func_normal;
	fn->cap_code = 0;
	fn->size_code = 0;
	fn->code = (void *)0;
	fn->type.ret = ret;
	fn->type.num_args = num_args;
	memcpy(fn->type.args, args, num_args);
	return fn;
}

vp_func *vp_func_create_native(vp_type ret, vp_type *args,
                               size_t num_args, vp_native_func func)
{
	vp_func *fn = vu_malloc(sizeof(*fn));
	/* unused fields */
	fn->code = (void *)0;
	fn->ftype = vp_func_native;
	fn->native = func;
	fn->type.ret = ret;
	fn->type.num_args = num_args;
	memcpy(fn->type.args, args, num_args);
	return fn;
}

int vp_func_free(vp_func *fn)
{
	if (fn->code != (void *)0) {
		vu_free(fn->code);
	}
	if (fn->name != (void *)0) {
		vu_free(fn->name);
	}
	vu_free(fn);
	return 0;
=======
#include "core.h"

struct vi_code *vi_code_create(int nreg, size_t size, u8 *code)
{
	struct vi_code *c = vi_malloc(sizeof(*c) + size);
	c->nreg = nreg;
	c->size = size;
	if (code != NULL) {
		memcpy(c->code, code, size);
	}
	return c;
}

struct vi_func *vi_func_create(vp_funcType *type, struct vi_code *code)
{
	struct vi_func *fn = vi_malloc(sizeof(*fn));
	fn->ty = type;
	fn->id = 0;
	fn->nest = NULL;
	fn->code = code;
	return fn;
}

void vi_func_delete(struct vi_func *fn)
{
	vi_free(fn->code);
	vi_func_free(fn);
}

void vi_func_free(struct vi_func *fn)
{
	vi_free(fn);
>>>>>>> bb5c964 (fuck it, full rewrite)
}