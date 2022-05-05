/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_ASM_H_
#define VP_ASM_H_

#include <stdio.h>

#include "nest.h"

vn_nest *vn_assemble_file(FILE *file);
vn_nest *vn_assemble(const char* src);

#endif