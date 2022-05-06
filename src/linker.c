/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>

#include <viper/util.h>
#include <viper/linker.h>

vn_linker *vn_linker_create(void)
{
	vn_linker *lk = vu_malloc(sizeof(*lk));
	lk->out = (void*)0;
	lk->cap_sym = VP_BUFF_DEFAULT;
	lk->num_sym = 0;
	lk->symbols = vu_malloc_array(lk->cap_sym, sizeof(*lk->symbols));
	lk->cap_calls = VP_BUFF_DEFAULT;
	lk->num_calls = 0;
	lk->calls = vu_malloc_array(lk->cap_calls, sizeof(*lk->calls));
	lk->cap_nests = VP_BUFF_DEFAULT;
	lk->num_nests = 0;
	lk->nests = vu_malloc_array(lk->cap_nests, sizeof(*lk->nests));
	return lk;
}

static i32 linker_resolve_import(vn_linker *lk, const char* name)
{
	for(int i = 0; i < lk->num_sym; i++) {
		vn_sym *sym = &lk->symbols[i];
		if(sym->name == name) {
			return sym->f->id;
		}
	}
	// TODO: add import
	return 0;
}

// resolve all calls
static int linker_resolve_calls(vn_linker *lk)
{
	for(int i = 0; i < lk->num_calls; i++) {
		vn_call *call = &lk->calls[i];
		i32 f = 0;
		if(call->imported) {
			f = linker_resolve_import(lk, call->name);
		} else {
			f = call->func->id;
		}
		*call->loc = f;
	}
	return 0;
}

// find all symbols
static int linker_symbols(vn_linker *lk) 
{
	for(int i = 0; i < lk->out->num_funcs; i++) {
		// TODO: dynamic array for symbols
		const char* name = lk->out->funcs[i]->name;
		if(name != (void*)0) {
			u32 sid = lk->num_sym;
			lk->num_sym++;
			lk->symbols[sid].name = name;
			lk->symbols[sid].f = lk->out->funcs[i];
		}
	}
	return 0;
}

static int linker_merge_nest(vn_linker *lk, vn_nest *nest)
{
	for(int i = 0; i < nest->num_funcs; i++) {
		u32 loc = lk->out->num_funcs;
		lk->out->num_funcs++;
		lk->out->funcs[loc] = nest->funcs[i];
		lk->out->funcs[loc]->id = loc;
	}
	return 0;
}

// merge all nests
static int linker_merge(vn_linker *lk)
{
	for(int i = 0; i < lk->num_nests; i++) {
		linker_merge_nest(lk, lk->nests[i]);
	}
	return 0;
}

vn_nest *vn_linker_link(vn_linker *lk)
{
	// TODO: link objects as well
	u32 num_funcs = 0;
	u32 num_imports = 0;
	u32 num_objs = 0;
	for(int i = 0; i < lk->num_nests; i++) {
		num_funcs += lk->nests[i]->num_funcs;
		num_objs += lk->nests[i]->num_objs;
	}
	vn_nest *nest = vn_nest_alloc(num_funcs, num_imports, num_objs);
	lk->out = nest;
	linker_merge(lk);
	linker_symbols(lk);
	linker_resolve_calls(lk);
	return nest;
}

static int check_func(vn_linker *lk, vn_nest *nest, vp_func *fn)
{
	u8 *ip = fn->code;
	u8 op;
	#define NEXT() (*ip++)
	#define NEXT4() (ip += 4, *(i32*)(ip - 4))
	#define SKIP(n) (ip += n)
	op = NEXT();
	while(ip <= fn->code + fn->size_code) {
		if(op < 3) {
		} else if(op < 4) {
			SKIP(1);
		} else if(op < 9) {
			SKIP(2);
		} else if(op < 37) {
			SKIP(3);
		} else if(op < 38) {
			SKIP(5);
		} else if(op < 39) {
			SKIP(9);
		} else if(op == 41) {
			// call instruction
			SKIP(2);
			u32* loc = (u32*)ip;
			i32 id = NEXT4();
			u8 toskip = NEXT();
			u32 index = lk->num_calls;
			lk->num_calls++;
			lk->calls[index].loc = loc;
			if(id < 0) {
				// imported function
				vn_import *imp = nest->imports[-id - 1];
				lk->calls[index].name = imp->name;
				lk->calls[index].imported = 1;
			} else {
				lk->calls[index].imported = 0;
				lk->calls[index].func = nest->funcs[id];
			}
			SKIP(toskip * 2);
		} else {
			// invalid instruction
			return 1;
		}
		op = NEXT();
	}
	return 0;
}

int vn_linker_add(vn_linker *lk, vn_nest *nest)
{
	lk->nests[lk->num_nests] = nest;
	lk->num_nests++;
	for(int i = 0; i < nest->num_funcs; i++) {
		if(nest->funcs[i]->ftype == vp_func_normal) {
			int r = check_func(lk, nest, nest->funcs[i]);
			if(r != 0) {
				return 1;
			}
		}
	}
	return 0;
}