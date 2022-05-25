/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <string.h>

#include <viper/bc.h>
#include <viper/emit.h>
#include <viper/util.h>

vn_reg *vn_reg_create(vn_bfunc *fn, vp_type type)
{
	if (fn->cap_regs <= fn->num_regs) {
		fn->cap_regs *= 2;
		fn->regs =
		    vu_realloc(fn->regs, sizeof(*fn->regs) * fn->cap_regs);
	}
	vn_reg *reg = &fn->regs[fn->num_regs];
	fn->num_regs++;
	reg->type = type;
	switch (type) {
	case (vp_i32): {
		reg->reg = fn->rw++;
		break;
	}
	case (vp_i64): {
		reg->reg = fn->rl++;
		break;
	}
	case (vp_iarch): {
		reg->reg = fn->rx++;
		break;
	}
	default: {
		// TODO: proper error handling
		return (void *)0;
	}
	}
	return reg;
}

u16 vn_label_create(vn_bfunc *fn)
{
	if (fn->cap_labels <= fn->num_labels) {
		fn->cap_labels *= 2;
		fn->labels = vu_realloc(fn->labels,
		                        sizeof(*fn->labels) * fn->cap_labels);
	}
	vp_label *lbl = &fn->labels[fn->num_labels];
	fn->num_labels++;
	lbl->pos = 0;
	return fn->num_labels - 1;
}

/* Emitter Functions */

static int emit_bytes(vn_bfunc *fn, u8 *src, size_t size)
{
	if (fn->cap_code <= fn->size_code + size) {
		fn->cap_code *= 2;
		fn->code =
		    vu_realloc(fn->code, sizeof(*fn->code) * fn->cap_code);
		return emit_bytes(fn, src, size);
	}
	memcpy(&fn->code[fn->size_code], src, size);
	fn->size_code += size;
	return 0;
}

static int emit_byte(vn_bfunc *fn, u8 src)
{
	if (fn->cap_code <= fn->size_code + 1) {
		fn->cap_code *= 2;
		fn->code =
		    vu_realloc(fn->code, sizeof(*fn->code) * fn->cap_code);
	}
	fn->code[fn->size_code] = src;
	fn->size_code++;
	return 0;
}

static int emit_uny(vn_bfunc *fn, u8 op, vn_reg *dst, vn_reg *src)
{
	vp_type type = dst->type;
	if (src->type != type) {
		return 1;
	}
	switch (type) {
	case (vp_i32): {
		emit_byte(fn, op); // e.g. neg.i32
		break;
	}
	case (vp_i64): {
		emit_byte(fn, op + 1); // e.g. neg.i64
		break;
	}
	default: {
		return 1;
	}
	}
	emit_byte(fn, dst->reg);
	emit_byte(fn, src->reg);
	return 0;
}

static int emit_bin(vn_bfunc *fn, u8 op, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	vp_type type = dst->type;
	if (op0->type != type || op1->type != type) {
		return 1;
	}
	switch (type) {
	case (vp_i32): {
		emit_byte(fn, op); // e.g. add.i32
		break;
	}
	case (vp_i64): {
		emit_byte(fn, op + 1); // e.g. add.i64
		break;
	}
	default: {
		return 1;
	}
	}
	emit_byte(fn, dst->reg);
	emit_byte(fn, op0->reg);
	emit_byte(fn, op1->reg);
	return 0;
}

int vn_emit_label(vn_bfunc *fn, u16 lbl)
{
	if (lbl >= fn->num_labels) {
		return 1;
	}
	fn->labels[lbl].pos = fn->size_code;
	return 0;
}

vn_reg *vn_bfunc_arg(vn_bfunc *fn, u8 arg) { return fn->args[arg]; }

int vn_emit_nop(vn_bfunc *fn) { return emit_byte(fn, OP_NOP); }

int vn_emit_end(vn_bfunc *fn) { return emit_byte(fn, OP_END); }

int vn_emit_ret(vn_bfunc *fn, vn_reg *reg)
{
	if (reg->type == fn->type.ret) {
		emit_byte(fn, OP_RET);
		emit_byte(fn, reg->type);
		emit_byte(fn, reg->reg);
		return 0;
	} else {
		return 1;
	}
}
int vn_emit_jmp(vn_bfunc *fn, u16 lbl)
{
	if (lbl >= fn->num_labels) {
		return 1;
	}
	emit_byte(fn, OP_JMP);
	emit_bytes(fn, (u8 *)&lbl, 2);
	return 0;
}

