/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VIPER_H_
#define VIPER_H_

/* Build Settings */
#define VP_DEBUG
#define VP_TRACE
// #define VP_CGO

#define VP_VER_GEN 0
#define VP_VER_MIN 1
#define VP_VER_REV 0
#define VP_VERSION "Viper 0.1.0"
#define VP_VER_NUM 100 /* 00.01.00 */

#define VP_VER_SYM vp_ver_0_1_0

void VP_VER_SYM(void);

#include "asm.h"
#include "builder.h"
#include "emit.h"
#include "func.h"
#include "linker.h"
#include "loader.h"
#include "nest.h"
#include "obj.h"
#include "state.h"
#include "util.h"

#endif