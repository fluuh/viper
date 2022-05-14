/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include <viper/builder.h>
#include <viper/util.h>

vb_builder *vb_builder_create(void)
{
	vb_builder *builder = vu_malloc(sizeof(*builder));
	builder->cap_funcs = VP_BUFF_DEFAULT;
	builder->num_funcs = 0;
	builder->funcs = vu_malloc_array(VP_BUFF_DEFAULT,
	                                 sizeof(*builder->funcs));
	return builder;
}

vb_func *vb_func_init(vb_builder *builder,
                      vp_type ret, vp_type *args, u8 num_args)
{
	vb_func *fn = vu_malloc(sizeof(*fn) + sizeof(vp_type) * num_args);
	fn->type.ret = ret;
	fn->type.num_args = num_args;
	memcpy(&fn->type.args, args, sizeof(vp_type) * num_args);
	builder->funcs[builder->num_funcs++] = fn;
	return fn;
}

vb_func *vb_func_create(vb_builder *builder,
                        vp_type ret, vp_type *args, u8 num_args)
{
	vb_func *fn = vb_func_init(builder, ret, args, num_args);
	fn->kind = vb_kind_normal;
	fn->cap_vals = VP_BUFF_DEFAULT;
	fn->num_vals = 0;
	fn->vals = vu_malloc_array(VP_BUFF_DEFAULT, sizeof(*fn->vals));
	fn->cap_blocks = 16; // VP_BUFF_DEFAULT is too big
	fn->num_blocks = 0;
	fn->blocks = vu_malloc_array(8, sizeof(*fn->blocks));
	fn->cap_labels = 16; // same as blocks
	fn->num_blocks = 0;
	fn->labels = vu_malloc_array(8, sizeof(*fn->labels));
	vb_block_create(fn); // initialize first block
	return fn;
}

vb_func *vb_func_native(vb_builder *builder, vp_native_func func,
                        vp_type ret, vp_type *args, u8 num_args)
{
	vb_func *fn = vb_func_init(builder, ret, args, num_args);
	fn->kind = vb_kind_native;
	fn->func = func;
	return fn;
}

vb_block *vb_block_create(vb_func *fn)
{
	vb_block *block = vu_malloc(sizeof(*block));
	block->first = (void*)0;
	block->last = (void*)0;
	if(fn->num_blocks >= fn->cap_blocks) {
		fn->cap_blocks *= 2;
		fn->blocks = vu_realloc(fn->blocks, fn->cap_blocks);
	}
	fn->blocks[fn->num_blocks++] = block;
	fn->current = block;
	return block;
}

vb_inst *vb_inst_init(int size)
{
	vb_inst *inst = vu_malloc(sizeof(*inst) + sizeof(vb_value) * size);
	inst->num = size;
	for(int i = 0; i < size; i++) {
		inst->vals[i].kind = vb_iarg_value;
	}
	return inst;
}

void vb_inst_append(vb_block *block, vb_inst *inst)
{
	if(block->first == (void*)0) {
		block->first = inst;
	}
	inst->next = (void*)0;
	inst->prev = block->last;
	block->last = inst;
}

vb_inst *vb_inst_create(vb_block *block, int size)
{
	vb_inst *inst = vb_inst_init(size);
	vb_inst_append(block, inst);
	return inst;
}

vb_label *vb_label_create(vb_func *fn)
{
	vb_label *lbl = vu_malloc(sizeof(*lbl));
	lbl->name = (void*)0;
	lbl->block = (void*)0;
	if(fn->num_labels >= fn->cap_labels) {
		fn->cap_labels *= 2;
		fn->labels = vu_realloc(fn->labels, fn->cap_labels);
	}
	fn->labels[fn->num_labels++] = lbl;
	return lbl;
}

int vb_label_bind(vb_label *lbl, vb_block *block)
{
	lbl->block = block;
	return 0;
}

vb_value *vb_value_init(vb_func *fn)
{
	vb_value *val = vu_malloc(sizeof(*val));
	if(fn->num_vals >= fn->cap_vals) {
		fn->cap_vals *= 2;
		fn->vals = vu_realloc(fn->vals, fn->cap_vals);
	}
	fn->vals[fn->num_vals++] = val;
	return val;
}

vb_value *vb_reg_create(vb_func *fn, vp_type type)
{
	vb_value *val = vb_value_init(fn);
	val->type = vb_val_reg;
	val->reg.t = type;
	// this will be assigned later
	val->reg.i = 0;
	return val;
}