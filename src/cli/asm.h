/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VIPER_CLI_ASM_H_
#define VIPER_CLI_ASM_H_

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

struct asm_loader {
	size_t cap;
	size_t len;
	char *buff;
	int num_included;
	char *included[256];
};

static int asm_read_file_i(FILE *file, const char *name, struct asm_loader *ld);

/* get the directory in a file path */
static char *fdir(const char *path, char *buff, size_t len)
{
	int num_slashes = 0;
	int last_slash = 0;
	for (int i = 0; i < strlen(path); i++) {
		if (path[i] == '/') {
			num_slashes++;
			last_slash = i;
		}
	}
	if (num_slashes == 0) {
		buff[0] = '.';
		buff[1] = 0;
	} else {
		if (len < last_slash) {
			return NULL;
		}
		memcpy(buff, path, last_slash);
		buff[last_slash] = 0; /* there's no reason to keep the slash */
	}
	return buff;
}

static int asm_read_directive(FILE *file, const char *name,
                              struct asm_loader *ld)
{
	int len = 0;
	char str[256];
	char c;
	while (isalpha(c = fgetc(file))) {
		str[len++] = c;
	}
	str[len] = 0;
	if (strcmp(str, "include") == 0) {
		if (fgetc(file) != '"') {
			return -1;
		}
		len = 0;
		while ((c = fgetc(file)) != '"' && c != EOF) {
			str[len++] = c;
		}
		str[len] = 0;
		FILE *sf = fopen(str, "r");
		if (sf == NULL) {
			printf("failed to open file \"%s\" in \"%s\"\n", str,
			       name);
			if (getcwd(str, 256) == NULL) {
				return -1;
			}
			printf("cwd: %s\n", str);
			return -1;
		}
		return asm_read_file_i(sf, str, ld);
	}
	return -1; /* invalid directive */
}

static int asm_read_file_i(FILE *file, const char *name, struct asm_loader *ld)
{
	char dir[256];
	if (fdir(name, dir, 256) == NULL) {
		return 1;
	}
	chdir(dir);
	char col = 0;
	char c = fgetc(file);
	while (c != EOF) {
		col++;
		if (c == '\n') {
			col = 0;
		} else if (c == '#' && col == 1) {
			int status = asm_read_directive(file, name, ld);
			if (status != 0) {
				/* the caller will change cwd */
				return status;
			}
			chdir(dir);
			goto next;
		}
		if (ld->cap <= ld->len) {
			ld->cap *= 2;
			ld->buff = realloc(ld->buff, ld->cap);
		}
		ld->buff[ld->len++] = (char)c;
	next:
		c = fgetc(file);
	}
	return 0;
}

static int asm_read_file(const char *fpath, char **dst)
{
	char cwd[256];
	if (getcwd(cwd, 256) == NULL) {
		return 1;
	}
	struct asm_loader *ld = vi_malloc(sizeof(*ld));
	ld->cap = 256;
	ld->len = 0;
	ld->buff = vi_malloc(256);
	/* open file */
	FILE *file = fopen(fpath, "r");
	if (file == NULL) {
		return 1;
	}
	/* read */
	int status = asm_read_file_i(file, fpath, ld);

	fclose(file);
	chdir(cwd);

	ld->buff[ld->len] = '\0';
	*dst = ld->buff;
	vi_free(ld);
	return status;
}

#endif