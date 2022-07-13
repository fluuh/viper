/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_RT_VM_H_
#define VP_RT_VM_H_

#include "../state.h"

typedef uint64_t vm_reg;

typedef struct vm_frame_def vm_frame;
struct vm_frame_def {
	vm_frame *prev;
	vp_func *func;
	uint8_t *ip;
	int ret;
	size_t start;
	size_t size;
};

typedef struct vp_vm_def {
	vp_state *state;
	vm_frame *frame;
	size_t stack_size;
	vm_reg *stack;
	size_t sp;
} vp_vm;

vp_vm *vp_vm_create(vp_state *s, size_t stack_size);
int vp_vm_call(vp_vm *vm, vp_func *fn, vm_reg *args, int n_args);
void vp_vm_free(vp_vm *vm);
void vp_vm_destroy(vp_vm *vm);

#define VM_REG(vm, i) ((void *)&vm->stack[vm->sp + i])
#define VM_REG_I32(vm, i) ((int32_t))
#define VM_REG_GET(vm, i) (*(vm_reg *)VM_REG(vm, i))
#define VM_REG_SET(vm, i, val) (vm->stack[vm->sp + i] = val)

#endif
