/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VIPER_LEXER_H_
#define VIPER_LEXER_H_

#include "token.h"

struct asm_lexer {
	size_t len;
	const char *src;
	char c;
	int i;
	int col; /* starts at 1 */
	int row; /* starts at 1 */
	struct asm_token tok;
};

struct asm_lexer *vi_lexer_create(struct asm_lexer *lex, const char *src);
struct asm_token *vi_lexer_next(struct asm_lexer *lex);

#endif