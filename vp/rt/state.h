/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_RT_STATE_H_
#define VP_RT_STATE_H_

#include "../core.h"
#include "mem.h"

typedef struct vp_state_def {
	vp_mem *mem;
	int n_objs;
	vp_ptr *obj;
} vp_state;

/* nest is only used for object lookup */
vp_state *vp_state_create(vp_nest *nest);
void vp_state_free(vp_state *state);

#endif
