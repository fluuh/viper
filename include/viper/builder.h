/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_BUILDER_H_
#define VP_BUILDER_H_

#include "common.h"
#include "nest.h"

typedef struct vb_label_s vb_label;

typedef struct vb_reg_s {
	vp_type t;
	int i;
} vb_reg;

typedef struct vb_const_s {
	vp_type type;
	u64 val;
} vb_const;

typedef struct vb_value_s {
	enum {
		vb_val_reg,
		vb_val_const,
	} type;
	union {
		vb_reg reg;
		vb_const c;
	};
} vb_value;

typedef struct vb_inst_s vb_inst;

struct vb_inst_s {
	vb_inst *next;
	vb_inst *prev;
	u8 op;
	// these are assumed to be correct
	int num;
	struct {
		enum {
			vb_iarg_value,
			vb_iarg_label,
		} kind;
		union {
			vb_value *val;
			vb_label *lbl;
		};
	} vals[];
};

typedef struct {
	vb_inst *first;
	vb_inst *last;
} vb_block;

struct vb_label_s {
	const char *name;
	vb_block *block;
};

typedef struct {
	u32 id;
	enum {
		vb_kind_normal,
		vb_kind_native,
	} kind;
	char *name;
	vp_native_func func;
	int regn;
	// args are the first values
	size_t cap_vals;
	size_t num_vals;
	vb_value **vals;
	size_t cap_blocks;
	size_t num_blocks;
	vb_block **blocks;
	vb_block *current;
	size_t cap_labels;
	size_t num_labels;
	vb_label **labels;
	vp_func_type type;
} vb_func;

typedef struct {
	size_t cap_funcs;
	size_t num_funcs;
	vb_func **funcs;
} vb_builder;

vb_builder *vb_builder_create(void);

vb_func *vb_func_init(const char *name, vb_builder *builder, vp_type ret, vp_type *args,
                      u8 num_args);

vb_func *vb_func_create(const char *name, vb_builder *builder, vp_type ret,
                        vp_type *args, u8 num_args);

vb_func *vb_func_native(const char *name, vb_builder *builder,
                        vp_native_func func, vp_type ret, vp_type *args,
                        u8 num_args);

vb_block *vb_block_create(vb_func *fn);
vb_inst *vb_inst_init(int size);
void vb_inst_append(vb_block *block, vb_inst *inst);
vb_inst *vb_inst_create(vb_block *block, int size);

// Create an unbound label
vb_label *vb_label_create(vb_func *fn);

int vb_label_bind(vb_label *lbl, vb_block *block);

vb_value *vb_value_init(vb_func *fn);

vb_value *vb_reg_create(vb_func *fn, vp_type type);

/* Emitter */
vb_inst *vb_emit_nop(vb_func *fn);
vb_inst *vb_emit_end(vb_func *fn);
vb_inst *vb_emit_ret(vb_func *fn, vb_value *val);
vb_inst *vb_emit_jmp(vb_func *fn, vb_label *lbl);
vb_inst *vb_emit_neg(vb_func *fn, vb_value *dst, vb_value *op);
vb_inst *vb_emit_eqz(vb_func *fn, vb_value *dst, vb_value *op);
vb_inst *vb_emit_eq(vb_func *fn, vb_value *dst, vb_value *op0, vb_value *op1);
vb_inst *vb_emit_add(vb_func *fn, vb_value *dst, vb_value *op0, vb_value *op1);
vb_inst *vb_emit_sub(vb_func *fn, vb_value *dst, vb_value *op0, vb_value *op1);
vb_inst *vb_emit_mul(vb_func *fn, vb_value *dst, vb_value *op0, vb_value *op1);
vb_inst *vb_emit_div(vb_func *fn, vb_value *dst, vb_value *op0, vb_value *op1);
vb_inst *vb_emit_idiv(vb_func *fn, vb_value *dst, vb_value *op0, vb_value *op1);
vb_inst *vb_emit_ldi(vb_func *fn, vb_value *dst, vb_value *op);
vb_inst *vb_emit_br(vb_func *fn, vb_value *cond, vb_label *ctrue,
                    vb_label *cfalse);

int vb_bind_func(vb_builder *builder, vb_func *fn);
int vb_bind(vb_builder *builder);
int vb_build(vb_builder *builder, vn_nest **dst);

#endif