/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <alloca.h>
#include <stdio.h>
#include <viper/asm.h>
#include <viper/viper.h>

#include "asm.h"

static void print_version() { fputs(VP_VERSION "\n", stdout); }

int main(int argc, char *argv[])
{
	char *src = NULL;
	if (asm_read_file(argv[1], &src) != 0) {
		return 1;
	}
	printf("%s\n", src);
	vi_free(src);
	return 0; /* dead */
	// static u8 fcode[] = {
	//     6,                       /* obj */
	//     0,                       /* %0 */
	//     0,  0, 0, 0,             /* i32 0 */
	//     7,                       /* ldi.i64 */
	//     2,                       /* %2 */
	//     0,  0, 0, 0, 0, 0, 0, 0, /* i64 0 */
	//     5,                       /* ldi.i32 */
	//     4,                       /* %4 */
	//     15, 0, 0, 0,             /* i32 15 */
	//     3,                       /* write */
	//     0,                       /* %0 */
	//     2,                       /* %2 */
	//     4,                       /* %4 */
	//     5,                       /* ldi.i32 */
	//     0,                       /* %0 */
	//     0,  0, 0, 0,             /* i32 0 */
	//     3,                       /* halt */
	//     0,                       /* %0 */
	//     2,                       /* retvoid */
	//     1,                       /* end */
	// };
	// struct vi_nest *nest = vi_nest_create(1, 1);
	// struct vi_obj *obj = vi_obj_create(15, (u8 *)"Hello, World!\n");
	// vi_obj_insert(nest, obj, 0);
	// struct vi_code *code = vi_code_create(5, 38, fcode);
	// vp_funcType *ty = alloca(sizeof(*ty));
	// ty->ret = vp_void;
	// ty->num_args = 0;
	// struct vi_func *fn = vi_func_create(ty, code);
	// vi_func_insert(nest, fn, 0);
	// /* run */
	// vp_runtime r = vp_runtime_create(nest);
	// vp_call(r, 0, NULL, 0, NULL);
	// print_version();
	// vp_runtime_delete(r);
	return 0;
}