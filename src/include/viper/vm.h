/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VIPER_VM_H_
#define VIPER_VM_H_

#include "viper.h"
#include "core.h"

enum {
	vperr_none = 0,
	vperr_internal,
	vperr_halt,
	vperr_code,
	vperr_index,
	vperr_mem,
};

#include "vm/state.h"

#endif