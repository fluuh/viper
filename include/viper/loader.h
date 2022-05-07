/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_LOADER_H_
#define VP_LOADER_H_

#include <stdio.h>
#include "nest.h"
#include "util.h"

#define VN_MAGIC "\x7f""FME"
#define VN_ELEMENT_START 0x6a
#define VN_SECTION_START 0xfa
#define VN_SECTION_EXPORTS 2
#define VN_SECTION_OBJS 4
#define VN_SECTION_FUNCS 6
#define VN_SECTION_CODE 7
#define VN_SECTION_IMPORTS 9

typedef struct {
	FILE *file;
	vn_nest *nest;
	u16 opt;
	vu_dbuff *exports;
	vu_dbuff *funcs;
	vu_dbuff *code;
	vu_dbuff *imports;
	vu_dbuff *objs;
} vn_writer;

// This function generally doesn't need to be called from
// user code.
vn_writer *vn_writer_init(FILE *file, vn_nest *nest);
// Write a nest to file.
int vp_write_nest(FILE *file, vn_nest *nest);

int vp_load_file(FILE *file, vn_nest **nest);

#endif