/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include <viper/util.h>
#include <viper/emit.h>

vn_builder *vn_build_create(void)
{
	vn_builder *builder = vu_malloc(sizeof(*builder));
	builder->num_funcs = 0;
	builder->cap_funcs = VP_BUFF_DEFAULT;
	builder->funcs = vu_malloc_array(VP_BUFF_DEFAULT, 
	                                 sizeof(*builder->funcs));
	builder->num_imports = 0;
	builder->cap_imports = VP_BUFF_DEFAULT;
	builder->imports = vu_malloc_array(VP_BUFF_DEFAULT, 
	                             sizeof(*builder->imports));
	builder->num_objs = 0;
	builder->cap_objs = VP_BUFF_DEFAULT;
	builder->objs = vu_malloc_array(VP_BUFF_DEFAULT, 
	                             sizeof(*builder->objs));
	return builder;
}


vn_import *vn_bimport(vn_builder *builder, const char *name,
                      vp_type ret, vp_type *args, u8 num_args)
{
	vn_import *import = vn_import_create(name, ret, args, num_args);
	import->id = builder->num_imports;
	builder->imports[builder->num_imports] = import;
	builder->num_imports++;
	return import;
}

vp_obj *vn_bobject(vn_builder *builder, size_t size, u8 *init)
{
	u32 i = builder->num_objs;
	builder->num_objs++;
	vp_obj *obj = vp_obj_create(init, size);
	obj->id = i;
	builder->objs[i] = obj;
	return obj;
}

int vn_build_free(vn_builder *builder)
{
	for(int i = 0; i < builder->num_funcs; i++) {
		vn_bfunc_free(builder->funcs[i]);
	}
	vu_free(builder->funcs);
	vu_free(builder);
	return 0;
}

vn_bfunc *vn_bfunc_create(vn_builder *builder, vp_type ret,
                        vp_type *args, size_t num_args)
{
	if(builder->cap_funcs <= builder->num_funcs + 1) {
		builder->cap_funcs *= 2;
		builder->funcs = vu_realloc(builder->funcs, 
		                            builder->cap_funcs);
	}
	vn_bfunc *fn = vu_malloc(sizeof(*fn) +
	                         sizeof(vp_type) * num_args);
	fn->ftype = vn_bfunc_normal;
	fn->name = (void*)0;
	fn->cap_code = VP_BUFF_DEFAULT;
	fn->size_code = 0;
	fn->code = vu_malloc_array(VP_BUFF_DEFAULT, sizeof(*fn->code));
	fn->cap_labels = VP_BUFF_DEFAULT;
	fn->num_labels = 0;
	fn->labels = vu_malloc_array(VP_BUFF_DEFAULT, sizeof(*fn->labels));
	fn->rw = 0;
	fn->rl = 0;
	fn->rx = 0;
	fn->cap_regs = VP_BUFF_DEFAULT;
	fn->num_regs = 0;
	fn->regs = vu_malloc_array(VP_BUFF_DEFAULT, sizeof(*fn->regs));
	fn->type.ret = ret;
	fn->type.num_args = num_args;
	memcpy(fn->type.args, args, num_args);
	builder->funcs[builder->num_funcs] = fn;
	fn->id = builder->num_funcs;
	builder->num_funcs++;

	fn->args = vu_malloc_array(fn->type.num_args, sizeof(*fn->args));
	for(int i = 0; i < fn->type.num_args; i++) {
		fn->args[i] = vn_reg_create(fn, fn->type.args[i]);
	}
	return fn;
}

vn_bfunc *vn_bfunc_create_native(vn_builder *builder, vp_type ret, 
                        vp_type *args, size_t num_args, vp_native_func func)
{
	vn_bfunc *fn = vu_malloc(sizeof(*fn) + sizeof(vp_type) * num_args);
	/* unused fields */
	fn->regs = (void*)0;
	fn->labels = (void*)0;
	fn->args = (void*)0;

	fn->name = (void*)0;
	fn->ftype = vn_bfunc_native;
	fn->native = func;
	fn->type.ret = ret;
	fn->type.num_args = num_args;
	memcpy(fn->type.args, args, num_args);
	builder->funcs[builder->num_funcs] = fn;
	fn->id = builder->num_funcs;
	builder->num_funcs++;
	return fn;
}

int vn_bfunc_free(vn_bfunc *fn)
{
	if(fn->regs != (void*)0) {
		vu_free(fn->regs);
	}
	// these are going to be generated
	// by vn_verify later on, so we don't
	// need to keep it
	if(fn->labels != (void*)0) {
		vu_free(fn->labels);
	}
	if(fn->args != (void*)0) {
		vu_free(fn->args);
	}
	vu_free(fn);
	return 0;
}

vn_nest *vn_build(vn_builder *builder)
{
	vn_nest *nest = vn_nest_alloc(builder->num_funcs, 
	                              builder->num_imports,
				      builder->num_objs);
	for(int i = 0; i < builder->num_imports; i++) {
		nest->imports[i] = builder->imports[i];
	}
	nest->num_imports = builder->num_imports;
	vu_free(builder->imports);
	for(int i = 0; i < builder->num_objs; i++) {
		nest->objs[i] = builder->objs[i];
	}
	nest->num_objs = builder->num_objs;
	vu_free(builder->objs);
	for(int i = 0; i < builder->num_funcs; i++) {
		vn_bfunc *bf = builder->funcs[i];
		if(bf->ftype == vn_bfunc_native) {
			vp_func *fn = vp_func_create_native(bf->type.ret,
			                                    bf->type.args,
							    bf->type.num_args,
							    bf->native);
			fn->name = bf->name;
			nest->funcs[nest->num_funcs] = fn;
			nest->num_funcs++;
		} else {
			vp_func *fn = vp_func_create(bf->type.ret, 
						bf->type.args, 
						bf->type.num_args);
			fn->cap_code = bf->cap_code;
			fn->size_code = bf->size_code;
			fn->code = bf->code;
			fn->name = bf->name;
			nest->funcs[nest->num_funcs] = fn;
			nest->num_funcs++;
		}
	}
	vn_build_free(builder);
	return nest;
}