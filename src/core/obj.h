/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_CORE_OBJ_H_
#define VP_CORE_OBJ_H_

struct vi_obj {
	u32             id;
	struct vi_nest *nest;
	size_t          size;
	u8              initial[];
};

struct vi_obj *vi_obj_create(size_t size, u8 *initial);
void           vi_obj_delete(struct vi_obj *obj);

#endif