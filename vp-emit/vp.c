/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "vp.h"

vpe_context *vpe_context_create()
{
	vpe_context *cx = vmalloc(sizeof(*cx));
	cx->last_func = NULL;
	return cx;
}

vpe_function *vpe_function_create(vpe_context *cx)
{
	vpe_function *func = vmalloc(sizeof(*func));
	if(cx->last_func == NULL) {
		func->id = 0;
	} else {
		func->id = cx->last_func->id + 1;
	}
	func->prev = cx->last_func;
	func->code.n_blocks = 0;
	func->code.last = NULL;
	vpe_block_create(func);
	return func;
}

vpe_block *vpe_block_create(vpe_function *func)
{
	vpe_block *block = vmalloc(sizeof(*block));
	block->first = NULL;
	block->last = NULL;
	if(func->code.last == NULL) {
		block->next = NULL;
		block->pos = 0;
	} else {
		block->next = func->code.last;
		block->pos = func->code.last->pos + func->code.last->size;
	}
	block->size = 0;
	func->code.last = block;
	func->code.n_blocks++;
	return block;
}
