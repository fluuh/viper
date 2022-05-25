/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_COMMON_H_
#define VP_COMMON_H_

/* Definitions */

#define VP_BUFF_DEFAULT 256

enum {
	vperr_none = 0,
	vperr_internal,
	vperr_halt,
	vperr_code,
	vperr_index,
	vperr_mem,
};

enum {
	// floats aren't tracked
	vp_void = 0,
	vp_i32 = -1, // word, size_t
	vp_i64 = -2, // long, ptr
	/* TODO: remove iarch */
	vp_iarch = -3,
};
/* Types */

// has to be here for vp_native_func
typedef struct vp_state vp_state;

typedef int (*vp_native_func)(vp_state *state);

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned long long uxx;

typedef signed long long i64;
typedef signed int i32;
typedef signed short i16;
typedef signed char i8;
typedef signed long long ixx;

typedef i8 vp_type;

typedef struct vp_func_type {
	vp_type ret;
	u8 num_args;
	vp_type args[];
} vp_func_type;

typedef i64 vp_export;
#define VP_EXPORT_NONE -1

#endif