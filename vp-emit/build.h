/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VPE_BUILD_H_
#define VPE_BUILD_H_

#include <vp/ld-def.h>
#include "vp.h"

static uint8_t elem_start = VPLD_ELEM_START;

#define EMITTER_INITIAL_CAP 256

typedef struct emitter_def {
	uint32_t len;
	uint32_t cap;
	unsigned char *c;
} emitter;

typedef struct builder_def {
	/* type section */
	uint32_t type_id;
	emitter *type;
	/* string section */
	uint32_t str_id;
	emitter *str;
	/* info section */
	emitter *info;
	/* import section */
	/*uint32_t import_id;
	emitter *import;*/
	/* export section */
	emitter *exports;
	/* objects */
	emitter *data;
	emitter *obj;
	/* functions */
	emitter *code;
	emitter *func;
} builder;

static emitter *emitter_create(void)
{
	emitter *em = vmalloc(sizeof(*em));
	em->len = 0;
	em->cap = EMITTER_INITIAL_CAP;
	em->c = vmalloc(em->cap);
	return em;
}

static void emitter_free(emitter *em)
{
	vfree(em->c);
	vfree(em);
}

static void emitter_write(emitter *em, const void *s, size_t len)
{
	if (em->cap < em->len + 1) {
		em->cap *= 2;
		em->c = vrealloc(em->c, em->cap);
		return emitter_write(em, s, len);
	}
	memcpy(&em->c[em->len], s, len);
	em->len += len;
}

static void emitter_write_str(emitter *em, const char *s)
{
	emitter_write(em, s, strlen(s));
}

static builder *builder_create(void)
{
	builder *b = vmalloc(sizeof(*b));
	/* ;( */
	b->type_id = 0;
	b->type = emitter_create();
	b->str_id = 0;
	b->str = emitter_create();
	b->info = emitter_create();
	b->exports = emitter_create();
	b->data = emitter_create();
	b->obj = emitter_create();
	b->code = emitter_create();
	b->func = emitter_create();
	return b;
}

static void builder_free(builder *b)
{
	emitter_free(b->type);
	emitter_free(b->str);
	emitter_free(b->info);
	emitter_free(b->exports);
	emitter_free(b->data);
	emitter_free(b->obj);
	emitter_free(b->code);
	emitter_free(b->func);
	vfree(b);
}

/* final emit */

static void emit_section(vpe_nest *nest, emitter *em, char section)
{
	if (em->len == 0) {
		return;
	}
	static uint8_t section_start = VPLD_SECTION_START;
	static uint8_t section_end = VPLD_SECTION_END;
#define POS &nest->nest[nest->len]
#define APPEND(d, l) (memcpy(POS, d, l), nest->len += l)
	APPEND(&section_start, 1);
	APPEND(&section, 1);
	APPEND(&em->len, 4);
	APPEND(em->c, em->len);
	APPEND(&section_end, 1);
#undef APPEND
#undef POS
}

static void builder_emit(builder *b, vpe_nest *nest)
{
	/* header */
	static const char *vpld_magic = VPLD_MAGIC;
	memcpy(nest->nest, vpld_magic, 4);
	static uint32_t vpld_version = VPLD_VERSION;
	memcpy(&nest->nest[4], &vpld_version, 4);
	nest->len = 8;
	/* sections */
	emit_section(nest, b->type, VPLD_SECTION_TYPE);
	emit_section(nest, b->str, VPLD_SECTION_STRING);
	emit_section(nest, b->info, VPLD_SECTION_INFO);
	emit_section(nest, b->exports, VPLD_SECTION_EXPORT);
	emit_section(nest, b->data, VPLD_SECTION_DATA);
	emit_section(nest, b->code, VPLD_SECTION_CODE);
	emit_section(nest, b->func, VPLD_SECTION_FUNC);
	emit_section(nest, b->obj, VPLD_SECTION_OBJ);
	/* end */
	static const uint8_t vpld_eof = VPLD_EOF;
	memcpy(&nest->nest[nest->len], &vpld_eof, 1);
	nest->len++;
}

