/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef VP_LD_DEF_H_
#define VP_LD_DEF_H_

/* definitions for the loader, etc. */

#define VPLD_MAGIC                                                             \
	"\x7f"                                                                 \
	"VP"                                                                   \
	"\x01"
#define VPLD_VERSION 0
#define VPLD_ELEM_START 0x0A
#define VPLD_SECTION_START 0x02
#define VPLD_SECTION_END 0x03
#define VPLD_EOF 0x04

/* user-defined, ignored */
#define VPLD_SECTION_CUSTOM 0
/* type section */
#define VPLD_SECTION_TYPE 1
/* strings for names, etc. */
#define VPLD_SECTION_STRING 2
/* info about the module, such as:
 * - author
 * - version
 * - license */
#define VPLD_SECTION_INFO 3
/* imports */
#define VPLD_SECTION_IMPORT 4
/* exports */
#define VPLD_SECTION_EXPORT 5
/* object data */
#define VPLD_SECTION_DATA 6
/* code */
#define VPLD_SECTION_CODE 7
/* functions */
#define VPLD_SECTION_FUNC 8
/* objects */
#define VPLD_SECTION_OBJ 9

#endif
