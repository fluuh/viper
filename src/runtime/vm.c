/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/* ;( */
#include <stdbool.h>

#include "runtime.h"
#include <viper/bc.h>

static int vm_dispatch(struct vi_vm *vm, void *ret);

static int vm_cf_push(struct vi_vm   *vm,
                      struct vi_func *fn,
                      void           *params,
                      int             num_params)
{
	struct vi_callframe *prev = vm->frame;
	struct vi_callframe *frame = (void *)((u8 *)prev + prev->size);
	frame->prev = prev;
	frame->fn = fn;
	frame->ip = fn->code->code;
	frame->nreg = fn->code->size;
	frame->size = sizeof(*frame) + sizeof(u32) * frame->nreg;

	vm->frame = frame;
	return 0;
}

static int vm_cf_pop(struct vi_vm *vm, int ret)
{
	struct vi_callframe *frame = vm->frame;
	if (frame == vm->base) {
		/* last call frame, return to caller */
		return 1;
	}
	void *reg = VM_REG32(vm, ret);
	int   ton = frame->ret;
	vm->frame = frame->prev;
	void *to = VM_REG32(vm, ton);
	switch (frame->fn->ty->ret) {
	case (vp_i32):
		*(u32 *)to = *(u32 *)reg;
		break;
	case (vp_i64):
		*(u64 *)to = *(u64 *)reg;
		break;
	default:
		break;
	}
	return 0;
}

struct vi_vm *vi_vm_create(struct vi_runtime *r)
{
	struct vi_vm *vm = vi_malloc(sizeof(*vm));
	vm->r = r;
	vm->status = vm_normal;
	vm->code = 0;
	vm->fsize = 1 << 16;
	vm->base = vi_malloc(vm->fsize);
	vm->frame = vm->base;
	vm->frame->size = 0;
	return vm;
}

void vi_vm_delete(struct vi_vm *vm)
{
	vi_free(vm->base);
	vi_free(vm);
}

int vi_vm_call(struct vi_vm   *vm,
               struct vi_func *fn,
               void           *params,
               int             num_params,
               void           *ret)
{
	vm_cf_push(vm, fn, params, num_params);
	int res = vm_dispatch(vm, ret);
	if (res < 0) {
		/* problem */
		vi_vm_delete(vm);
		return res;
	}

	switch (fn->ty->ret) {
	case (vp_i32):
		break;
	default:
		break;
	}
	vi_vm_delete(vm);
	return 0;
}

static int vm_dispatch(struct vi_vm *vm, void *ret)
{
	register u8                  *ip;
	register u32                 *_reg;
	register struct vi_callframe *frame;
	register struct vi_func      *fn;

#define REG(i) ((void *)(_reg + i))

#define IMM() (*ip++)
#define IMM2() (ip += 2, *(ip - 2))
#define IMM4() (ip += 4, *(ip - 4))
#define IMM8() (ip += 8, *(ip - 8))

#define IREG() REG(IMM())

#define LOAD_FRAME()                                                           \
	do {                                                                   \
		frame = vm->frame;                                             \
		ip = frame->ip;                                                \
		_reg = frame->regs;                                            \
		fn = frame->fn;                                                \
	} while (false)

#define STORE_FRAME()                                                          \
	do {                                                                   \
		frame->ip = ip;                                                \
	} while (false)

#define OP_CASE(name, s, t)                                                    \
	case OP_##name:                                                        \
		goto OPC_##name;
#define NEXT()                                                                 \
	switch (op = IMM()) {                                                  \
		BCDEF(OP_CASE)                                                 \
	}
#define vm_break NEXT()
#define vm_case(op) OPC_##op:

	LOAD_FRAME();

	u8 op;

	vm_break;
	vm_case(NOP)
	{
		vm_break;
	}
	vm_case(END)
	{
		return -2; /* error in code */
	}
	vm_case(RETVOID)
	{
		if (fn->ty->ret != vp_void) {
			/* non-void function have to return something */
			return -2;
		}
		if (vm_cf_pop(vm, 0) == 1) {
			return 0;
		}
		LOAD_FRAME();
		vm_break;
	}
	vm_case(HALT)
	{
		vm->status = vm_dead;
		return 1;
	}
	vm_case(WRITE)
	{ /* TODO: write */
	}
	vm_case(LDI_W)
	{
		u32 *reg = IREG();
		*reg = IMM4();
		vm_break;
	}
	vm_case(OBJ)
	{
		u64 *reg = IREG();
		u32  id = IMM4();
		vm_break;
	}
	vm_case(LDI_L)
	{
		u64 *reg = IREG();
		*reg = IMM8();
		vm_break;
	}

	return -1;
}