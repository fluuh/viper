#include "vp.h"

/* we can't depend on the definitions in viper */

#define OPCODE(ENUM, NAME, TYPE) sizeof(TYPE) ,
static const unsigned char bc_length[] = {
	BCDEF(OPCODE)
};
#undef OPCODE

#define OPCODE(ENUM, NAME, TYPE) TYPE ,
static const char *const bc_type[] = {
	BCDEF(OPCODE)
};
#undef OPCODE

static vpe_insn *create_insn(vp_bc code)
{
	vpe_insn *insn = vmalloc(sizeof(*insn) +
		                 sizeof(*insn->ops) * bc_length[code]);
	insn->code = code;
	insn->n_ops = bc_length[code];
	return insn;
}