int vn_emit_neg(vn_bfunc *fn, vn_reg *dst, vn_reg *src)
{
	return emit_uny(fn, OP_NEG_W, dst, src);
}
/* Comparison Ops */
int vn_emit_eqz(vn_bfunc *fn, vn_reg *dst, vn_reg *src)
{
	return emit_uny(fn, OP_EQZ_W, dst, src);
}
int vn_emit_eq(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_EQ_L, dst, op0, op1);
}

int vn_emit_lt_u(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_LT_WU, dst, op0, op1);
}

int vn_emit_lt_s(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_LT_WS, dst, op0, op1);
}

int vn_emit_gt_u(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_GT_WU, dst, op0, op1);
}

int vn_emit_gt_s(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_GT_WS, dst, op0, op1);
}

int vn_emit_le_u(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_LE_WU, dst, op0, op1);
}

int vn_emit_le_s(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_LE_WS, dst, op0, op1);
}

int vn_emit_ge_u(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_GE_WU, dst, op0, op1);
}

int vn_emit_ge_s(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_GE_WS, dst, op0, op1);
}

/* Math */
int vn_emit_add(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_ADD_W, dst, op0, op1);
}

int vn_emit_sub(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_SUB_W, dst, op0, op1);
}

int vn_emit_mul(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_MUL_W, dst, op0, op1);
}

int vn_emit_div(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_DIV_WU, dst, op0, op1);
}

int vn_emit_idiv(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1)
{
	return emit_bin(fn, OP_DIV_WS, dst, op0, op1);
}

int vn_emit_branch(vn_bfunc *fn, vn_reg *cond, u16 ctrue, u16 cfalse)
{
	if (fn->num_labels <= ctrue || fn->num_labels <= cfalse) {
		return 1;
	}
	emit_byte(fn, OP_BR);
	emit_byte(fn, cond->type);
	emit_byte(fn, cond->reg);
	emit_bytes(fn, (u8 *)&ctrue, 2);
	emit_bytes(fn, (u8 *)&cfalse, 2);
	return 0;
}

int vn_emit_obj(vn_bfunc *fn, vn_reg *dst, u32 oi)
{
	if (dst->type != vp_iarch) {
		return 1;
	}
	emit_byte(fn, OP_OBJ);
	emit_byte(fn, dst->reg);
	emit_bytes(fn, (u8 *)&oi, 4);
	return 0;
}

int vn_emit_ldi(vn_bfunc *fn, vn_reg *dst, u64 num)
{
	switch (dst->type) {
	case (vp_i32): {
		emit_byte(fn, OP_LDI_W);
		emit_byte(fn, dst->reg);
		emit_bytes(fn, (u8 *)&num, 4);
		break;
	}
	case (vp_i64): {
		emit_byte(fn, OP_LDI_L);
		emit_byte(fn, dst->reg);
		emit_bytes(fn, (u8 *)&num, 8);
		break;
	}
	default: {
		return 1;
	}
	}
	return 0;
}

static int emit_call(vn_bfunc *fn, vn_reg *dst, i32 id, u8 num_args,
                     vn_reg **args)
{
	emit_byte(fn, OP_CALL);
	if (dst == (void *)0) {
		// the function doesn't return anything
		emit_byte(fn, vp_void);
		emit_byte(fn, 0);
	} else {
		emit_byte(fn, dst->type);
		emit_byte(fn, dst->reg);
	}
	emit_bytes(fn, (u8 *)&id, 4);
	emit_byte(fn, num_args);
	for (int i = 0; i < num_args; i++) {
		emit_byte(fn, args[i]->type);
		emit_byte(fn, args[i]->reg);
	}
	return 0;
}

int vn_emit_call_import(vn_bfunc *fn, vn_reg *dst, vn_import *import,
                        vn_reg **args, u8 num_args)
{
	return emit_call(fn, dst, -1 - import->id, num_args, args);
}

int vn_emit_call(vn_bfunc *fn, vn_reg *dst, vn_bfunc *cf, vn_reg **args,
                 u8 num_args)
{
	return emit_call(fn, dst, cf->id, num_args, args);
}