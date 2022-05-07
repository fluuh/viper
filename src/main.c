/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>

#include <viper/viper.h>
#include <viper/linker.h>
#include <viper/emit.h>
#include <viper/asm.h>
#include <viper/loader.h>
#include <viper/state.h>

static void print_copyright(void)
{
	fputs("Viper " VP_VERSION "\n", stdout);
}

static void print_usage(const char* progname) 
{
	fputs("Usage: \n", stderr);
	fputs("  ", stderr);
	fputs(progname, stderr);
	fputs(" [options] <file> [args...]\n", stderr);
	fputs("Options:\n"
	"  -s            Run assembler\n"
	"  -d            Run disassembler\n"
	"  -l            Run linker\n"
	"  -f <function> Start function      default: _start\n"
	"  -o <file>     Write output to <file>\n"
	"  -h            Display this information\n"
	"  -v            Show version information\n", stderr);
	fflush(stderr);
}

static struct args {
	int mode;
	int flags;
	char *out;
	char *start;
	char **args;
} args;

#define MODE_RUN 0
#define MODE_ASM 1
#define MODE_DASM 2
#define MODE_LINK 3

#define FLAG_VERSION 2
#define FLAG_HELP 4

static int parse_args(char **argv)
{
	args.mode = MODE_RUN;
	args.flags = 0;
	args.out = "a.out";
	args.start = "_start";
	int i;
	for(i = 1; argv[i] != (void*)0 && argv[i][0] == '-'; i++) {
		switch(argv[i][1]) {
		case('s'):
			args.mode = MODE_ASM;
			break;
		case('d'):
			args.mode = MODE_DASM;
			break;
		case('l'):
			args.mode = MODE_LINK;
			break;
		case('o'):
			i++;
			if(argv[i] == (void*)0)
				return -1;
			args.out = argv[i];
			break;
		case('f'):
			i++;
			if(argv[i] == (void*)0)
				return -1;
			args.start = argv[i];
			break;
		case('h'):
			args.flags |= FLAG_HELP;
			return 0;
		case('v'):
			args.flags |= FLAG_VERSION;
			return 0;
		default:
			return -1;
		}
	}
	args.args = &argv[i];
	return 0;
}

static struct status {
	char **argv;
	int argc;
	int status;
} status;

static void print_error(const char* msg) {
	fputs("\x1b[31;1merror:\x1b[0m ", stderr);
	fputs(msg, stderr);
	fputs("\n", stderr);
	fflush(stderr);
}

static int std_print(vp_state *state)
{
	printf("%s\n", (char*)*VP_RXX(state->fp));
	return 0;
}

static vn_nest *create_stdnest(void)
{
	vn_builder *builder = vn_build_create();
	vp_type params[] = {vp_iarch};
	vn_bfunc *fn = vn_bfunc_create_native(builder, 
	                                      vp_void, params, 1,
					      &std_print);
	fn->name = "print";
	return vn_build(builder);
}

static int pmain(void)
{
	struct status *s = &status;
	if(status.argc < 2) {
		print_error("no input");
		return 1;
	}
	int argn = parse_args(s->argv);
	if(argn < 0) {
		print_error("invalid options");
		return 1;
	}
	if((args.flags & FLAG_VERSION)) {
		print_copyright();
		return 0;
	}
	if((args.flags & FLAG_HELP)) {
		print_usage(status.argv[0]);
		return 0;
	}

	if(args.mode == MODE_ASM) {
		FILE* file = fopen(args.args[0], "r");
		vn_nest *nest = vn_assemble_file(file);
		if(nest == (void*)0) {
			print_error("assembler failed");
			return 1;
		}
		FILE* out = fopen(args.out, "w");
		if(vp_write_nest(out, nest) != 0) {
			print_error("writer failed");
			return 1;
		}
		vn_nest_free(nest);
		return 0;
	}
	FILE* file = fopen(args.args[0], "r");
	vn_nest *new;
	int nres = vp_load_file(file, &new);
	if(nres != 0) {
		print_error("loader failed");
		return 1;
	}
	vn_nest *stdnest = create_stdnest();
	vn_linker *lk = vn_linker_create();
	if(vn_linker_add(lk, new) != 0 ||
	   vn_linker_add(lk, stdnest) != 0) {
		print_error("linker failed");
		return 1;
	}
	vn_nest *nest = vn_linker_link(lk);
	if(nest == (void*)0) {
		print_error("linker failed");
		return 1;
	}
	vp_state *state = vp_state_init(nest);
	vp_export start = vn_get_export(state->nest, args.start);
	if(start == -1) {
		print_error("start function not found");
		return 1;
	}
	i32 res = 0;
	vp_call_func(state, start, &res, (void*)0, 0);
	printf("program exited with code %i\n", res);
	vp_state_free(state);
	vn_nest_free(nest);
	return 0;
}

int main(int argc, char **argv)
{
	VP_VER_SYM();
	if(!argv[0]) {
		argc++;
		argv[0] = "viper";
	}
	status.argc = argc;
	status.argv = argv;
	int p = pmain();
	return (p > 0) ? EXIT_FAILURE : EXIT_SUCCESS;
}