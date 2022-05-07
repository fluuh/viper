/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <string.h>

#include <viper/loader.h>
#include <viper/viper.h>

typedef struct export {
	const char *name;
	int type;
	u32 id;
} export;

typedef struct loader {
	u16 opt;
	FILE *file;
	size_t cap_funcs;
	size_t num_funcs;
	vp_func **funcs;
	size_t cap_exports;
	size_t num_exports;
	export *exports;
	size_t cap_objs;
	size_t num_objs;
	vp_obj **objs;
	size_t cap_imports;
	size_t num_imports;
	vn_import **imports;
} loader;

static int read_imports(loader *ld)
{
	if(fgetc(ld->file) != VN_SECTION_IMPORTS) {
		return 1;
	}
	char buff[4];
	fread(buff, 4, 1, ld->file);
	while(fgetc(ld->file) == VN_ELEMENT_START) {
		u8 ret = fgetc(ld->file);
		u8 num_args = fgetc(ld->file);
		vp_type args[16];
		fread(args, 1, num_args, ld->file);
		u32 size;
		fread(&size, 4, 1, ld->file);
		char *name = vu_malloc_array(size, 1);
		fread(name, 1, size, ld->file);
		vn_import *imp = vn_import_create(name, ret, args, num_args);
		u32 id = ld->num_imports;
		ld->num_imports++;
		ld->imports[id] = imp;
	}
	return 0;
}

static int read_code(loader *ld)
{
	if(fgetc(ld->file) != VN_SECTION_CODE) {
		return 1;
	}
	char buff[4];
	fread(buff, 4, 1, ld->file);
	u32 id = 0;
	while(fgetc(ld->file) == VN_ELEMENT_START) {
		vp_func *fn = ld->funcs[id];
		fread(&fn->r32, 2, 1, ld->file);
		fread(&fn->r64, 2, 1, ld->file);
		fread(&fn->rxx, 2, 1, ld->file);
		fread(&fn->rresv, 2, 1, ld->file);
		u32 size;
		fread(&size, 4, 1, ld->file);
		u8 *code = vu_malloc_array(size, 1);
		fread(code, 1, size, ld->file);
		fn->size_code = size;
		fn->cap_code = size;
		fn->code = code;
		id++;
	}
	return 0;
}

static int read_funcs(loader *ld)
{
	if(fgetc(ld->file) != VN_SECTION_FUNCS) {
		return 1;
	}
	char buff[4];
	fread(buff, 4, 1, ld->file);
	while(fgetc(ld->file) == VN_ELEMENT_START) {
		u8 tret = fgetc(ld->file);
		u8 num_args = fgetc(ld->file);
		vp_type args_buff[16];
		fread(args_buff, 1, num_args, ld->file);
		u32 id = ld->num_funcs;
		ld->num_funcs++;
		vp_func *fn = vp_func_create(tret, args_buff, num_args);
		fn->id = id;
		ld->funcs[id] = fn;
	}
	return 0;
}

static int read_objs(loader *ld)
{
	if(fgetc(ld->file) != VN_SECTION_OBJS) {
		return 1;
	}
	char buff[4];
	fread(buff, 4, 1, ld->file);
	while(fgetc(ld->file) == VN_ELEMENT_START) {
		u32 id = ld->num_objs;
		ld->num_objs++;
		u32 size;
		fread(&size, 4, 1, ld->file);
		u8 *init = vu_malloc_array(size, 1);
		fread(init, 1, size, ld->file);
		vp_obj *obj = vp_obj_create(init, size);
		obj->id = id;
		ld->objs[id] = obj;
	}
	return 0;
}

static int read_exports(loader *ld)
{
	if(fgetc(ld->file) != VN_SECTION_EXPORTS) {
		return 1;
	}
	char buff[4];
	fread(buff, 4, 1, ld->file); // skip size
	while(fgetc(ld->file) == VN_ELEMENT_START) {
		int id = ld->num_exports;
		export *ex = &ld->exports[id];
		ld->num_exports++;
		ex->type = fgetc(ld->file);
		fread(&ex->id, 4, 1, ld->file);
		u32 len;
		fread(&len, 4, 1, ld->file);
		char *name = vu_malloc_array(len + 1, 1);
		fread(name, len, 1, ld->file);
		name[len] = '\0';
		ex->name = name;
	}
	return 0;
}

static int read_header(loader *ld)
{
	char buff[5];
	fread(buff, 4, 1, ld->file);
	buff[4] = '\0';
	if(strcmp(buff, VN_MAGIC) != 0) {
		return 1;
	}
	u8 gen = (u8)fgetc(ld->file);
	u8 min = (u8)fgetc(ld->file);
	if(gen != VP_VER_GEN || min != VP_VER_MIN) {
		return 2;
	}
	fread(&ld->opt, 2, 1, ld->file);
	return 0;
}

static int resolve_exports(loader *ld)
{
	for(int i = 0; i < ld->num_exports; i++) {
		export *ex = &ld->exports[i];
		if(ex->type == VN_SECTION_OBJS) {
			ld->objs[ex->id]->name = ex->name;
		} else if(ex->type == VN_SECTION_FUNCS) {
			ld->funcs[ex->id]->name = ex->name;
		} else {
			return 1;
		}
	}
	return 0;
}

int vp_load_file(FILE *file, vn_nest **nest)
{
	loader *ld = vu_malloc(sizeof(*ld));
	ld->file = file;
	ld->cap_exports = VP_BUFF_DEFAULT;
	ld->num_exports = 0;
	ld->exports = vu_malloc_array(VP_BUFF_DEFAULT, sizeof(*ld->exports));
	ld->cap_funcs = VP_BUFF_DEFAULT;
	ld->num_funcs = 0;
	ld->funcs = vu_malloc_array(VP_BUFF_DEFAULT, sizeof(*ld->funcs));
	ld->cap_objs = VP_BUFF_DEFAULT;
	ld->num_objs = 0;
	ld->objs = vu_malloc_array(VP_BUFF_DEFAULT, sizeof(*ld->objs));
	ld->num_imports = 0;
	ld->cap_imports = VP_BUFF_DEFAULT;
	ld->imports = vu_malloc_array(VP_BUFF_DEFAULT, sizeof(*ld->imports));
	if(read_header(ld) != 0) {
		return 1;
	}
	#define SEC_START() \
		if(fgetc(ld->file) != VN_SECTION_START) { \
			return 1; \
		}
	SEC_START();
	if(read_exports(ld) != 0 ||
	   read_objs(ld) != 0 ||
	   read_funcs(ld) != 0 ||
	   read_code(ld) != 0 ||
	   read_imports(ld) != 0 ||
	   resolve_exports(ld) != 0) {
		return 1;
	}
	vn_nest *new = vn_nest_alloc(ld->num_funcs, 
	                             ld->num_imports, ld->num_objs);
	new->num_funcs = ld->num_funcs;
	new->num_imports = ld->num_imports;
	new->num_objs = ld->num_objs;
	for(int i = 0; i < ld->num_funcs; i++) {
		new->funcs[i] = ld->funcs[i];
	}
	for(int i = 0; i < ld->num_objs; i++) {
		new->objs[i] = ld->objs[i];
	}
	for(int i = 0; i < ld->num_imports; i++) {
		new->imports[i] = ld->imports[i];
	}
	*nest = new;
	vu_free(ld->exports);
	vu_free(ld->funcs);
	vu_free(ld->objs);
	vu_free(ld->imports);
	vu_free(ld);
	return 0;
}