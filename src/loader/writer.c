/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include "loader.h"

static int read_funcs(vn_writer *writer)
{
	vu_dbuff *bf = writer->funcs;
	vu_dbuff *bc = writer->code;
	vu_dbuff *be = writer->exports;
	for (int i = 0; i < writer->nest->num_funcs; i++) {
		vp_func *fn = writer->nest->funcs[i];
		/* export */
		const char *name = fn->name;
		if (name != (void *)0) {
			// is exported
			u32 len = (u32)strlen(name);
			u32 fid = i;
			vu_dbuff_cwrite(be, VN_ELEMENT_START);
			// export type
			// this is the section type for consistency
			vu_dbuff_cwrite(be, VN_SECTION_FUNCS);

			vu_dbuff_write(be, (u8 *)&fid, 4); // function id
			vu_dbuff_write(be, (u8 *)&len, 4); // name length
			// strings are not null-terminated
			vu_dbuff_write(be, (const u8 *)name, strlen(name));
		}
		/* function type */
		vu_dbuff_cwrite(bf, VN_ELEMENT_START);
		vu_dbuff_cwrite(bf, fn->type.ret);
		vu_dbuff_write(bf, (u8 *)&fn->type.num_args, 1);
		vu_dbuff_write(bf, (u8 *)fn->type.args, fn->type.num_args);
		/* code */
		vu_dbuff_cwrite(bc, VN_ELEMENT_START);
		vu_dbuff_write(bc, (u8 *)&fn->r32, 2);
		vu_dbuff_write(bc, (u8 *)&fn->r64, 2);
		vu_dbuff_write(bc, (u8 *)&fn->rxx, 2);
		// if rresv is ever used, this has to be modified
		u16 resv = 0;
		vu_dbuff_write(bc, (u8 *)&resv, 2);
		u32 csize = (u32)fn->size_code;
		vu_dbuff_write(bc, (u8 *)&csize, 4);
		vu_dbuff_write(bc, fn->code, fn->size_code);
	}
	return 0;
}

static int read_imports(vn_writer *writer)
{
	vn_nest *nest = writer->nest;
	vu_dbuff *b = writer->imports;
	for (int i = 0; i < nest->num_imports; i++) {
		vn_import *im = nest->imports[i];
		const char *name = im->name;
		if (name == (void *)0) {
			return 1;
		}
		vu_dbuff_cwrite(b, VN_ELEMENT_START);
		vu_dbuff_cwrite(b, im->type.ret);
		vu_dbuff_cwrite(b, im->type.num_args);
		vu_dbuff_write(b, (u8 *)im->type.args, im->type.num_args);
		u32 len = (u32)strlen(name);
		vu_dbuff_write(b, (u8 *)&len, 4);
		vu_dbuff_write(b, (const u8 *)name, (size_t)len);
	}
	return 0;
}

static int read_objs(vn_writer *writer)
{
	vn_nest *nest = writer->nest;
	vu_dbuff *b = writer->objs;
	for (int i = 0; i < nest->num_funcs; i++) {
		vp_obj *obj = nest->objs[i];
		const char *name = obj->name;
		if (name != (void *)0) {
			// is exported
			u32 len = (u32)strlen(name);
			u32 id = i;
			vu_dbuff_cwrite(b, VN_ELEMENT_START);
			// export type
			// this is the section type for consistency
			vu_dbuff_cwrite(b, VN_SECTION_OBJS);

			vu_dbuff_write(b, (u8 *)&id, 4);
			vu_dbuff_write(b, (u8 *)&len, 4);
			// strings are not null-terminated
			vu_dbuff_write(b, (const u8 *)name, len);
		}
		vu_dbuff_cwrite(b, VN_ELEMENT_START);
		u32 size = obj->size;
		vu_dbuff_write(b, (u8 *)&size, 4);
		vu_dbuff_write(b, obj->init, size);
	}
	return 0;
}

static int write_section(vn_writer *writer, vu_dbuff *buff, u8 sid)
{
	u8 ss = VN_SECTION_START;
	u32 sec_size = (u32)buff->size;
	if (sec_size == 0) {
		return 0;
	}
	fwrite(&ss, 1, 1, writer->file);
	fwrite(&sid, 1, 1, writer->file);
	fwrite(&sec_size, 4, 1, writer->file); // section size
	fwrite(buff->buff, buff->size, 1, writer->file);
	return 0;
}

static int write_header(vn_writer *writer)
{
	static const char *mag = VN_MAGIC;
	static u8 gen = VP_VER_GEN;
	static u8 min = VP_VER_MIN;
	fwrite(mag, 1, 4, writer->file);
	fwrite(&gen, 1, 1, writer->file);
	fwrite(&min, 1, 1, writer->file);
	fwrite(&writer->opt, 2, 1, writer->file);
	return 0;
}

static void vn_writer_free(vn_writer *writer)
{
	vu_dbuff_free(writer->exports);
	vu_dbuff_free(writer->objs);
	vu_dbuff_free(writer->funcs);
	vu_dbuff_free(writer->code);
	vu_dbuff_free(writer->imports);
	vu_free(writer);
}

vn_writer *vn_writer_init(FILE *file, vn_nest *nest)
{
	vn_writer *writer = vu_malloc(sizeof(vn_writer));
	writer->file = file;
	writer->nest = nest;
	writer->opt = 0;
	writer->exports = vu_dbuff_create(VP_BUFF_DEFAULT, 1);
	writer->funcs = vu_dbuff_create(VP_BUFF_DEFAULT, 1);
	writer->code = vu_dbuff_create(VP_BUFF_DEFAULT, 1);
	writer->imports = vu_dbuff_create(VP_BUFF_DEFAULT, 1);
	writer->objs = vu_dbuff_create(VP_BUFF_DEFAULT, 1);
	return writer;
}

int vp_write_nest(FILE *file, vn_nest *nest)
{
	vn_writer *writer = vn_writer_init(file, nest);
	if (read_objs(writer) != 0 || read_funcs(writer) != 0 ||
	    read_imports(writer) != 0) {
		return 1;
	}

	write_header(writer);
	/* write exports */
	write_section(writer, writer->exports, VN_SECTION_EXPORTS);
	/* write objects */
	write_section(writer, writer->objs, VN_SECTION_OBJS);
	/* write functions */
	write_section(writer, writer->funcs, VN_SECTION_FUNCS);
	/* write code */
	write_section(writer, writer->code, VN_SECTION_CODE);
	/* write imports */
	write_section(writer, writer->imports, VN_SECTION_IMPORTS);
	vn_writer_free(writer);
	return 0;
}