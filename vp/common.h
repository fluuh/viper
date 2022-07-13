/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_COMMON_H_
#define VP_COMMON_H_

/* used by most files */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* mostly not useful */
#include "viper.h"

/* vp_type */
typedef uint8_t vp_type;
enum vp_type_enum {
	vp_void,
	vp_i32,
	vp_i64,
	vp_f32,
	vp_f64,
	vp_isize, /* pointer? */
};

/* memory management macros */
#define vmalloc(size) malloc(size)
#define vrealloc(ptr, newsize) realloc(ptr, newsize)
#define vfree(ptr) free(ptr)
#define vfree_const(ptr) free((void *)ptr)

/* definitions */
/* the initial size of a vector */
#define VP_VEC_INIT 256

#endif
