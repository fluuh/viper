/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "runtime.h"

vp_runtime vp_runtime_create(vp_nest nest) { return vi_runtime_create(nest); }

void vp_runtime_delete(vp_runtime r) { vi_runtime_delete(r); }

struct vi_runtime *vi_runtime_create(struct vi_nest *nest)
{
	struct vi_runtime *r = vi_malloc(sizeof(*r));
	r->nest = nest;
	return r;
}

void vi_runtime_delete(struct vi_runtime *r)
{
	vi_nest_delete(r->nest);
	vi_runtime_free(r);
}
void vi_runtime_free(struct vi_runtime *r) { vi_free(r); }

int vp_call(vp_runtime r, vp_func fn, u32 *params, int num_params, void *ret)
{
	if (fn >= r->nest->num_funcs) {
		return 1;
	}
	struct vi_func *func = r->nest->funcs[fn];
	return vi_call(r, func, params, num_params, ret);
}