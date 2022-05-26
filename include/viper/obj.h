/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_OBJ_H_
#define VP_OBJ_H_

#include "common.h"
#include <stdlib.h>

// An object
// It can be used for global values and
// data.
// It will be placed in memory at runtime
// with an initial value and
// the "obj" instruction will return the
// address of a given object id.
typedef struct {
	char *name;
	u32 id;
	size_t size;
	u8 init[];
} vp_obj;

// name is optional
vp_obj *vp_obj_create(const char *name, u8 *init, size_t size);

int vp_obj_free(vp_obj *obj);

#endif