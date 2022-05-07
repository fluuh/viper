/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ctype.h>
#include <string.h>

#include <viper/asm.h>
#include <viper/emit.h>
#include <viper/util.h>

/* lexer */

/* error */
enum {
	asmerr_none,
	asmerr_expected,
	asmerr_type,
	asmerr_unknown,
};

typedef struct {
	int pos;
	int col;
	int row;
	int err;
	const char *msg;
} asm_error;

#define ERROR(as, e, m) \
	do { \
		as->err.err = e; \
		as->err.pos = as->i; \
		as->err.col = as->col; \
		as->err.row = as->row; \
		as->err.msg = m; \
	} while(0)

typedef struct token token;

struct token {
	token *next;
	enum {
		tok_nop,
		tok_str,
		tok_num,
		tok_id,
		tok_reg,
		tok_data,
		tok_extern,
		tok_func,
		tok_pub,
		tok_type,
		tok_dot,
		tok_comma,
		tok_lbrace,
		tok_rbrace,
		tok_lparen,
		tok_rparen,
		tok_eq,
		tok_arrow,
		tok_line,
		tok_eof,
	} type;
	union {
		i64 num;
		vp_type vtype;
		char* str;
	};
};

typedef struct as_sym as_sym;

struct as_sym {
	struct as_sym* next;
	struct as_sym* prev;
	const char* name;
	enum {
		sym_data,
		sym_extern,
		sym_func
	} type;
	i32 index;
};

typedef struct {
	vn_builder *builder;
	as_sym* sym;
	as_sym* sym_last;
	token *troot;
	u8 cfin;
	char ch;
	// if compiling from a file
	FILE *file;
	// else
	int i;
	int col;
	int row;
	const char* src;
	token *c;
	asm_error err;
} assembler;

static as_sym *new_symbol(assembler *as, const char *name, 
                          int type, i32 index)
{
	as_sym *sym = vu_malloc(sizeof(*sym));
	sym->index = index;
	sym->name = name;
	sym->type = type;
	if(as->sym == (void*)0) {
		sym->next = (void*)0;
		sym->prev = (void*)0;
		as->sym = sym;
		as->sym_last = sym;
	} else {
		as->sym_last->next = sym;
		sym->prev = as->sym_last;
		sym->next = (void*)0;
		as->sym_last = sym;
	}
	return sym;
}

static as_sym *sym_find(assembler *as, const char *name)
{
	as_sym *current = as->sym;
	while(current != (void*)0) {
		if(strcmp(current->name, name) == 0) {
			return current;
		}
		current = current->next;
	}

	return (void*)0;
}

static token *tok_init(assembler *as)
{
	token *tok = vu_malloc(sizeof(token));
	tok->str = (void*)0;
	tok->next = as->troot;
	as->troot = tok;
	return tok;
}

static char read_char(assembler *as)
{
	if(as->cfin) {
		return as->ch;
	}
	if(as->file != (void*)0) {
		as->i++;
		int c = fgetc(as->file);
		if(c == EOF) {
			as->cfin = 1;
			as->ch = '\0';
			return '\0';
		}
		if(c == '\n') {
			as->col = 0;
			as->row++;
		}
		as->ch = c;
		return c;
	}
	char c = as->i++;
	if(c == 0) {
		as->cfin = 1;
	}
	as->ch = c;
	return c;
}

