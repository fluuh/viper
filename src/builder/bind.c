/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <viper/viper.h>
#include <viper/bc.h>

static size_t block_size(vb_block *block)
{
	size_t size = 0;
	vb_inst *inst = block->first;
	while(inst != (void*)0) {
		size += 1; // opcode
		if(inst->op < 3) {
			
		} else if(inst->op < 4) {
			size += 1;
		} else if(inst->op < 9) {
			size += 2;
		} else if(inst->op < 37) {
			size += 3;
		} else if(inst->op < 39) {
			size += 5;
		} else if(inst->op < 40) {
			size += 6;
		} else if(inst->op < 41) {
			size += 9;
		} else if(inst->op == 41) {
			// call instruction
			size += 1 + inst->num * 2;
		} else {
			return 0;
		}
		inst = inst->next;
	}
	return size;
}

/* TODO: improve register allocator
 * Could be done by using a control-flow tree (maybe built by verifier?)
 */
static int bind_registers(vb_func *fn)
{
	int r32;
	int r64;
	for(int i = 0; i < fn->num_vals; i++) {
		vb_value *val = fn->vals[i];
		if(val->type != vb_val_reg) {
			continue;
		}
		switch(val->reg.t) {
		case(vp_i32):
			val->reg.i = r32++;
			break;
		case(vp_i64):
			val->reg.i = r64++;
			break;
		default:
			return 1;
		}
	}
	fn->r32 = r32;
	fn->r64 = r64;
	return 0;
}


int vb_bind_func(vb_builder *builder, vb_func *fn)
{
	if(bind_registers(fn) != 0) return 1;
	return 0;
}

int vb_bind(vb_builder *builder)
{
	for(int i = 0; i < builder->num_funcs; i++) {
		vb_func *fn = builder->funcs[i];
		if(vb_bind_func(builder, fn) != 0) {
			return 1;
		}
	}
}