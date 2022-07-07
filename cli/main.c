/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>

#include <vp-emit/emit.h>

int main(int argc, char **argv)
{
	vpe_context *cx = vpe_context_create();
	vpe_insn_nop(vpe_function_create(cx));
	vpe_context_build(cx);
	return 0;
}
