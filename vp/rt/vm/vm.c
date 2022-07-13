/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "vm.h"

static vm_frame *cf_push(vp_vm *vm, vp_func *fn, int dst, vm_reg *args,
                         int n_args);
static vm_frame *cf_pop(vp_vm *vm, vm_reg ret);
static int dispatch(vp_vm *vm);

vp_vm *vp_vm_create(vp_state *s, size_t stack_size)
{
	vp_vm *vm = vmalloc(sizeof(*vm));
	vm->state = s;
	vm->frame = NULL;
	vm->stack_size = stack_size;
	vm->stack = vmalloc(sizeof(*vm->stack) * stack_size);
	return vm;
}

int vp_vm_call(vp_vm *vm, vp_func *fn, vm_reg *args, int n_args)
{
	cf_push(vm, fn, 0, args, n_args);
	return dispatch(vm);
}

void vp_vm_free(vp_vm *vm)
{
	vfree(vm->stack);
	vm_frame *frame = vm->frame;
	while (frame != NULL) {
		vm_frame *prev = frame->prev;
		vfree(frame);
		frame = prev;
	}
	vfree(vm);
}

void vp_vm_destroy(vp_vm *vm)
{
	vp_state_free(vm->state);
	vp_vm_free(vm);
}

/* execution */

/* copying args will probably fail if the stack is resized */
static vm_frame *cf_push(vp_vm *vm, vp_func *fn, int dst, vm_reg *args,
                         int n_args)
{
	/* check if stack has enough space */
	if (vm->sp + vm->frame->size + fn->code->n_reg >= vm->stack_size) {
		/* TODO: resize stack */
		return NULL;
	}
	/* memory for frames should be allocated earlier */
	vm_frame *frame = vmalloc(sizeof(*frame));
	frame->prev = vm->frame;
	frame->ret = dst;
	frame->ip = fn->code->ip;
	frame->func = fn;
	frame->size = fn->code->n_reg;
	frame->start = vm->sp + vm->frame->size;
	vm->sp = frame->start;
	vm->frame = frame;
	/* copy arguments */
	memcpy(&vm->stack[vm->sp], args, sizeof(*args) * n_args);
	return frame;
}
static vm_frame *cf_pop(vp_vm *vm, vm_reg ret)
{
	vm_frame *last = vm->frame;
	int dst = last->ret;
	vm->frame = last->prev;
	vfree(last);
	vm->sp = vm->frame->start;
	VM_REG_SET(vm, dst, ret);
	return vm->frame;
}

static int dispatch(vp_vm *vm)
{
	register vm_reg *_r;
	register vm_frame *frame;
	register uint8_t *ip;
	(void)_r;
	(void)frame;
	(void)ip;

#define IMM() (*ip++)

#define LOAD_FRAME()                                                           \
	do {                                                                   \
		_r = &vm->stack[vm->sp];                                       \
		frame = vm->frame;                                             \
		ip = vm->frame->ip;                                            \
	} while (0)

#define STORE_FRAME() vm->frame->ip = ip

	/* dispatch */
#define NEXT_CASE(ENUM, NAME, FORMAT)                                          \
	case (BC_##ENUM):                                                      \
		goto OPC_##ENUM;
#define NEXT()                                                                 \
	switch (*ip++) {                                                       \
		BCDEF(NEXT_CASE)                                               \
	}
#define vm_break NEXT()
#define vm_loop NEXT();
#define vm_case(code) OPC_##code

	LOAD_FRAME();

	vm_loop;

	vm_case(NOP) : vm_break;
	vm_case(END) : return -1;
	vm_case(RETVOID) :
	{
		/* dunno */
		vm_break;
	}
	vm_case(HALT) :
	{
		uint32_t *reg = VM_REG(vm, IMM());
		(void)reg;
		return 1;
	}
	vm_case(BREAK) : vm_break;
	vm_case(ADD_U4) :
	{
		uint32_t a = *(uint32_t *)VM_REG(vm, IMM());
		uint32_t b = *(uint32_t *)VM_REG(vm, IMM());
		*(uint32_t *)VM_REG(vm, IMM()) = a + b;
		vm_break;
	}
	vm_case(ADD_S4) :
	{
		int32_t a = *(int32_t *)VM_REG(vm, IMM());
		int32_t b = *(int32_t *)VM_REG(vm, IMM());
		*(int32_t *)VM_REG(vm, IMM()) = a + b;
		vm_break;
	}
	vm_case(ADD_U8) :
	{
		uint64_t a = *(uint64_t *)VM_REG(vm, IMM());
		uint64_t b = *(uint64_t *)VM_REG(vm, IMM());
		*(uint64_t *)VM_REG(vm, IMM()) = a + b;
		vm_break;
	}
	vm_case(ADD_S8) :
	{
		int64_t a = *(int64_t *)VM_REG(vm, IMM());
		int64_t b = *(int64_t *)VM_REG(vm, IMM());
		*(int64_t *)VM_REG(vm, IMM()) = a + b;
		vm_break;
	}
	vm_case(PRINT) :
	{
		(void)IMM();
		(void)IMM();
		(void)IMM();
		vm_break;
	}
	return -1;
}
