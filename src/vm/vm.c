/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <viper/state.h>
#include <viper/bc.h>

static int dispatch(vp_state *state);

static int push_callframe(vp_state *state, vp_func *fn)
{
	ve_callframe *fp = state->fp + state->fp->size;
	fp->prev = state->fp;
	state->fp = fp;
	size_t r32 = sizeof(u32) * fn->r32;
	size_t r64 = sizeof(u64) * fn->r64;
	size_t rxx = sizeof(uxx) * fn->rxx;
	size_t size = sizeof(*fp) + r32 + r64 + rxx;
	fp->size = size;
	fp->r32 = sizeof(*fp);
	fp->r64 = fp->r32 + r32;
	fp->rxx = fp->r64 + r64;
	fp->fn = fn;
	if(fn->ftype == vp_func_normal) {
		fp->ip = fn->code;
	}
	fp->ret_offset = 0; // this is set by the caller
	return 0;
}

// This function assumes that there is a previous callframe
static int pop_callframe(vp_state *state)
{
	state->fp = state->fp->prev;
	return 0;
}

int vp_call_func(vp_state *state, vp_export fi, void *res, 
                 u64 *args, int num_args)
{
	vp_func *fn = state->nest->funcs[fi];
	push_callframe(state, fn);
	// TODO: load args
	int ret = dispatch(state);
	switch(fn->type.ret) {
	case(vp_i32):
		*(u32*)res = *(u32*)((uxx)state->fp + state->fp->ret_offset);
		break;
	case(vp_i64):
		*(u64*)res = *(u64*)((uxx)state->fp + state->fp->ret_offset);
		break;
	case(vp_iarch):
		*(uxx*)res = *(uxx*)((uxx)state->fp + state->fp->ret_offset);
		break;
	default:
		return 1;
	}
	return ret;
}

static int dispatch(vp_state *state)
{
	register u8 *ip;
	register u32 *r32;
	register u64 *r64;
	register uxx *rxx;
	register vp_func* fn;

	#define imm() (*ip++)
	#define imm2() \
		(ip += 2, *(u16*)(ip - 2))
	#define imm4() \
		(ip += 4, *(u32*)(ip - 4))

	#define LOAD_FRAME() \
		ip = state->fp->ip; \
		r32 = VP_R32(state->fp); \
		r64 = VP_R64(state->fp); \
		rxx = VP_RXX(state->fp); \
		fn = state->fp->fn;	
	#define STORE_FRAME() \
		state->fp->ip = ip;

	#ifdef VP_CGO

	static void* dispatch_table[] = {
		#define OPCODE(name, _) &&OPC_##name,
		BCDEF(OPCODE)
		#undef OPCODE
	};

	#define next() \
		goto *dispatch_table[imm()]

	#define vm_loop \
		next();

	
	#define vm_break \
		next()

	#define vm_case(op) \
		OPC_##op

	#else

	#define vm_loop \
		loop: \
		switch(imm())
	
	#define vm_break \
		goto loop
	
	#define vm_case(op) \
		case OP_##op

	#endif

	LOAD_FRAME();

	vm_loop {
	vm_case(NOP):
		vm_break;
	vm_case(RET):
	{
		imm();
		u8 rreg = imm();
		void *ret;
		switch(fn->type.ret) {
		case(vp_i32):
			ret = r32 + rreg;
			break;
		case(vp_i64):
			ret = r64 + rreg;
			break;
		case(vp_iarch):
			ret = rxx + rreg;
			break;
		default:
			// probably vp_void
			ret = 0;
			break;
		}
		if(state->fp == state->frames) {
			// there are no frames left,
			// return to host
			// the host will use this to find the return value
			state->fp->ret_offset = 
			    (size_t)((uxx)ret - (uxx)state->fp);
			return 0;
		}
		if(state->fp->ret_offset == 0) {
			// the return value doesn't matter
			pop_callframe(state);
			vm_break;
		}
		// save return value
		pop_callframe(state);

		switch(fn->type.ret) {
		case(vp_i32):
			*(u32*)((u8*)state->fp + state->fp->ret_offset) =
			    *(u32*)ret; 
			break;
		case(vp_i64):
			*(u64*)((u8*)state->fp + state->fp->ret_offset) =
			    *(u64*)ret; 
			break;
		case(vp_iarch):
			*(uxx*)((u8*)state->fp + state->fp->ret_offset) =
			    *(uxx*)ret; 
			break;
		default:
			// do nothing
			break;
		}
		LOAD_FRAME();
		vm_break;
	}
	vm_case(OBJ):
	{
		u8 dst = imm();
		u32 id = imm4();
		uxx val = (uxx)state->objs[id];
		*(rxx + dst) = val;
		vm_break;
	}
	vm_case(LDI_W):
	{
		u8 dst = imm();
		u32 val = imm4();
		*(r32 + dst) = val;
		vm_break;
	}
	vm_case(CALL):
	{
		vp_type tdst = imm();
		u8 dst = imm();
		size_t rof = 0;
		switch(tdst) {
		case(vp_i32):
			rof = state->fp->r32 + dst;
			break;
		case(vp_i64):
			rof = state->fp->r64 + dst;
			break;
		case(vp_iarch):
			rof = state->fp->rxx + dst;
			break;
		default:
			// vp_void
			break;
		}
		u32 id = imm4();
		vp_func *f = state->nest->funcs[id];
		u8 num_args = imm();
		u8 *args = ip;
		ip += num_args * 2;
		STORE_FRAME();
		push_callframe(state, f);
		state->fp->ret_offset = rof;
		int rw = 0;
		int rl = 0;
		int rx = 0;
		for(int i = 0; i < num_args * 2; i++) {
			vp_type type = *(args + i++);
			u8 reg = *(args + i++);
			switch(type) {
			case(vp_i32):
				*(VP_R32(state->fp) + rw) = *(r32 + reg);
				rw++;
				break;
			case(vp_i64):
				*(VP_R64(state->fp) + rl) = *(r64 + reg);
				rl++;
				break;
			case(vp_iarch):
				*(VP_RXX(state->fp) + rx) = *(rxx + reg);
				rx++;
				break;
			default:
				return 1;
			}
		}
		if(f->ftype == vp_func_normal) {
			LOAD_FRAME();
		} else {
			f->native(state);
			pop_callframe(state);
		}
		vm_break;
	}
	}
	#undef vm_case
	#undef vm_break
	#undef vm_loop
	#undef next
	#undef LOAD_FRAME
	#undef imm
	#undef imm2
	#undef imm4

	return 1;
}