/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include <viper/core.h>
#include <viper/util.h>

vp_func *vp_func_create(const char *name, vp_type ret, vp_type *args,
                        size_t num_args)
{
	vp_func *fn = vu_malloc(sizeof(*fn));
	if (name != NULL) {
		fn->name = vu_malloc(strlen(name) + 1);
		memcpy(fn->name, name, strlen(name) + 1);
	} else {
		fn->name = NULL;
	}
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

vp_func *vp_func_create_native(const char *name, vp_type ret, vp_type *args,
                               size_t num_args, vp_native_func func)
{
	vp_func *fn = vu_malloc(sizeof(*fn));
	if (name != NULL) {
		fn->name = vu_malloc(strlen(name) + 1);
		memcpy(fn->name, name, strlen(name) + 1);
	} else {
		fn->name = NULL;
	}
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
}