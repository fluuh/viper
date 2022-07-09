/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_CODE_H_
#define VP_CODE_H_

#include "common.h"

/* bytecode */
#define BCDEF(_) \
	_(NOP, "nop", "") \
	_(END, "end", "")

#define OPCODE(ENUM, NAME, TYPE) BC_##ENUM,
typedef enum vp_bc_def {
	BCDEF(OPCODE) BC_LAST_UNUSED, /* to get number of codes */
} vp_bc;
#undef OPCODE

#define VP_NUM_BC (BC_LAST_UNUSED)
extern const char *const vp_bc_name[];
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
