/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

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
}