/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/* tf is this */
#ifndef VP_EMIT_EMIT_H_
#define VP_EMIT_EMIT_H_

#include "vp.h"

/* emit instructions */
/* it might be better to auto-generate these */

vpe_insn *vpe_insn_nop(vpe_function *func);
vpe_insn *vpe_insn_retvoid(vpe_function *func);
vpe_insn *vpe_insn_halt(vpe_function *func);
vpe_insn *vpe_insn_break(vpe_function *func);

#endif
