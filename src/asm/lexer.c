/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ctype.h>

#include "lexer.h"
#include "util.h"

static char nextc(struct asm_lexer *lex)
{
	if (lex->i >= lex->len) {
		lex->c = 0;
		return 0;
	}
	lex->c = lex->src[lex->i++];
	if (lex->c == '\n') {
		lex->col = 1;
		lex->row++;
	}
	return lex->c;
}

struct asm_lexer *vi_lexer_create(struct asm_lexer *lex, const char *src)
{
	lex->src = src;
	lex->len = strlen(src);
	lex->i = 0;
	lex->col = 1;
	lex->row = 1;
	nextc(lex);
	return lex;
}

static int get_escape(char c, char *dst)
{
	switch (c) {
	case ('\\'):
	case ('"'):
		*dst = c;
		return 0;
	case ('n'):
		*dst = '\n';
		return 0;
	default:
		/* invalid code */
		return -1;
	}
}

static int bin_ranges[] = {48, 49, 0};
static int oct_ranges[] = {48, 55, 0};
static int dec_ranges[] = {48, 57, 0};
static int hex_ranges[] = {48, 57, 65, 70, 97, 102, 0};

static int is_in_ranges(char c, int *ranges)
{
	unsigned char uc = *(unsigned char *)&c;
	int i = 0;
	while (ranges[i] != 0 && ranges[i + 1] != 0) {
		if (c > ranges[i] && c < ranges[i + 1]) {
			return 1;
		}
		i += 2;
	}
	/* no ranges matched */
	return 0;
}

static int parse_num(struct asm_lexer *lex, struct asm_token *tok)
{
	tok->ty = tok_num;
	int start = lex->i - 1;
	int *ranges;
	if (lex->c == '0') {
		/* not decimal */
		char c = nextc(lex);
		switch (c) {
		case ('b'):
			/* binary */
			ranges = bin_ranges;
			tok->num_ty = num_bin;
			nextc(lex);
			break;
		case ('x'):
			/* hexadecimal */
			ranges = hex_ranges;
			tok->num_ty = num_hex;
			nextc(lex);
			break;
		default:
			/* octal */
			ranges = oct_ranges;
			tok->num_ty = num_oct;
			break;
		}
	} else {
		ranges = dec_ranges;
		tok->num_ty = num_dec;
	}
	while (is_in_ranges(lex->c, ranges)) {
		nextc(lex);
	}
	tok->str = &lex->src[start];
	tok->len = lex->i - 1 - start;
	return 0;
}

static int parse_string(struct asm_lexer *lex, struct asm_token *tok)
{
	const char *str = &lex->src[lex->i];
	int start = lex->i;
	while (nextc(lex) != '\n') {
		if (lex->c == '\\') {
			/* skip escapes */
			nextc(lex);
		}
	}
	tok->ty = tok_str;
	tok->len = lex->i - 2 - start;
	tok->str = str;
	return 0;
}

static int parse_id(struct asm_lexer *lex, struct asm_token *tok)
{
	tok->ty = tok_id;
	int start = lex->i - 1;
	tok->str = &lex->src[start];
	char c = lex->c;
	while (isalpha(c) || isdigit(c) || c == '_' || c == '.') {
		c = nextc(lex);
	}
	tok->len = lex->i - 1 - start;
	return 0;
}

/* check if an identifier is a keyword */
static int id_keyword(const char *s, size_t len)
{
	switch (len) {
	case (4):
		if (vi_strcmpn("func", s, 4))
			return tok_func;
		if (vi_strcmpn("data", s, 4))
			return tok_data;
	}
	return tok_id;
}

static int parse_others(struct asm_lexer *lex, struct asm_token *tok)
{
	char c = lex->c;
	if (isalpha(c) || c == '_') {
		if (parse_id(lex, tok) < 0) {
			return -1;
		}
		tok->ty = id_keyword(tok->str, tok->len);
	} else if (c == '"') {
		return parse_string(lex, tok);
	} else if (c == '.') {
		/* label */
		parse_id(lex, tok);
		if (lex->c != ':') {
			return -1;
		}
		tok->ty = tok_label;
	} else if (isdigit(c)) {
		return parse_num(lex, tok);
	} else {
		return -1;
	}
	return 0;
}

struct asm_token *vi_lexer_next(struct asm_lexer *lex)
{
	struct asm_token *tok = &lex->tok;

	switch (lex->c) {
	case (0):
		tok->ty = tok_eof;
		return tok;
	case (' '):
	case ('\t'):
		nextc(lex);
		return vi_lexer_next(lex);
	case ('\n'):
		tok->ty = tok_newline;
		break;
	case ('%'):
		nextc(lex);
		if (parse_num(lex, tok) < 0) {
			tok->ty = tok_error;
		} else {
			tok->ty = tok_reg;
		}
		return tok;
		break;
	case ('('):
		tok->ty = tok_lparen;
		break;
	case (')'):
		tok->ty = tok_rparen;
		break;
	case ('{'):
		tok->ty = tok_lbrace;
		break;
	case ('}'):
		tok->ty = tok_rbrace;
		break;
	case ('['):
		tok->ty = tok_lbracket;
		break;
	case (']'):
		tok->ty = tok_rbracket;
		break;
	case ('-'):
		tok->str = &lex->src[lex->i];
		char next = nextc(lex);
		if (next == '-') {
			/* comment */
			while (next != '\n' && next != 0) {
				next = nextc(lex);
			}
			nextc(lex);
			return vi_lexer_next(lex);
		} else if (next == '>') {
			tok->ty = tok_arrow;
		} else {
			tok->ty = tok_error;
		}
		tok->len = 2;
		break;
	case ('='):
		tok->ty = tok_eq;
		break;
	case (','):
		tok->ty = tok_comma;
		break;
	default:
		if (parse_others(lex, tok) < 0) {
			tok->str = &lex->src[lex->i - 1];
			tok->len = 1;
			tok->ty = tok_error;
		}
		return tok;
	}
	/* only tokens that don't call nextc reach this */
	nextc(lex);
	return tok;
}