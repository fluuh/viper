/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VIPER_RUNTIME_H_
#define VIPER_RUNTIME_H_

enum {
	vperr_none = 0,
	vperr_internal = -1,
	vperr_mem = -2,
	vperr_code = -3,
};

typedef u64 vp_ptr;

typedef struct vi_runtime *vp_runtime;

/* reserved file ids:
 * 0 - none (zero on read, nothing on write)
 * 1 - stdout
 * 2 - stderr
 * 3 - stdin
 */
typedef u64 vp_file;

typedef int (*vp_writer)(vp_runtime r, vp_file f, void *data, size_t size);

vp_runtime vp_runtime_create(vp_nest nest);
void vp_runtime_delete(vp_runtime r);

int vp_call(vp_runtime r, vp_func fn, u32 *params, int num_params, void *ret);

#endif