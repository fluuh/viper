/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include "core.h"

struct vi_code *vi_code_create(int nreg, size_t size, u8 *code)
{
	struct vi_code *c = vi_malloc(sizeof(*c) + size);
	c->nreg = nreg;
	c->size = size;
	if (code != NULL) {
		memcpy(c->code, code, size);
	}
	return c;
}

struct vi_func *vi_func_create(vp_funcType *type, struct vi_code *code)
{
	struct vi_func *fn = vi_malloc(sizeof(*fn));
	fn->ty = type;
	fn->id = 0;
	fn->nest = NULL;
	fn->code = code;
	return fn;
}

void vi_func_delete(struct vi_func *fn)
{
	vi_free(fn->code);
	vi_func_free(fn);
}

void vi_func_free(struct vi_func *fn)
{
	vi_free(fn);
}