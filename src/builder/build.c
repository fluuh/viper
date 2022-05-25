/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <viper/builder.h>

int vb_build(vb_builder *builder, vn_nest **dst)
{
	int b = vb_bind(builder);
	if(b != 0) {
		return 1;
	}
}