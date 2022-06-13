/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_CORE_FUNC_H_
#define VP_CORE_FUNC_H_

struct vi_code {
	int nreg;
	size_t size;
	u8 code[];
};

struct vi_func {
	struct vi_nest *nest;
	u32 id;
	vp_funcType *ty;
	struct vi_code *code;
};

// code can be null
struct vi_code *vi_code_create(int nreg, size_t size, u8 *code);
// void vi_code_delete(struct vi_code *code);

struct vi_func *vi_func_create(vp_funcType *type, struct vi_code *code);
// frees code, doesn't free type
void vi_func_delete(struct vi_func *fn);
void vi_func_free(struct vi_func *fn);

#endif