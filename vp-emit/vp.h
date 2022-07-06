/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_EMIT_H_
#define VP_EMIT_H_

/* as long as we don't call any functions this is fine */
/* includes stdio.h and stdint.h */
#include <vp/core.h>

/* This is an emitter api which matches Viper IR very closely.
 * It might be worthwhile to write a more abstract emitter later,
 * but that isn't really useful to me. */

typedef struct vpe_insn_def {
	vp_bc op;
} vpe_insn;

/* a block will automatically be created when emitting
 * a jump or ret instruction */
typedef struct vpe_block_def vpe_block;
struct vpe_block_def {
	vpe_block *next;
	size_t size;
	/* this is useful for emitting jumps */
	size_t pos;
	vpe_insn *first;
	vpe_insn *last;
};

#define VPE_MAX_LABEL_USES 32

typedef struct vpe_label_def {
	int num_uses;
	vpe_block **uses[VPE_MAX_LABEL_USES];
} vpe_label;

typedef struct vpe_code_def {
	int n_blocks;
	vpe_block *last;
} vpe_code;

/* implementing this as a linked list is fine as
 * we'll never have to get an index */
typedef struct vpe_function_def vpe_function;
struct vpe_function_def {
	uint32_t id;
	vpe_function *prev;
	vpe_code code;
};

typedef struct vpe_context_def {
	vpe_function *last_func;
} vpe_context;

typedef struct vpe_nest_def {
	size_t len;
	const char *nest;
} vpe_nest;

vpe_context *vpe_context_create(void);
vpe_function *vpe_function_create(vpe_context *cx);

vpe_block *vpe_block_create(vpe_function *func);

/* Creates a nest. */
vpe_nest *vpe_context_build(vpe_context *cx);

#endif