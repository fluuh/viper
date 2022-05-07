#include <viper/state.h>
#include <viper/bc.h>

static int dispatch(vp_state *state);

static int push_callframe(vp_state *state, vp_func *fn)
{
	ve_callframe *fp = state->fp + state->fp->size;
	size_t r32 = sizeof(u32) * fn->r32;
	size_t r64 = sizeof(u64) * fn->r64;
	size_t rxx = sizeof(uxx) * fn->rxx;
	size_t size = sizeof(*fp) + r32 + r64 + rxx;
	fp->size = size;
	fp->r32 = sizeof(*fp);
	fp->r64 = fp->r32 + r32;
	fp->rxx = fp->r64 + r64;
	fp->fn = fn;
	fp->ip = fn->code;
	fp->ret_offset = 0; // this is set by the caller
	fp->prev = state->fp;
	state->fp = fp;
	return 0;
}

int vp_call_func(vp_state *state, vp_export fi, void *res, 
                 u64 *args, int num_args)
{
	vp_func *fn = state->nest->funcs[fi];
	push_callframe(state, fn);
	// TODO: load args
	int ret = dispatch(state);
	// TODO: save result into res
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
		r32 = (void*)((uxx)state->fp + state->fp->r32); \
		r64 = (void*)((uxx)state->fp + state->fp->r64); \
		rxx = (void*)((uxx)state->fp + state->fp->rxx); \
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