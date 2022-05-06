/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include <viper/util.h>
#include <viper/func.h>

vp_func *vp_func_create(vp_type ret, vp_type *args, size_t num_args)
{
	vp_func *fn = vu_malloc(sizeof(*fn));
	// the code is provided by the caller
	fn->ftype = vp_func_normal;
	fn->name = (void*)0;
	fn->cap_code = 0;
	fn->size_code = 0;
	fn->code = (void*)0;
	fn->type.ret = ret;
	fn->type.num_args = num_args;
	memcpy(fn->type.args, args, num_args);
	return fn;
}

vp_func *vp_func_create_native(vp_type ret, vp_type *args, size_t num_args,
                               vp_native_func func)
{
	vp_func *fn = vu_malloc(sizeof(*fn));
	fn->ftype = vp_func_native;
	fn->native = func;
	fn->name = (void*)0;
	fn->type.ret = ret;
	fn->type.num_args = num_args;
	memcpy(fn->type.args, args, num_args);
	return fn;
}

int vp_func_free(vp_func *fn)
{
	vu_free(fn->code);
	vu_free(fn);
	return 0;
}