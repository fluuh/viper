/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>

#include <vp/viper.h>
#include <vp-emit/emit.h>

static void print_version(void)
{
	printf("%s ver %s\n", VP_PROGNAME, VP_VERSION);
}

int main(int argc, char **argv)
{
	vpe_context *cx = vpe_context_create();
	vpe_signature *sig = vpe_sig_create(0, 0, NULL);
	vpe_function *func = vpe_function_create(cx, sig);
	vpe_insn_add_u4(func, 2, 0, 1); /* should probably initialize these :X */
	vpe_info_add(cx, "AUTHORS", "Me <me@example.org>");
	vpe_nest *nest = vpe_context_build(cx);
	fwrite(nest->nest, 1, nest->len, stdout);
	return 0;
}
