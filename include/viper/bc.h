/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VIPER_BC_H_
#define VIPER_BC_H_

/* operand types:
 * - d: destination register (i8)
 * - e: destination register i64 (i8)
 * - r: typed register (t, i8)
 * - i: register i32 (i8)
 * - l: register i64 (i8)
 * - t: type (i8)
 * - j: jump target (i16)
 * - 4: word (i32)
 * - 8: long (i64)
 * - o: object index (i32)
 * - f: function index (i32)
 */

/* ranges:
 *   0-2: 1+0
 *   3-3: 1+1
 *   4-4: 1+3
 */

#define BCDEF(_)                                                               \
	_(NOP, "nop", "")                                                      \
	_(END, "end", "")                                                      \
	_(RETVOID, "retvoid", "")                                              \
	/* +1 */                                                               \
	_(HALT, "halt", "i")                                                   \
	/* +3 */                                                               \
	_(WRITE, "write", "lll")                                               \
	/* +5 */                                                               \
	_(LDI_W, "ldi.i32", "d4")                                              \
	_(OBJ, "obj", "eo")                                                    \
	/* +8 */                                                               \
	_(LDI_L, "ldi.i64", "e8")

extern char *vi_bc_ops[];

enum {
#define OPCODE(name, s, t) OP_##name,
	BCDEF(OPCODE)
#undef OPCODE
};

#endif