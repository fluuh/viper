/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_LINKER_H_
#define VP_LINKER_H_

typedef struct vn_sym {
	const char *name;
	vp_func *f;
} vn_sym;

typedef struct {
	enum {
		vn_ref_call,
		vn_ref_obj,
	} type;
	u32 *loc;
	u8 imported;
	union {
		const char *name;
		vp_func *func;
		vp_obj *obj;
	};
} vn_ref;

typedef struct vn_linker {
	vn_nest *out;
	size_t cap_sym;
	size_t num_sym;
	vn_sym *symbols;
	size_t cap_refs;
	size_t num_refs;
	vn_ref *refs;
	size_t cap_nests;
	size_t num_nests;
	vn_nest **nests;
} vn_linker;

vn_linker *vn_linker_create(void);
void vn_linker_free(vn_linker *lk);
int vn_linker_add(vn_linker *linker, vn_nest *nest);
// Linking is done in two steps:
// 1. Find all calls and exports.
//    1. Link all local calls to the right functions.
//    2. Redirect external calls to the right symbol.
// 2. Resolve imports and fix all IDs.
//    If an import couldn't be resolved, it's added to
//    a new imports table.
vn_nest *vn_linker_link(vn_linker *linker);

#endif