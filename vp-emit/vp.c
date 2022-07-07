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
	if (cx->last_func == NULL) {
		func->id = 0;
	} else {
		func->id = cx->last_func->id + 1;
	}
	func->prev = cx->last_func;
	func->code.n_blocks = 0;
	func->code.last = NULL;
	func->code.first = NULL;
	/* insert */
	cx->last_func = func;
	vpe_block_create(func);
	return func;
}

vpe_block *vpe_block_create(vpe_function *func)
{
	vpe_block *block = vmalloc(sizeof(*block));
	block->first = NULL;
	block->last = NULL;
	if (func->code.last == NULL) {
		func->code.first = block;
		block->prev = NULL;
		block->pos = 0;
	} else {
		block->prev = func->code.last;
		block->prev->next = block;
		block->pos = func->code.last->pos + func->code.last->size;
	}
	block->next = NULL;
	block->size = 0;
	func->code.last = block;
	func->code.n_blocks++;
	return block;
}

/* free block and return prev */
static vpe_block *block_free(vpe_block *block)
{
	vpe_insn *insn = block->first;
	while (insn != NULL) {
		vpe_insn *next = insn->next;
		vfree(insn);
		insn = next;
	}
	vpe_block *prev = block->prev;
	vfree(block);
	return prev;
}

/* free function and return prev */
static vpe_function *func_free(vpe_function *func)
{
	/* free blocks */
	vpe_block *block = func->code.last;
	while (block != NULL) {
		block = block_free(block);
	}
	vpe_function *prev = func->prev;
	vfree(func);
	return prev;
}

static void cx_free(vpe_context *cx)
{
	/* free functions */
	vpe_function *func = cx->last_func;
	while (func != NULL) {
		func = func_free(func);
	}
	vfree(cx);
}

vpe_nest *vpe_context_build(vpe_context *cx)
{
	/* TODO: build cx */
	cx_free(cx);
	return NULL;
}
