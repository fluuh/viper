/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "emit.h"

/* we can't depend on the definitions in viper */

#define OPCODE(ENUM, NAME, TYPE) sizeof(TYPE) ,
const unsigned char vpe_bc_length[] = {
	BCDEF(OPCODE)
};
#undef OPCODE

#define OPCODE(ENUM, NAME, TYPE) TYPE ,
const char *const vpe_bc_type[] = {
	BCDEF(OPCODE)
};
#undef OPCODE

static vpe_insn *create_insn(vp_bc code)
{
	vpe_insn *insn = vmalloc(sizeof(*insn) +
		                 sizeof(*insn->ops) * vpe_bc_length[code]);
	insn->code = code;
	insn->n_ops = vpe_bc_length[code];
	insn->next = NULL;
	return insn;
}

static vpe_insn *emit(vpe_function *func, int code)
{
	vpe_block *block = func->code.last;
	vpe_insn *insn = create_insn(code);
	if(block->first == NULL) {
		block->first = insn;
	} else {
		block->last->next = insn;
	}
	block->last = insn;
	return insn;
}

vpe_insn *vpe_insn_nop(vpe_function *func)
{
	return emit(func, BC_NOP);
}

vpe_insn *vpe_insn_retvoid(vpe_function *func)
{
	return emit(func, BC_RETVOID);
}

vpe_insn *vpe_insn_halt(vpe_function *func)
{
	return emit(func, BC_HALT);
}

vpe_insn *vpe_insn_break(vpe_function *func)
{
	return emit(func, BC_BREAK);
}
