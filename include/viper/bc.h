/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_BC_H_
#define VP_BC_H_

// TODO: add iarch instructions
#define BCDEF(_) \
	_(NOP, "nop") \
	_(END, "end") \
	_(RETVOID, "retvoid") \
	/* +1 */\
	_(RET, "ret") \
	/* +2 */\
	_(JMP, "jmp") \
	_(NEG_W, "neg.i32") \
	_(NEG_L, "neg.i64") \
	_(EQZ_W, "eqz.i32") \
	_(EQZ_L, "eqz.i64") \
	/* +3 */\
	/* Comparison Ops */\
	_(EQ_W, "eq.i32") \
	_(EQ_L, "eq.i64") \
	_(LT_WU, "lt_u.i32") \
	_(LT_LU, "lt_u.i64") \
	_(LT_WS, "lt_s.i32") \
	_(LT_LS, "lt_s.i64") \
	_(GT_WU, "gt_u.i32") \
	_(GT_LU, "gt_u.i64") \
	_(GT_WS, "gt_s.i32") \
	_(GT_LS, "gt_s.i64") \
	_(LE_WU, "le_u.i32") \
	_(LE_LU, "le_u.i64") \
	_(LE_WS, "le_s.i32") \
	_(LE_LS, "le_s.i64") \
	_(GE_WU, "ge_u.i32") \
	_(GE_LU, "ge_u.i64") \
	_(GE_WS, "ge_s.i32") \
	_(GE_LS, "ge_s.i64") \
	/* Binary Ops */\
	_(ADD_W, "add.i32") \
	_(ADD_L, "add.i64") \
	_(SUB_W, "sub.i32") \
	_(SUB_L, "sub.i64") \
	_(MUL_W, "mul.i32") \
	_(MUL_L, "mul.i64") \
	_(DIV_WU, "div_u.i32") \
	_(DIV_LU, "div_u.i64") \
	_(DIV_WS, "div_s.i32") \
	_(DIV_LS, "div_s.i64") \
	/* +5 */\
	_(OBJ, "obj") \
	_(LDI_W, "ldi.i32") \
	/* +6 */\
	_(BR, "branch") \
	/* +9 */\
	_(LDI_L, "ldi.i64") \
	/* variable length */\
	_(CALL, "call") /* +6 + 2*args */\

enum {
	#define OPCODE(name, _) OP_##name,
	BCDEF(OPCODE)
	#undef OPCODE
};

#endif