/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_LOADER_H_
#define VP_LOADER_H_

#include "viper.h"
#include "core.h"

#include <stdio.h>

// Write a nest to file.
int vp_write_nest(FILE *file, vn_nest *nest);

int vp_load_file(FILE *file, vn_nest **nest);

#endif