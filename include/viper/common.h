/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VIPER_COMMON_H_
#define VIPER_COMMON_H_

#include <stdint.h>
#include <stddef.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

enum {
	vp_void = 0,
	vp_i32 = 4,
	vp_i64 = 8,
};
typedef i8 vp_type;

// uhm
#include <stdlib.h>

#define vi_malloc(size) malloc(size)
#define vi_free(ptr) free(ptr)
#define vi_free_const(ptr) free((void*)ptr)

#endif