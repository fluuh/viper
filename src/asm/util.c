/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "util.h"

int vi_strcmpn(const char *s1, const char *s2, int n)
{
	while (n > 0 && *s1 == *s2) {
		if (*s1 == '\0')
			return 0;
		s1++;
		s2++;
		n--;
	}
	if (n == 0)
		return 1;
	return 0;
}