static uint32_t insert_str(builder *b, const char *s)
{
	emitter_write(b->str, &elem_start, 1);
	uint32_t len = (uint32_t)strlen(s);
	emitter_write(b->str, &len, 4);
	emitter_write_str(b->str, s);
	return b->str_id++;
}

static void insert_info(builder *b, const char *key, const char *val)
{
	emitter_write(b->info, &elem_start, 1);
	uint32_t key_id = insert_str(b, key);
	uint32_t val_id = insert_str(b, val);
	emitter_write(b->info, &key_id, 4);
	emitter_write(b->info, &val_id, 4);
}

static uint32_t insert_sig(builder *b, vpe_signature *sig)
{
	emitter_write(b->type, &elem_start, 1);
	uint8_t n_rets = (uint8_t)sig->n_rets;
	uint8_t n_args = (uint8_t)sig->n_args;
	emitter_write(b->type, &n_rets, 1);
	emitter_write(b->type, &n_args, 1);
	emitter_write(b->type, sig->types, n_rets + n_args);
	return b->type_id++;
}

/* functions */

static void insert_insn(builder *b, vpe_insn *insn)
{
	uint8_t code = (uint8_t)insn->code;
	emitter_write(b->code, &code, 1);
	for (int i = 0; i < vpe_bc_length[code]; i++) {
		switch (vpe_bc_type[code][i]) {
		case('f'):
		case('F'):
		case('d'):
		case('D'):
		case('w'):
		case('W'):
		case('l'):
		case('L'):
		case('s'):
		case('S'):
			/* register */
			unsigned char reg = (unsigned char)insn->ops[i].reg;
			emitter_write(b->code, &reg, 1);
			break;
		default:
			/* oh no */
			break;
		}
	}
}

static void insert_block(builder *b, vpe_block *block)
{
	for (vpe_insn *insn = block->first; insn != NULL; insn = insn->next) {
		insert_insn(b, insn);
	}
}

static void insert_func(builder *b, vpe_function *func)
{
	/* insert function header */
	emitter_write(b->func, &elem_start, 1); /* elem_start */
	uint32_t ty_id = insert_sig(b, func->sig);
	emitter_write(b->func, &ty_id, 4);
	/* insert code */
	emitter_write(b->code, &elem_start, 1); /* elem_start */
	uint32_t code_size = func->code.last->pos + func->code.last->size + 1;
	emitter_write(b->code, &code_size, 4);
	emitter_write(b->code, &func->code.n_regs, 4);
	for (vpe_block *block = func->code.first; block != NULL;
	     block = block->next) {
		insert_block(b, block);
	}
	/* end */
	static uint8_t end = BC_END;
	emitter_write(b->code, &end, 1);
}

static vpe_nest *cx_build(vpe_context *cx)
{
	builder *b = builder_create();
	/* info */
	for (vpe_info *info = cx->info; info != NULL; info = info->next) {
		insert_info(b, info->key, info->val);
	}
	/* functions */
	for (vpe_function *func = cx->last_func; func != NULL;
	     func = func->prev) {
		insert_func(b, func);
	}
	/* emit */
	size_t len = 9; /* header and end */
	emitter *ems[] = {b->code, b->data, b->exports, b->func, b->info,
	                  b->obj,  b->str,  b->type,    NULL};
	/* get length */
	for (int i = 0; ems[i] != NULL; i++) {
		emitter *em = ems[i];
		if (em->len == 0) {
			continue;
		}
		/* section start and type */
		len += 2;
		/* section length */
		len += 4;
		/* section */
		len += em->len;
		/* section end */
		len += 1;
	}
	vpe_nest *nest = vmalloc(sizeof(*nest));
	nest->len = 0;
	nest->nest = vmalloc(len);
	builder_emit(b, nest);
	builder_free(b);
	return nest;
}

#endif
