/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "code.h"

#define OPCODE(ENUM, NAME, TYPE) NAME ,
const char *const vp_bc_name[] = {
	BCDEF(OPCODE)
};
#undef OPCODE

#define OPCODE(ENUM, NAME, TYPE) TYPE ,
const char *const vp_bc_type[] = {
	BCDEF(OPCODE)
};
#undef OPCODE

#define OPCODE(ENUM, NAME, TYPE) sizeof( TYPE ),
const unsigned char vp_bc_length[] = {
	BCDEF(OPCODE)
};
#undef OPCODE

vp_code *vp_code_alloc(size_t len)
{
	vp_code *code = vmalloc(sizeof(*code));
	code->n_reg = 0;
	code->len = len;
	code->ip = vmalloc(len);
	return code;
}

vp_code *vp_code_create(int n_reg, uint8_t *ip, size_t len)
{
	vp_code *code = vp_code_alloc(len);
	code->n_reg = n_reg;
	memcpy(code->ip, ip, len);
	return code;
}

vp_code *vp_code_init(vp_code *code, int n_reg, uint8_t *ip, size_t len)
{
	code->n_reg = n_reg;
	code->len = len;
	code->ip = vmalloc(len);
	memcpy(code->ip, ip, len);
	return code;
}