static token *get_token(assembler *as)
{
	token *tok = tok_init(as);
	char c = as->ch;
	if(c == '\0') {
		tok->type = tok_eof;
		return tok;
	}
	if(c == '\n') {
		c = read_char(as);
		tok->type = tok_line;
		return tok;
	}
	if(isspace(c)) {
		while(isspace(c))
			c = read_char(as);
		return get_token(as);
	}
	if(isalpha(c) || c == '_') {
		// id
		int i = 0;
		char buff[255];
		while(isalnum(c) || c == '_') {
			buff[i] = c;
			i++;
			c = read_char(as);
		}
		buff[i] = '\0';
		i++;
		if(strcmp(buff, "data") == 0) {
			tok->type = tok_data;
			return tok;
		} else if(strcmp(buff, "extern") == 0) {
			tok->type = tok_extern;
			return tok;
		} else if(strcmp(buff, "func") == 0) {
			tok->type = tok_func;
			return tok;
		} else if(strcmp(buff, "pub") == 0) {
			tok->type = tok_pub;
			return tok;
		} else if(strcmp(buff, "void") == 0) {
			tok->type = tok_type;
			tok->vtype = vp_void;
			return tok;
		} else if(strcmp(buff, "i32") == 0) {
			tok->type = tok_type;
			tok->vtype = vp_i32;
			return tok;
		} else if(strcmp(buff, "i64") == 0) {
			tok->type = tok_type;
			tok->vtype = vp_i64;
			return tok;
		} else if(strcmp(buff, "iarch") == 0) {
			tok->type = tok_type;
			tok->vtype = vp_iarch;
			return tok;
		}
		tok->type = tok_id;
		tok->str = vu_malloc(i);
		strcpy(tok->str, buff);
	} else if(isdigit(c)) {
		int i = 0;
		char buff[255];
		while(isdigit(c)) {
			buff[i] = c;
			i++;
			c = read_char(as);
		}
		buff[i] = '\0';
		tok->type = tok_num;
		tok->num = atoi(buff);
	} else if(c == '"') {
		int i = 0;
		char buff[255];
		c = read_char(as);
		while(c != '"') {
			buff[i] = c;
			i++;
			c = read_char(as);
			if(c == '\\') {

			}
		}
		buff[i] = '\0';
		i++;
		c = read_char(as);
		tok->type = tok_str;
		tok->str = vu_malloc(i);
		strcpy(tok->str, buff);
	} else if(c == '%') {
		c = read_char(as);
		if(!isdigit(c)) {
			tok->type = tok_nop;
			return tok;
		}
		int i = 0;
		char buff[255];
		while(isdigit(c)) {
			buff[i] = c;
			i++;
			c = read_char(as);
		}
		buff[i] = '\0';
		tok->type = tok_reg;
		tok->num = atoi(buff);
	} else if(c == '=') {
		c = read_char(as);
		tok->type = tok_eq;	
	} else if(c == '(') {
		read_char(as);
		tok->type = tok_lparen;
		return tok;
	} else if(c == ')') {
		read_char(as);
		tok->type = tok_rparen;
	} else if(c == '.') {
		c = read_char(as);
		tok->type = tok_dot;
	} else if(c == '-') {
		c = read_char(as);
		if(c == '>') {
			tok->type = tok_arrow;
		} else if(c == '-') {
			while(c != '\n')
				c = read_char(as);
			return get_token(as);
		}
		c = read_char(as);
	} else if(c == '{') {
		read_char(as);
		tok->type = tok_lbrace;
	} else if(c == '}') {
		read_char(as);
		tok->type = tok_rbrace;
	} else if(c == '.') {
		read_char(as);
		tok->type = tok_dot;
	} else if(c == ',') {
		read_char(as);
		tok->type = tok_comma;
	} else {
		tok->type = tok_eof;
	}
	return tok;
}

static token *tok_expect(assembler *as, int type)
{
	token *tok = get_token(as);
	if(tok->type == type)
		return tok;
	// TODO: error handling
	return (void*)0;
}

/* parser */

static int parse_data(assembler *as, int pub)
{
	// TODO: add support for data other than strings
	token *name = tok_expect(as, tok_id);
	token *data = tok_expect(as, tok_str);
	size_t size = strlen(data->str) + 1;
	vp_obj *obj = vn_bobject(as->builder, size, (u8*)data->str);
	if(pub) {
		obj->name = name->str;
	}
	new_symbol(as, name->str, sym_data, 0);
	return 0;
}

