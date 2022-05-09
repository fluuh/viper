/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <viper/builder.h>
#include <viper/util.h>

vb_builder *vb_builder_create(void)
{
	vb_builder *builder = vu_malloc(sizeof(*builder));
	builder->cap_funcs = VP_BUFF_DEFAULT;
	builder->num_funcs = 0;
	builder->funcs = vu_malloc_array(VP_BUFF_DEFAULT,
	                                 sizeof(*builder->funcs));
	return builder;
}