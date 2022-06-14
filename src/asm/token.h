/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VI_ASM_TOKEN_H_
#define VI_ASM_TOKEN_H_

#include <string.h>

struct asm_token {
	enum {
		tok_error = 0,
		tok_eof,
		tok_id,
		tok_num,
		tok_str,
		tok_reg, /* %n */
		tok_ty,  /* i32, i64, void */
		tok_func,
		tok_data,
		tok_extern,
		tok_export,
		tok_lparen,
		tok_rparen,
		tok_lbrace,
		tok_rbrace,
		tok_lbracket,
		tok_rbracket,
		tok_arrow,
		tok_eq,
		tok_comma,
		tok_colon,
		tok_label, /* dot cut out */
		tok_newline,
	} ty;
	/* the base of a number, used when parsing */
	enum {
		num_bin,
		num_oct,
		num_dec,
		num_hex,
	} num_ty;
	/* str is just a pointer to somewhere in the source
	 * string, so it isn't null-terminated */
	size_t len;
	const char *str;
};

const char *vi_tok_name(int ty);

#endif