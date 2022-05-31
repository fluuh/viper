/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_RUNTIME_VM_H_
#define VP_RUNTIME_VM_H_

struct vi_callframe {
	size_t               size;
	struct vi_callframe *prev;
	struct vi_func      *fn;
	u8                  *ip;
	int                  ret;
	int                  nreg;
	u32                  regs[];
};

#define VM_REG32(vm, i) ((u32 *)&vm->frame->regs[i])
#define VM_REG64(vm, i) ((u64 *)&vm->frame->regs[i])

struct vi_vm {
	struct vi_runtime *r;
	enum {
		vm_normal,
		vm_trapped,
		vm_dead,
	} status;
	int                  code;
	size_t               fsize;
	struct vi_callframe *base;
	struct vi_callframe *frame;
};

struct vi_vm *vi_vm_create(struct vi_runtime *r);
void          vi_vm_delete(struct vi_vm *vm);

int           vi_vm_call(struct vi_vm   *vm,
                         struct vi_func *fn,
                         void           *params,
                         int             num_params,
                         void           *ret);

#endif