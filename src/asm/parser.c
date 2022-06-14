/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "asm.h"
#include "lexer.h"
#include "parser.h"

static struct asm_unit *asm_unit_create(void)
{
 struct asm_unit *unit = vi_malloc(sizeof(*unit));
 unit->num_funcs = 0;
 unit->num_objs = 0;
 return unit;
}

void vi_asm_unit_free(struct asm_unit *unit)
{
 /*
 for(int i = 0; i < unit->num_funcs; i++) {
  asm_func_free(unit->funcs[i]);
 }
 for(int i = 0; i < unit->num_objs; i++) {
  asm_obj_free(unit->objs[i]);
 }
 */
 vi_free(unit);
}
