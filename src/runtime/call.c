/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "runtime.h"

int vi_call(struct vi_runtime *r, struct vi_func *fn, void *params,
            int num_params, void *ret)
{
	struct vi_vm *vm = vi_vm_create(r);
	return vi_vm_call(vm, fn, params, num_params, ret);
}