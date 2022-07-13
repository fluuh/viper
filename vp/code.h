/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_CODE_H_
#define VP_CODE_H_

#include "common.h"

/* bytecode */
#define BCDEF(_)                                                               \
	_(NOP, "nop", "")                                                      \
	_(END, "end", "")                                                      \
	_(RETVOID, "retvoid", "")                                              \
	_(HALT, "halt", "w")                                                   \
	_(BREAK, "break", "")                                                  \
	_(ADD_U4, "add.i32", "Www")                                            \
	_(ADD_S4, "iadd.i32", "Www")                                           \
	_(ADD_U8, "add.i64", "Lll")                                            \
	_(ADD_S8, "iadd.i64", "Lll")                                           \
	_(PRINT, "print", "Sss")
/* _(SUB_U4, "sub.i32", "Www") \
_(SUB_S4, "isub.i32", "Www") \
_(SUB_U8, "sub.i64", "Lll") \
_(SUB_S8, "isub.i64", "Lll") */

#define OPCODE(ENUM, NAME, TYPE) BC_##ENUM,
typedef enum vp_bc_def {
	BCDEF(OPCODE) BC_LAST_UNUSED, /* to get number of codes */
} vp_bc;
#undef OPCODE

#define VP_NUM_BC (BC_LAST_UNUSED)
extern const char *const vp_bc_name[];
/* see types in code.c */
extern const char *const vp_bc_type[];
extern const unsigned char vp_bc_length[];

typedef struct vp_code_def {
	int n_reg;
	size_t len;
	uint8_t *ip;
} vp_code;

/* allocate code */
vp_code *vp_code_alloc(size_t len);
/* allocate and initialize code */
vp_code *vp_code_create(int n_reg, uint8_t *ip, size_t len);
/* initialize already allocated code */
vp_code *vp_code_init(vp_code *code, int n_reg, uint8_t *ip, size_t len);
/* free code */
void vp_code_free(vp_code *code);

#endif
