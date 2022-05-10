/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_BUILDER_H_
#define VP_BUILDER_H_

#include "common.h"
#include "nest.h"

typedef struct vb_label vb_label;

typedef struct vb_reg {
	vp_type t;
	u16 i;
} vb_reg;

typedef struct vb_value {
	enum {
		vb_val_reg,
		vb_val_num,
		vb_val_name,
		vb_val_lbl,
	} type;
	union {
		vb_reg reg;
		i64 num;
		const char *name;
		vb_label *lbl;
	};
} vb_value;

typedef struct vb_inst vb_inst;

struct vb_inst {
	vb_inst *next;
	vb_inst *prev;
	u8 op;
	// these are assumed to be correct
	int num;
	vb_value vals[];
};

typedef struct {
	vb_inst *first;
	vb_inst *last;
} vb_block;

struct vb_label {
	const char *name;
	vb_block *block;
};

typedef struct {
	enum {
		vb_kind_normal,
		vb_kind_native,
	} kind;
	vp_native_func func;
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

vb_func *vb_func_init(vb_builder *builder,
                      vp_type ret, vp_type *args, u8 num_args);

vb_func *vb_func_create(vb_builder *builder, 
                        vp_type ret, vp_type *args, u8 num_args);

vb_func *vb_func_native(vb_builder *builder, vp_native_func func,
                        vp_type ret, vp_type *args, u8 num_args);

vb_block *vb_block_create(vb_func *fn);
vb_inst *vb_inst_init(int size);
void vb_inst_append(vb_block *block, vb_inst *inst);
vb_inst *vb_inst_create(vb_block *block, int size);

// Create an unbound label
vb_label *vb_label_create(vb_func *fn);

int vb_label_bind(vb_label *lbl, vb_block *block);

#endif