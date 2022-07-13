/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "loader.h"

vp_nest *vp_load(const char *s, size_t len)
{
	return NULL;
}

static int read_file(FILE *f, char **s, size_t *len)
{
	fseek(f, SEEK_END);
	size_t size = ftell(f);
	rewind(f);
	if (fread(*s, 1, size, f) != size) {
		return -1;
	}
	return 0;
}

vp_nest *vp_load_file(FILE *f)
{
	char *s;
	size_t len;
	if (read_file(f, &s, &len) < 0) {
		return NULL;
	}
	return vp_load(s, len);
}
