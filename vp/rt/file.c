/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "../common.h"
#include "file.h"

vp_writer *vp_writer_create(vp_fwrite_t write, vp_fread_t read, vp_fopen_t open)
{
	vp_writer *writer = vmalloc(sizeof(*writer));
	writer->write = write;
	writer->read = read;
	writer->open = open;
	return writer;
}

void vp_writer_free(vp_writer *writer)
{
	vfree(writer);
}
