/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VIPER_RUNTIME_H_
#define VIPER_RUNTIME_H_

typedef struct vi_runtime *vp_runtime;

typedef u64 vp_file;

typedef int(*vp_writer)(vp_runtime r, vp_file f, void* data, size_t size);

vp_runtime vp_runtime_create(vp_nest nest);
void vp_runtime_delete(vp_runtime r);

int vp_call(vp_runtime r, vp_func fn, u32 *params, int num_params, void *ret);

#endif