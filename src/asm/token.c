/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "token.h"

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
	case (tok_label):
		return "label";
	case (tok_newline):
		return "newline";
	default:
		return "unknown";
	}
}
