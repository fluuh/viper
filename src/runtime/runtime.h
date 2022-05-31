/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_RUNTIME_H_
#define VP_RUNTIME_H_

#include <viper/runtime.h>
#include "../core/core.h"
#include "vm.h"

struct vi_runtime {
	struct vi_nest *nest;
};

struct vi_runtime *vi_runtime_create(struct vi_nest *nest);
void               vi_runtime_delete(struct vi_runtime *r);
void               vi_runtime_free(struct vi_runtime *r);

int                vi_call(struct vi_runtime *r,
                           struct vi_func    *fn,
                           void              *params,
                           int                num_params,
                           void              *ret);

#endif