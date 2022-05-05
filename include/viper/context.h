/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_CONTEXT_H_
#define VP_CONTEXT_H_

#include "nest.h"

typedef struct vp_context {
	vn_nest *nest;
} vp_context;

#endif