static int parse_extern(assembler *as)
{
	token *name = tok_expect(as, tok_id);
	tok_expect(as, tok_lparen);
	int num_args = 0;
	vp_type ret;
	vp_type args[255];
	token *tok = get_token(as);
	if(tok->type == tok_type) {
		args[num_args] = tok->vtype;
		num_args++;
		tok = get_token(as);
		while(tok->type == tok_comma) {
			tok = tok_expect(as, tok_type);
			args[num_args] = tok->vtype;
			num_args++;
			tok = get_token(as);
		}
	}
	tok_expect(as, tok_arrow);
	tok = tok_expect(as, tok_type);
	ret = tok->vtype;
	vn_import *imp = vn_bimport(as->builder, name->str,
	                            ret, args, num_args);
	new_symbol(as, name->str, sym_extern, imp->id);
	return 0;
}

static vn_reg *check_reg(vn_bfunc *fn, vn_reg **regs, u8 reg, vp_type type)
{
	if(regs[reg] == (void*)0) {
		if(type == vp_void) {
			// can't create a void register
			return (void*)0;
		}
		// doesn't exist, create it
		regs[reg] = vn_reg_create(fn, type);
		return regs[reg];
	}
	// register exists, check the type
	// if type is void, skip check
	if(type != vp_void && regs[reg]->type != type) {
		return (void*)0;
	}
	return regs[reg];
}

static int parse_call(assembler *as, vn_bfunc *fn, vn_reg **regs, i32 reg)
{
	token *type = tok_expect(as, tok_type);
	token *name = tok_expect(as, tok_id);
	tok_expect(as, tok_lparen);
	vn_reg *args[16]; // no use supporting more than this
	int argc = 0;
	token *tok = get_token(as);
	while(tok->type == tok_type) {
		token *tk = tok_expect(as, tok_reg);
		vn_reg *r = check_reg(fn, regs, tk->num, tok->vtype);
		if(r == (void*)0) {
			return 1;
		}
		args[argc] = r; 
		argc++;
		tok = get_token(as);
		if(tok->type == tok_comma) {
			tok = get_token(as);
		} else if(tok->type == tok_rparen) {
			break;
		}
	}
	vn_reg *to;
	if(reg == -1) {
		if(type->vtype != vp_void) {
			return 1;
		}
		to = (void*)0;
	} else {
		to = check_reg(fn, regs, reg, type->type);
	}
	as_sym *sym = sym_find(as, name->str);
	if(sym == (void*)0) {
		return 1;
	}
	if(sym->type == sym_extern) {
		vn_emit_call_import(fn, to, as->builder->imports[sym->index],
	        	            args, argc);
		return 0;
	} else if(sym->type == sym_func) {
		vn_emit_call(fn, to, as->builder->funcs[sym->index],
	        	     args, argc);
		return 0;
	}
	return 1;
}

static int parse_ret(assembler *as, vn_bfunc *fn, vn_reg **regs) 
{
	token *type = tok_expect(as, tok_type);
	vp_type t = fn->type.ret;
	if(t != type->vtype) {
		return 1;
	}
	token *tok = tok_expect(as, tok_reg);
	vn_reg* reg = check_reg(fn, regs, tok->num, t);
	if(reg == (void*)0) {
		return 1;
	}
	vn_emit_ret(fn, reg);
	return 0;
}

static int parse_ldi(assembler *as, vn_bfunc *fn, vn_reg **regs, u8 reg)
{
	tok_expect(as, tok_dot);
	token *type = tok_expect(as, tok_type);
	vp_type t = type->vtype;
	token *num = tok_expect(as, tok_num);
	vn_reg *r = check_reg(fn, regs, reg, t);
	vn_emit_ldi(fn, r, num->num);
	return 0;
}

static int parse_inst(assembler *as, vn_bfunc *fn, vn_reg **regs, char *name)
{
	if(strcmp(name, "call") == 0) {
		return parse_call(as, fn, regs, -1);
	} else if(strcmp(name, "ret") == 0) {
		return parse_ret(as, fn, regs);
	}
	return 1;
}

