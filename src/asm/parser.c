/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "asm.h"
#include "lexer.h"
#include "parser.h"

static struct asm_obj *asm_obj_create(char *name, size_t size, u8 *initial)
{
	struct asm_obj *obj = vi_malloc(sizeof(*obj) + size);
	obj->name = name;
	obj->size = size;
	memcpy(&obj->initial, initial, size);
	return obj;
}

static void asm_obj_free(struct asm_obj *obj)
{
	/* nothing else should depend on the name */
	vi_free(obj->name);
	vi_free(obj);
}

static struct asm_inst *asm_emit(struct asm_code *code, int op,
                                 struct asm_operand *operands, int len)
{
	struct asm_inst *inst =
	    vi_malloc(sizeof(*inst) + sizeof(*inst->args) * len);
	inst->op = op;
	inst->len = len;
	memcpy(inst->args, operands, sizeof(*inst->args) * len);
	/* append instruction to code */
	code->num_insts++;
	if (code->first == NULL) {
		code->first = inst;
	}
	inst->next = NULL;
	inst->prev = code->last;
	code->last = inst;
	return inst;
}

static struct asm_label *asm_emit_label(struct asm_code *code, char *name)
{
	if (code->num_labels + 1 >= ASM_MAX_LABELS) {
		return NULL;
	}
	struct asm_label *lbl = &code->labels[code->num_labels++];
	lbl->name = name;
	lbl->pos = code->num_insts;
	return lbl;
}

static struct asm_code *asm_code_create(struct asm_code *code)
{
	code->num_labels = 0;
	code->num_insts = 0;
	code->first = NULL;
	code->last = NULL;
	return code;
}

/* frees instructions and label names */
static void asm_code_free(struct asm_code *code)
{
	struct asm_inst *inst = code->first;
	while (inst != NULL) {
		for (int i = 0; i < inst->len; i++) {
			if (inst->args[i].ty == asm_name ||
			    inst->args[i].ty == asm_label) {
				vi_free(inst->args[i].str);
			}
		}
		struct asm_inst *tf = inst;
		inst = tf->next;
		vi_free(tf);
	}
	for (int i = 0; i < code->num_labels; i++) {
		vi_free(code->labels[i].name);
	}
}

static struct asm_func *asm_func_create(char *name, int vis, vp_funcType *ty)
{
	struct asm_func *fn = vi_malloc(sizeof(*fn));
	fn->vis = vis;
	fn->ty = ty;
	asm_code_create(&fn->code);
	return fn;
}

static void asm_func_free(struct asm_func *fn)
{
	vi_free(fn->name);
	vi_free(fn->ty);
	asm_code_free(&fn->code);
	vi_free(fn);
}

static struct asm_unit *asm_unit_create(void)
{
	struct asm_unit *unit = vi_malloc(sizeof(*unit));
	unit->num_funcs = 0;
	unit->num_objs = 0;
	return unit;
}

void vi_asm_unit_free(struct asm_unit *unit)
{
	/*
	for(int i = 0; i < unit->num_funcs; i++) {
	 asm_func_free(unit->funcs[i]);
	}
	for(int i = 0; i < unit->num_objs; i++) {
	 asm_obj_free(unit->objs[i]);
	}
	*/
	vi_free(unit);
}

static int asm_func_append(struct asm_func *fn, struct asm_unit *unit)
{
	if (unit->num_funcs + 1 >= ASM_MAX_FUNCTIONS) {
		return -1;
	}
	unit->funcs[unit->num_funcs++] = fn;
	return unit->num_funcs - 1;
}

static int asm_obj_append(struct asm_obj *obj, struct asm_unit *unit)
{
	if (unit->num_objs + 1 >= ASM_MAX_OBJECTS) {
		return -1;
	}
	unit->objs[unit->num_objs++] = obj;
	return unit->num_objs - 1;
}
