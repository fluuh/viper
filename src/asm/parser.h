/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VI_ASM_PARSER_H_
#define VI_ASM_PARSER_H_

#include <stdlib.h>

#define ASM_MAX_LABELS 256
#define ASM_MAX_OPERANDS 18 /* memory usage go poof */
#define ASM_MAX_FUNCTIONS 1024
#define ASM_MAX_OBJECTS 1024

/* could use vi_obj, but I don't want to depend on core too much */
struct asm_obj {
	char *name;
	size_t size;
	u8 initial[];
};

struct asm_operand {
	enum {
		asm_reg,
		asm_num,
		asm_name,
		asm_label,
	} ty;
	union {
		i64 num;
		char *str;
	};
};

struct asm_inst {
	struct asm_inst *next;
	struct asm_inst *prev;
	int op;
	int len;
	struct asm_operand args[];
};

struct asm_label {
	char *name;
	int pos;
};

struct asm_code {
	int num_labels;
	struct asm_label labels[ASM_MAX_LABELS]; /* instruction index */
	int num_insts;
	struct asm_inst *first;
	struct asm_inst *last;
};

struct asm_func {
	enum {
		func_private,
		func_exported,
		func_imported,
	} ty;
	int id;
	char *name;
	struct asm_code code;
};

struct asm_unit {
	int num_funcs;
	struct asm_func *funcs[ASM_MAX_FUNCTIONS];
	int num_objs;
	struct asm_obj *objs[ASM_MAX_OBJECTS];
};

void vi_asm_unit_free(struct asm_unit *unit);

#endif