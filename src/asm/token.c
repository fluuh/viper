/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <viper/viper.h>
#include "token.h"

char *vi_tok_str(struct asm_token *tok)
{
	char *str = vi_malloc(tok->len + 1);
	memcpy(str, tok->str, tok->len);
	str[tok->len] = 0;
	return str;
}

static long int char_to_num(char c)
{
	if (c >= 48 && c <= 57) {
		return c - 48;
	} else if (c >= 65 && c <= 70) {
		return 10 + c - 65;
	} else if (c >= 97 && c <= 102) {
		return 10 + c - 97;
	} else {
		return -1;
	}
}

static long int parse_num(const char *str, size_t len, int base)
{
	const char *p = &str[len - 1];
	int mul = 1;
	long int num = 0;
	while (len > 0) {
		long int n = char_to_num(*p--);
		if (n >= base || n < 0) {
			return 0;
		}
		num += n * mul;
		mul *= base;
		len--;
	}
	return num;
}

int vi_tok_num(struct asm_token *tok)
{
	int base;
	switch (tok->num_ty) {
	case (num_bin):
		base = 2;
		break;
	case (num_oct):
		base = 8;
		break;
	case (num_dec):
		base = 10;
		break;
	case (num_hex):
		base = 16;
		break;
	default:
		return 0;
	}
	return parse_num(tok->str, tok->len, base);
}

vp_type vi_tok_type(struct asm_token *tok)
{
	switch (tok->len) {
	case (3):
		if (vi_strcmpn("i32", tok->str, 3))
			return vp_i32;
		if (vi_strcmpn("i64", tok->str, 3))
			return vp_i64;
		break;
	case (4):
		if (vi_strcmpn("void", tok->str, 4))
			return vp_void;
		break;
	}
	return -1;
}

const char *vi_tok_name(int ty)
{
	switch (ty) {
	case (tok_error):
		return "error";
	case (tok_eof):
		return "eof";
	case (tok_id):
		return "identifier";
	case (tok_num):
		return "number";
	case (tok_str):
		return "string";
	case (tok_reg):
		return "register";
	case (tok_ty):
		return "type";
	case (tok_func):
		return "func";
	case (tok_data):
		return "data";
	case (tok_export):
		return "export";
	case (tok_extern):
		return "extern";
	case (tok_lparen):
		return "lparen";
	case (tok_rparen):
		return "rparen";
	case (tok_lbrace):
		return "lbrace";
	case (tok_rbrace):
		return "rbrace";
	case (tok_lbracket):
		return "lbracket";
	case (tok_rbracket):
		return "rbracket";
	case (tok_arrow):
		return "arrow";
	case (tok_eq):
		return "eq";
	case (tok_comma):
		return "comma";
	case (tok_colon):
		return "colon";
	case (tok_label):
		return "label";
	case (tok_newline):
		return "newline";
	default:
		return "unknown";
	}
}
