/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <viper/builder.h>
#include <viper/bc.h>

/*
 * If the type of the value is register, return
 * the value. If it's a constant, create a new
 * register and load the constant into it.
 */
static vb_value *val2reg(vb_func *fn, vb_value *val)
{
	if(val->type == vb_val_reg) {
		return val;
	} else if(val->type == vb_val_const) {
		vb_value *reg = vb_reg_create(fn, val->c.type);
		vb_emit_ldi(fn, reg, val);
		return reg;
	} else {
		return (void*)0;
	}
}

vb_inst *vb_emit_nop(vb_func *fn)
{
	vb_inst *inst = vb_inst_create(fn->current, 0);
	inst->op = OP_NOP;
	return inst;
}

vb_inst *vb_emit_end(vb_func *fn)
{
	vb_inst *inst = vb_inst_create(fn->current, 0);
	inst->op = OP_END;
	return inst;
}

vb_inst *vb_emit_ret(vb_func *fn, vb_value *val)
{
	if(val == (void*)0) {
		vb_inst *inst = vb_inst_create(fn->current, 1);
		inst->op = OP_RETVOID;
		vb_block_create(fn);
		return inst;
	} else {
		vb_inst *inst = vb_inst_create(fn->current, 1);
		val = val2reg(fn, val);
		inst->op = OP_RET;
		inst->vals[0].val = val;
		vb_block_create(fn);
		return inst;
	}
}

vb_inst *vb_emit_jmp(vb_func *fn, vb_label *lbl)
{
	vb_inst *inst = vb_inst_create(fn->current, 1);
	inst->vals[0].kind = vb_iarg_label;
	inst->vals[0].lbl = lbl;
	return inst;
}

vb_inst *vb_emit_neg(vb_func *fn, vb_value *dst, vb_value *op)
{
	if(dst->type != vb_val_reg) {
		return (void*)0;
	}
	op = val2reg(fn, op);
	vb_inst *inst = vb_inst_create(fn->current, 2);
	inst->op = OP_NEG_W + -(op->reg.t) - 1;
	inst->vals[0].val = dst;
	inst->vals[1].val = op;
	return inst;
}

vb_inst *vb_emit_eqz(vb_func *fn, vb_value *dst, vb_value *op)
{
	if(dst->type != vb_val_reg || op->type != vb_val_reg) {
		return (void*)0;
	}
	if(dst->reg.t != op->reg.t) {
		return (void*)0;
	}
	op = val2reg(fn, op);
	vb_inst *inst = vb_inst_create(fn->current, 2);
	switch(dst->reg.t) {
	case(vp_i32):
		inst->op = OP_EQZ_W;
		break;
	case(vp_i64):
		inst->op = OP_EQZ_L;
		break;
	default:
		return (void*)0;
	}
	inst->vals[0].val = dst;
	inst->vals[1].val = op;
	return inst;
}

vb_inst *vb_emit_eq(vb_func *fn, vb_value *dst, vb_value *op0, vb_value *op1)
{
	if(dst->type == vb_val_reg) {
		return (void*)0;
	}
	op0 = val2reg(fn, op0);
	op1 = val2reg(fn, op1);
	vb_inst *inst = vb_inst_create(fn->current, 3);
	vp_type type = dst->reg.t;
	if(op0->reg.t != type || op1->reg.t != type) {
		return (void*)0;
	}
	switch(type) {
	case(vp_i32):
		inst->op = OP_EQ_W;
		break;
	case(vp_i64):
		inst->op = OP_EQ_L;
		break;
	default:
		return (void*)0;
	}
	inst->vals[0].val = dst;
	inst->vals[1].val = op0;
	inst->vals[2].val = op1;
	return inst;
}

static vb_inst *vb_emit_arith(vb_func *fn, int op, vb_value *dst,
			      vb_value *op0, vb_value *op1)
{
	if(dst->type != vb_val_reg) {
		return (void*)0;
	}
	vb_inst *inst = vb_inst_create(fn->current, 3);
	inst->op = op - dst->reg.t - 1;
	inst->vals[0].val = dst;
	inst->vals[1].val = val2reg(fn, op0);
	inst->vals[2].val = val2reg(fn, op1);
	return inst;
}

vb_inst *vb_emit_add(vb_func *fn, vb_value *dst,
		     vb_value *op0, vb_value *op1)
{
	return vb_emit_arith(fn, OP_ADD_W, dst, op0, op1);
}

vb_inst *vb_emit_ldi(vb_func *fn, vb_value *dst, vb_value *op)
{
	if(dst->type != vb_val_reg || op->type != vb_val_const) {
		return (void*)0;
	}
	if(dst->reg.t != op->c.type) {
		return (void*)0;
	}
	vb_inst *inst = vb_inst_create(fn->current, 2);
	switch(dst->reg.t) {
	case(vp_i32):
		inst->op = OP_LDI_W;
		break;
	case(vp_i64):
		inst->op = OP_LDI_L;
		break;
	default:
		return (void*)0;
	}
	inst->vals[0].kind = vb_iarg_value;
	inst->vals[0].val = dst;
	inst->vals[1].kind = vb_iarg_value;
	inst->vals[1].val = op;
	return inst;
}