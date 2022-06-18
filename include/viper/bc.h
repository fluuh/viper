/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VIPER_BC_H_
#define VIPER_BC_H_

/* type of bytecode inst
 * - d: destination register (i8)
 * - e: destination register i64 (i8)
 * - r: register (i8)
 * - s: register i64 (i8)
 * - t: type (i8)
 * - b: byte (i8)
 * - i: immediate (i16)
 * - j: jump target (i16)
 * - w: word (i32)
 * - l: long (i64)
 * - o: object index (i32)
 * - f: function index (i32)
 */

/* ranges:
 *   0-2: 1+0
 *   3-3: 1+1
 *   4-4: 1+3
 */

#define BCDEF(_)                                                               \
	_(NOP, "nop", "")                                                 \
	_(END, "end", "")                                                 \
	_(RETVOID, "retvoid", "")                                         \
	/* +1 */                                                               \
	_(HALT, "halt", "r")                                                  \
	/* +3 */                                                               \
	_(WRITE, "write", "sss")                                              \
	/* +5 */                                                               \
	_(LDI_W, "ldi.i32", "dw")                                             \
	_(OBJ, "obj", "eo")                                                   \
	/* +8 */                                                               \
	_(LDI_L, "ldi.i64", "el")

extern char *vi_bc_ops[];

enum {
#define OPCODE(name, s, t) OP_##name,
	BCDEF(OPCODE)
#undef OPCODE
};

#endif