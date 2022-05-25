/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_BC_H_
#define VP_BC_H_

/* ranges:
 * 0-2: +0
 * 3: +1
 * 4-8: +2
 * 9-36: +3
 * 37-38: +5
 * 39: +6
 * 40: +9
 * 41: variable length
 */

/* Control flow effect */
#define BC_CFE_NONE 0
#define BC_CFE_BR 1
#define BC_CFE_CALL 2
#define BC_CFE_RET 3

/* Op types */

/*
 * used for building
 * r: register
 * l: label
 * w: i32
 * d: i64
 */
typedef enum {
	bc_none,
	bc_r,
	bc_l,
	bc_rr,
	bc_rrr,
	bc_rw,
	bc_rll,
	bc_rd,
	bc_call,
} bc_type;

/* _(op, str, type) */
#define BCDEF(_)                                                               \
	_(NOP, "nop", bc_none)                                                 \
	_(END, "end", bc_none)                                                 \
	_(RETVOID, "retvoid", bc_none)                                         \
	/* +1 */                                                               \
	_(RET, "ret", bc_r)                                                    \
	/* +2 */                                                               \
	_(JMP, "jmp", bc_l)                                                    \
	_(NEG_W, "neg.i32", bc_rr)                                             \
	_(NEG_L, "neg.i64", bc_rr)                                             \
	_(EQZ_W, "eqz.i32", bc_rr)                                             \
	_(EQZ_L, "eqz.i64", bc_rr)                                             \
	/* +3 */                                                               \
	_(EQ_W, "eq.i32", bc_rrr)                                              \
	_(EQ_L, "eq.i64", bc_rrr)                                              \
	_(LT_WU, "lt_u.i32", bc_rrr)                                           \
	_(LT_LU, "lt_u.i64", bc_rrr)                                           \
	_(LT_WS, "lt_s.i32", bc_rrr)                                           \
	_(LT_LS, "lt_s.i64", bc_rrr)                                           \
	_(GT_WU, "gt_u.i32", bc_rrr)                                           \
	_(GT_LU, "gt_u.i64", bc_rrr)                                           \
	_(GT_WS, "gt_s.i32", bc_rrr)                                           \
	_(GT_LS, "gt_s.i64", bc_rrr)                                           \
	_(LE_WU, "le_u.i32", bc_rrr)                                           \
	_(LE_LU, "le_u.i64", bc_rrr)                                           \
	_(LE_WS, "le_s.i32", bc_rrr)                                           \
	_(LE_LS, "le_s.i64", bc_rrr)                                           \
	_(GE_WU, "ge_u.i32", bc_rrr)                                           \
	_(GE_LU, "ge_u.i64", bc_rrr)                                           \
	_(GE_WS, "ge_s.i32", bc_rrr)                                           \
	_(GE_LS, "ge_s.i64", bc_rrr)                                           \
	_(ADD_W, "add.i32", bc_rrr)                                            \
	_(ADD_L, "add.i64", bc_rrr)                                            \
	_(SUB_W, "sub.i32", bc_rrr)                                            \
	_(SUB_L, "sub.i64", bc_rrr)                                            \
	_(MUL_W, "mul.i32", bc_rrr)                                            \
	_(MUL_L, "mul.i64", bc_rrr)                                            \
	_(DIV_WU, "div_u.i32", bc_rrr)                                         \
	_(DIV_LU, "div_u.i64", bc_rrr)                                         \
	_(DIV_WS, "div_s.i32", bc_rrr)                                         \
	_(DIV_LS, "div_s.i64", bc_rrr)                                         \
	/* +5 */                                                               \
	_(OBJ, "obj", bc_rw)                                                   \
	_(LDI_W, "ldi.i32", bc_rw)                                             \
	/* +6 */                                                               \
	_(BR, "br", bc_rll)                                                    \
	/* +9 */                                                               \
	_(LDI_L, "ldi.i64", bc_rd)                                             \
	/* variable length */                                                  \
	_(CALL, "call", bc_call) /* +6 + 2*args */

enum {
#define OPCODE(name, s, t) OP_##name,
	BCDEF(OPCODE)
#undef OPCODE
};

#endif