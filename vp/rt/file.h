/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_RT_FILE_H_
#define VP_RT_FILE_H_

#include "state.h"

typedef uint64_t vp_file;
typedef uint32_t vp_store;
typedef int(*vp_fwrite_t)(vp_state *s, vp_file f);
typedef int(*vp_fread_t)(vp_state *s, vp_file f, vp_ptr to, uint64_t len);
typedef vp_file(*vp_fopen_t)(vp_state *s, vp_store store, const char *path);

typedef struct vp_writer_def {
	vp_fwrite_t write;
	vp_fread_t read;
	vp_fopen_t open;
} vp_writer;

vp_writer *vp_writer_create(vp_fwrite_t write, vp_fread_t read, vp_fopen_t open);
void vp_writer_free(vp_writer *writer);

#endif
