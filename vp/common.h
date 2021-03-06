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

typedef int32_t vp_s32_t;
typedef int64_t vp_s64_t;
typedef uint32_t vp_u32_t;
typedef uint64_t vp_u64_t;
typedef float vp_f32_t;
typedef double vp_f64_t;
/* vp_isize has to be the pointer size, not size_t */
typedef intptr_t vp_ssize_t;
typedef uintptr_t vp_usize_t;

/* memory management macros */
#define vmalloc(size) malloc(size)
#define vrealloc(ptr, newsize) realloc(ptr, newsize)
#define vfree(ptr) free(ptr)
#define vfree_const(ptr) free((void *)ptr)

/* definitions */
/* the initial size of a vector */
#define VP_VEC_INIT 256

#endif
