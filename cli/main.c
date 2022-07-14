/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>

#include <vp/viper.h>
#include <vp/core.h>
#include <vp/rt/vm/vm.h>
#include <vp-emit/emit.h>

static void print_version(void)
{
	printf("%s ver %s\n", VP_PROGNAME, VP_VERSION);
}

static void run(void)
{
	vp_type types[] = { vp_i32, vp_i32 };
	vp_sig *sig = vp_sig_create(0, 2, types);
	uint8_t prog[] = {
		BC_ADD_U4, 0, 0, 1,
		BC_RETVOID,
		BC_END,
	};
	vp_code *code = vp_code_create(2, prog, sizeof(prog));
	vp_func *fn = vp_func_create(sig, code);
	vp_nest *nest = vp_nest_create();
	vp_func_insert(nest, fn);
	vp_state *s = vp_state_create(nest);
	vp_vm *vm = vp_vm_create(s, 0);
	vm_reg args[] = {
		14, 16
	};
	vp_vm_call(vm, fn, args, 2); 
	vp_vm_destroy(vm);
	vp_nest_free(nest);
	vp_func_free(fn);
}

int main(int argc, char **argv)
{
	vpe_context *cx = vpe_context_create();
	vpe_signature *sig = vpe_sig_create(0, 0, NULL);
	vpe_function *func = vpe_function_create(cx, sig);
	vpe_insn_add_u4(func, 2, 0, 1); /* should probably initialize these :X */
	vpe_info_add(cx, "AUTHORS", "Me <me@example.org>");
	vpe_nest *nest = vpe_context_build(cx);
	// fwrite(nest->nest, 1, nest->len, stdout);
	vpe_nest_free(nest);
	run();
	return 0;
}