static int parse_setter(assembler *as, vn_bfunc *fn,
                        vn_reg **regs, token *_reg)
{
	u8 reg = _reg->num;
	token *tok = get_token(as);
	switch(tok->type) {
	case(tok_type):
		// a declaration
		break;
	case(tok_id): {
		if(strcmp(tok->str, "obj") == 0) {
			vn_reg *r = check_reg(fn, regs, reg, vp_iarch);
			if(r == (void*)0) {
				// welp
				return 1;
			}
			tok = get_token(as);
			if(tok->type != tok_id) {
				// it has to be
				return 1;
			}
			as_sym* obj = sym_find(as, tok->str);
			if(obj == (void*)0) {
				return 1;
			}
			if(obj->type != sym_data) {
				// invalid again
				return 1;
			}
			vn_emit_obj(fn, r, obj->index);
		} else if(strcmp(tok->str, "ldi") == 0) {
			return parse_ldi(as, fn, regs, reg);
		} else if(strcmp(tok->str, "add") == 0) {
			tok_expect(as, tok_dot);
			token *type = tok_expect(as, tok_type);
			vp_type t = type->vtype;
			vn_reg *dst = check_reg(fn, regs, reg, t);
			token *to0 = tok_expect(as, tok_reg);
			vn_reg *op0 = check_reg(fn, regs, to0->num, t);
			tok_expect(as, tok_comma);
			token *to1 = tok_expect(as, tok_reg);
			vn_reg *op1 = check_reg(fn, regs, to1->num, t);
			vn_emit_add(fn, dst, op0, op1);
		} else if(strcmp(tok->str, "sub") == 0) {
			tok_expect(as, tok_dot);
			token *type = tok_expect(as, tok_type);
			vp_type t = type->vtype;
			vn_reg *dst = check_reg(fn, regs, reg, t);
			token *to0 = tok_expect(as, tok_reg);
			vn_reg *op0 = check_reg(fn, regs, to0->num, t);
			tok_expect(as, tok_comma);
			token *to1 = tok_expect(as, tok_reg);
			vn_reg *op1 = check_reg(fn, regs, to1->num, t);
			vn_emit_sub(fn, dst, op0, op1);
		} else if(strcmp(tok->str, "mul") == 0) {
			tok_expect(as, tok_dot);
			token *type = tok_expect(as, tok_type);
			vp_type t = type->vtype;
			vn_reg *dst = check_reg(fn, regs, reg, t);
			token *to0 = tok_expect(as, tok_reg);
			vn_reg *op0 = check_reg(fn, regs, to0->num, t);
			tok_expect(as, tok_comma);
			token *to1 = tok_expect(as, tok_reg);
			vn_reg *op1 = check_reg(fn, regs, to1->num, t);
			vn_emit_mul(fn, dst, op0, op1);
		} else if(strcmp(tok->str, "div") == 0) {
			tok_expect(as, tok_dot);
			token *type = tok_expect(as, tok_type);
			vp_type t = type->vtype;
			vn_reg *dst = check_reg(fn, regs, reg, t);
			token *to0 = tok_expect(as, tok_reg);
			vn_reg *op0 = check_reg(fn, regs, to0->num, t);
			tok_expect(as, tok_comma);
			token *to1 = tok_expect(as, tok_reg);
			vn_reg *op1 = check_reg(fn, regs, to1->num, t);
			vn_emit_div(fn, dst, op0, op1);
		} else if(strcmp(tok->str, "idiv") == 0) {
			tok_expect(as, tok_dot);
			token *type = tok_expect(as, tok_type);
			vp_type t = type->vtype;
			vn_reg *dst = check_reg(fn, regs, reg, t);
			token *to0 = tok_expect(as, tok_reg);
			vn_reg *op0 = check_reg(fn, regs, to0->num, t);
			tok_expect(as, tok_comma);
			token *to1 = tok_expect(as, tok_reg);
			vn_reg *op1 = check_reg(fn, regs, to1->num, t);
			vn_emit_idiv(fn, dst, op0, op1);
		} else if(strcmp(tok->str, "call")) {
			check_reg(fn, regs, reg, vp_void);
			return parse_call(as, fn, regs, reg);
		}
		break;
	}
	default:
		return 1; //problem!
	}
	return 0;
}

