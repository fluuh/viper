/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_EMIT_H_
#define VP_EMIT_H_

#include "nest.h"
#include "obj.h"

typedef struct vn_reg {
	u8 reg;
	vp_type type;
} vn_reg;

typedef struct vn_bfunc {
	// if it has a name it's exported
	const char* name;
	i32 id;
	size_t size_code;
	size_t cap_code;
	u8 *code;
	u8 rw;
	u8 rl;
	u8 rx;
	size_t num_regs;
	size_t cap_regs;
	vn_reg *regs;
	vn_reg **args;
	size_t num_labels;
	size_t cap_labels;
	// a custom type isn't needed here
	vp_label *labels;
	vp_func_type type;
} vn_bfunc;

typedef struct vn_builder {
	size_t num_funcs;
	size_t cap_funcs;
	vn_bfunc **funcs;
	size_t num_imports;
	size_t cap_imports;
	// a custom type isn't needed here
	vn_import **imports;
	size_t num_objs;
	size_t cap_objs;
	vp_obj **objs;
} vn_builder;

vn_builder *vn_build_create(void);

vn_bfunc *vn_bfunc_create(vn_builder *builder, vp_type ret, 
                        vp_type *args, size_t num_args);
vn_import *vn_bimport(vn_builder *builder, const char *name,
                      vp_type ret, vp_type *args, u8 num_args);
vp_obj *vn_bobject(vn_builder *builder, size_t size, u8 *init);
// doesn't free code
int vn_bfunc_free(vn_bfunc *fn);

// Verify and produce nest
vn_nest *vn_build(vn_builder *builder);
// this doesn't free code or imports
int vn_build_free(vn_builder *builder);

vn_reg *vn_reg_create(vn_bfunc *fn, vp_type type);
u16 vn_label_create(vn_bfunc *fn);
vn_reg *vn_bfunc_arg(vn_bfunc *fn, u8 arg);

/* Emitter Functions */
int vn_emit_label(vn_bfunc *fn, u16 lbl);
int vn_emit_nop(vn_bfunc *fn);
int vn_emit_end(vn_bfunc *fn);
int vn_emit_ret(vn_bfunc *fn, vn_reg *reg);
int vn_emit_jmp(vn_bfunc *fn, u16 lbl);
int vn_emit_neg(vn_bfunc *fn, vn_reg *dst, vn_reg *src);
/* Comparison Ops */
int vn_emit_eqz(vn_bfunc *fn, vn_reg *dst, vn_reg *src);
int vn_emit_eq(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);

int vn_emit_lt_u(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);
int vn_emit_lt_s(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);

int vn_emit_gt_u(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);
int vn_emit_gt_s(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);

int vn_emit_le_u(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);
int vn_emit_le_s(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);

int vn_emit_ge_u(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);
int vn_emit_ge_s(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);
/* Math */
int vn_emit_add(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);
int vn_emit_sub(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);
int vn_emit_mul(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);
int vn_emit_div(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);
int vn_emit_idiv(vn_bfunc *fn, vn_reg *dst, vn_reg *op0, vn_reg *op1);

int vn_emit_branch(vn_bfunc *fn, vn_reg *cond, u16 ctrue, u16 cfalse);
int vn_emit_obj(vn_bfunc *fn, vn_reg *dst, u32 oi);

int vn_emit_ldi(vn_bfunc *fn, vn_reg *dst, u64 num);

int vn_emit_call(vn_bfunc *fn, vn_reg *dst, 
                 vn_bfunc *cf, vn_reg **args, u8 num_args);
int vn_emit_call_import(vn_bfunc *fn, vn_reg *dst,
                        vn_import *import, vn_reg **args, u8 num_args);


#endif