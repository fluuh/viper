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
vpe_insn *vpe_insn_end(vpe_function *func);
vpe_insn *vpe_insn_retvoid(vpe_function *func);
vpe_insn *vpe_insn_halt(vpe_function *func, vpe_reg op0);
vpe_insn *vpe_insn_break(vpe_function *func);
vpe_insn *vpe_insn_add_u4(vpe_function *func, vpe_reg dst, vpe_reg op0,
                          vpe_reg op1);
vpe_insn *vpe_insn_add_s4(vpe_function *func, vpe_reg dst, vpe_reg op0,
                          vpe_reg op1);
vpe_insn *vpe_insn_add_u8(vpe_function *func, vpe_reg dst, vpe_reg op0,
                          vpe_reg op1);
vpe_insn *vpe_insn_add_s8(vpe_function *func, vpe_reg dst, vpe_reg op0,
                          vpe_reg op1);
vpe_insn *vpe_insn_print(vpe_function *func, vpe_reg op0, vpe_reg op1,
                         vpe_reg op2);

#endif
