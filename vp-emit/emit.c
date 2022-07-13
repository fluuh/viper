/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "emit.h"

/* we can't depend on the definitions in viper */

#define OPCODE(ENUM, NAME, TYPE) sizeof(TYPE),
const unsigned char vpe_bc_length[] = {BCDEF(OPCODE)};
#undef OPCODE

#define OPCODE(ENUM, NAME, TYPE) TYPE,
const char *const vpe_bc_type[] = {BCDEF(OPCODE)};
#undef OPCODE

static vpe_insn *create_insn(vp_bc code)
{
	vpe_insn *insn =
	    vmalloc(sizeof(*insn) + sizeof(*insn->ops) * vpe_bc_length[code]);
	insn->code = code;
	insn->n_ops = vpe_bc_length[code];
	insn->next = NULL;
	return insn;
}

static vpe_insn *emit(vpe_function *func, int code)
{
	vpe_block *block = func->code.last;
	vpe_insn *insn = create_insn(code);
	if (block->first == NULL) {
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

vpe_insn *vpe_insn_end(vpe_function *func)
{
	return emit(func, BC_END);
}

vpe_insn *vpe_insn_retvoid(vpe_function *func)
{
	return emit(func, BC_RETVOID);
}

vpe_insn *vpe_insn_halt(vpe_function *func, vpe_reg op0)
{
	vpe_insn *insn = emit(func, BC_HALT);
	insn->ops[0].reg = op0;
	return insn;
}

vpe_insn *vpe_insn_break(vpe_function *func)
{
	return emit(func, BC_BREAK);
}

static vpe_insn *vpe_insn_triple(vpe_function *func, int code, vpe_reg dst, vpe_reg op0, vpe_reg op1)
{
	vpe_insn *insn = emit(func, code);
	insn->ops[0].reg = dst;
	insn->ops[1].reg = op0;
	insn->ops[2].reg = op1;
	return insn;
}

vpe_insn *vpe_insn_add_u4(vpe_function *func, vpe_reg dst, vpe_reg op0, vpe_reg op1)
{
	return vpe_insn_triple(func, BC_ADD_U4, dst, op0, op1);
}

vpe_insn *vpe_insn_add_s4(vpe_function *func, vpe_reg dst, vpe_reg op0, vpe_reg op1)
{
	return vpe_insn_triple(func, BC_ADD_S4, dst, op0, op1);
}

vpe_insn *vpe_insn_add_u8(vpe_function *func, vpe_reg dst, vpe_reg op0, vpe_reg op1)
{
	return vpe_insn_triple(func, BC_ADD_U8, dst, op0, op1);
}

vpe_insn *vpe_insn_add_s8(vpe_function *func, vpe_reg dst, vpe_reg op0, vpe_reg op1)
{
	return vpe_insn_triple(func, BC_ADD_S8, dst, op0, op1);
}

vpe_insn *vpe_insn_print(vpe_function *func, vpe_reg op0, vpe_reg op1, vpe_reg op2)
{
	vpe_insn *insn = emit(func, BC_PRINT);
	insn->ops[0].reg = op0;
	insn->ops[1].reg = op1;
	insn->ops[2].reg = op2;
	return insn;
}
