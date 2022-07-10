/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_LOADER_H_
#define VP_LOADER_H_

#include <stdio.h>
#include "core.h"

vp_nest *vp_load(const char *s, size_t len);
vp_nest *vp_load_file(FILE *f);

#endif
