/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VIPER_H_
#define VIPER_H_

#define VP_VERSION "Viper 0.1.0a"
#define VP_VER_NUM 1000 /* 000.001.000 */

#include "common.h"

/* always include core & loader */
#include "vp/core.h"
#include "vp/loader.h"

/* optional modules */
#ifdef VP_USE_RUNTIME
#include "vp/runtime.h"
#endif
#ifdef VP_USE_EMITTER
#include "vp/emitter.h"
#endif
#ifdef VP_USE_ASM
#include "vp/asm.h"
#endif

#endif