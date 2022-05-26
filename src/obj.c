/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include <viper/obj.h>
#include <viper/util.h>

vp_obj *vp_obj_create(const char *name, u8 *init, size_t size)
{
	vp_obj *obj = vu_malloc(sizeof(*obj) + size);
	if (name != NULL) {
		obj->name = vu_malloc(strlen(name) + 1);
		memcpy(obj->name, name, strlen(name) + 1);
	} else {
		obj->name = NULL;
	}
	obj->id = 0; // this is set by the caller
	obj->size = size;
	memcpy(obj->init, init, size);
	return obj;
}

int vp_obj_free(vp_obj *obj)
{
	if(obj->name != NULL) {
		vu_free(obj->name);
	}
	vu_free(obj);
	return 0;
}