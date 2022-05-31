/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

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
}