/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_ASM_H_
#define VP_ASM_H_

#include <viper/emitter.h>
#include <viper/asm.h>
#include <viper/viper.h>

#define ASM_MAX_LABELS 256
#define ASM_MAX_OPERANDS 18 /* memory usage go poof */
#define ASM_MAX_INSTS 1024  /* seriously, fuck memory */

/* could use vi_obj, but I don't want to depend on core too much */
struct asm_obj {
	char *name;
	size_t size;
	u8 initial[];
};

struct asm_inst {
	int len;
	int op;
	vp_value args[];
};

struct asm_code {
	int num_labels;
	int labels[ASM_MAX_LABELS]; /* instruction index */
	int num_insts;
	struct asm_inst insts[ASM_MAX_INSTS];
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

struct vi_asm {
	const char *src;
	int i;
};

#endif