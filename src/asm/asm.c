/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "asm.h"
#include "lexer.h"

vp_nest vp_assemble(const char *src)
{
	// struct vi_asm as = vi_asm_parse(src);
	struct asm_lexer *lex = vi_malloc(sizeof(*lex));
	vi_lexer_create(lex, src);
	struct asm_token *tok = vi_lexer_next(lex);
	while (tok->ty != tok_eof) {
		printf("%s  \"%.*s\"\n", vi_tok_name(tok->ty), (int)tok->len,
		       tok->str);
		tok = vi_lexer_next(lex);
	}
	return NULL;
}