static int parse_body(assembler *as, vn_bfunc *fn)
{
	vn_reg *regs[256];
	for(int i = 0; i < 256; i++) {
		regs[i] = (void*)0;
	}
	tok_expect(as, tok_lbrace);
	for(token *tok = get_token(as); tok->type == tok_line;) {
		tok = get_token(as);
		switch(tok->type) {
		case(tok_reg):
			// probably a setter op
			tok_expect(as, tok_eq);
			parse_setter(as, fn, regs, tok);
			break;
		case(tok_id):
			parse_inst(as, fn, regs, tok->str);
			break;
		case(tok_rbrace):
			return 0;
		default:
			// well shit
			return 1;
		}
		tok = get_token(as);
	}
	return 1;
}

static int parse_func(assembler *as, int pub)
{
	// might as well copy this
	token *name = tok_expect(as, tok_id);
	tok_expect(as, tok_lparen);
	int num_args = 0;
	vp_type ret;
	vp_type args[255];
	token *tok = get_token(as);
	if(tok->type == tok_type) {
		args[num_args] = tok->vtype;
		num_args++;
		tok = get_token(as);
		while(tok->type == tok_comma) {
			tok = tok_expect(as, tok_type);
			args[num_args] = tok->vtype;
			num_args++;
			tok = get_token(as);
		}
	}
	tok_expect(as, tok_arrow);
	tok = tok_expect(as, tok_type);
	ret = tok->vtype;
	vn_bfunc *fn = vn_bfunc_create(as->builder,
	                            ret, args, num_args);
	if(pub) {
		fn->name = name->str;
	}
	new_symbol(as, name->str, sym_extern, fn->id);
	return parse_body(as, fn);
}

static int parse_top(assembler *as, int pub) {
	token *tok = get_token(as);
	switch(tok->type) {
	case(tok_pub):
		return parse_top(as, 1);
	case(tok_data):
		return parse_data(as, pub);
	case(tok_extern):
		if(pub) {
			// TODO: error
			return 1;
		}
		return parse_extern(as);
	case(tok_func):
		return parse_func(as, pub);
	case(tok_line):
		return parse_top(as, 0);
	case(tok_eof):
		return 0;
	default:
		return 1;
	}
}

static int top_loop(assembler *as)
{
	int r = parse_top(as, 0);
	for(token *tok = get_token(as); tok->type == tok_line;) {
		if(r != 0) {
			return r;
		}
		r = parse_top(as, 0);
		tok = get_token(as);
	}
	return r;
}

/* init */

static void as_free(assembler *as)
{
	token *tok = as->troot;
	while(tok != (void*)0) {
		token *next = tok->next;
		vu_free(tok);
		tok = next;
	}
	as_sym *sym = as->sym;
	while(sym != (void*)0) {
		as_sym *next = sym->next;
		vu_free(sym);
		sym = next;
	}
	vu_free(as);
	return;
}

static assembler *as_init(void)
{
	assembler *as = vu_malloc(sizeof(*as));
	as->builder = vn_build_create();
	as->cfin = 0;
	as->c = 0;
	as->ch = 0;
	as->troot = (void*)0;
	as->sym = (void*)0;
	as->sym_last = (void*)0;
	as->err.err = asmerr_none;
	return as;
}

vn_nest *vn_assemble_file(FILE *file)
{
	assembler *as = as_init();
	as->file = file;
	as->col = 0;
	as->row = 0;
	as->i = 0;
	read_char(as);
	if(top_loop(as) != 0) {
		return (void*)0;
	}
	vn_nest *nest = vn_build(as->builder);
	as_free(as);
	return nest;
}

vn_nest *vn_assemble(const char* src)
{
	assembler *as = as_init();
	as->file = (void*)0;
	as->i = 0;
	as->src = src;
	read_char(as);
	top_loop(as);
	return vn_build(as->builder);
}