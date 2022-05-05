/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_LINKER_H_
#define VP_LINKER_H_

#include "nest.h"

typedef struct vn_sym {
	const char *name;
	vp_func *f;
} vn_sym;

typedef struct {
	u32* loc;
	u8 imported;
	union {
		const char *name;
		vp_func *func;
	};
} vn_call;

typedef struct vn_linker {
	vn_nest *out;
	size_t cap_sym;
	size_t num_sym;
	vn_sym *symbols;
	// this should be provided by the verifier
	size_t cap_calls;
	size_t num_calls;
	vn_call *calls;
	size_t cap_nests;
	size_t num_nests;
	vn_nest **nests;
} vn_linker;

vn_linker *vn_linker_create(void);
int vn_linker_add(vn_linker *linker, vn_nest *nest);
// Linking is done in two steps:
// 1. Find all calls and exports.
//    1. Link all local calls to the right functions.
//    2. Redirect external calls to the right symbol.
// 2. Resolve imports and fix all IDs.
//    If an import couldn't be resolved, it's added to
//    a new imports table.
vn_nest* vn_linker_link(vn_linker *linker);

#endif