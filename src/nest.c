/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include <viper/nest.h>
#include <viper/util.h>

vn_nest *vn_nest_alloc(u32 num_funcs, u32 num_imports, u32 num_objs,
                       u32 num_exports)
{
	vn_nest *nest = vu_malloc(sizeof(*nest));
	nest->cap_funcs = num_funcs;
	nest->num_funcs = 0; // this has to be set by the caller
	nest->funcs = vu_malloc_array(num_funcs, sizeof(*nest->funcs));
	nest->cap_imports = num_imports;
	nest->num_imports = 0; // same here
	nest->imports = vu_malloc_array(num_imports, sizeof(*nest->imports));
	nest->cap_objs = num_objs;
	nest->num_objs = 0; // again!
	nest->objs = vu_malloc_array(num_objs, sizeof(*nest->objs));
	nest->cap_exports = num_exports;
	nest->num_exports = 0;
	nest->exports = vu_malloc_array(num_exports, sizeof(*nest->exports));
	return nest;
}

vn_import *vn_import_create(const char *name, vp_type ret, vp_type *args,
                            u8 num_args)
{
	vn_import *import =
	    vu_malloc(sizeof(*import) + sizeof(vp_type) * num_args);
	import->name = name;
	import->id = 0; // this has to be set by the caller
	import->type.ret = ret;
	import->type.num_args = num_args;
	memcpy(import->type.args, args, num_args);
	return import;
}

int vn_import_free(vn_import *import)
{
	vu_free(import);
	return 0;
}

int vn_verify_func(vn_nest *nest, vp_func *f)
{
	// TODO: verify functions
	return 0;
}

int vn_verify(vn_nest *nest)
{
	for (int i = 0; i < nest->num_funcs; i++) {
		vn_verify_func(nest, nest->funcs[i]);
	}
	return 0;
}

int vn_nest_free_partial(vn_nest *nest)
{
	for (int i = 0; i < nest->num_imports; i++) {
		vn_import_free(nest->imports[i]);
	}
	vu_free(nest->funcs);
	vu_free(nest->imports);
	vu_free(nest->objs);
	vu_free(nest);
	return 0;
}

int vn_nest_free(vn_nest *nest)
{
	for (int i = 0; i < nest->num_funcs; i++) {
		vp_func_free(nest->funcs[i]);
	}
	for (int i = 0; i < nest->num_objs; i++) {
		// this should call a vp_obj_free function
		vu_free(nest->objs[i]);
	}
	return vn_nest_free_partial(nest);
}

vp_export vn_get_export(vn_nest *nest, const char *name)
{
	for (int i = 0; i < nest->num_funcs; i++) {
		const char *en = nest->funcs[i]->name;
		if (en == (void *)0) {
			continue;
		}
		if (strcmp(en, name) == 0) {
			return i;
		}
	}
	// this should probably be better than this
	return -1;
}