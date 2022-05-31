/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

<<<<<<< HEAD
#include <viper/core.h>
#include <viper/util.h>

vp_obj *vp_obj_create(u8 *init, size_t size)
{
	vp_obj *obj = vu_malloc(sizeof(*obj) + size);
	obj->id = 0; // this is set by the caller
	obj->size = size;
	memcpy(obj->init, init, size);
	return obj;
}

int vp_obj_free(vp_obj *obj)
{
	vu_free(obj);
	return 0;
=======
#include "core.h"

struct vi_obj *vi_obj_create(size_t size, u8 *initial)
{
	struct vi_obj *obj = vi_malloc(sizeof(*obj) + size);
	obj->id = 0;
	obj->nest = NULL;
	obj->size = size;
	memcpy(&obj->initial, initial, size);
	return obj;
}
void vi_obj_delete(struct vi_obj *obj)
{
	vi_free(obj);
>>>>>>> bb5c964 (fuck it, full rewrite)
}