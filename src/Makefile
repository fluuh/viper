
AR = ar rcs

CC = clang
CCOPT = -march=native -std=c99 -I../include -g
# -Wcast-qual 
CWARN = -Wall -Wshadow -Wpointer-arith \
	-Wstrict-prototypes -Wmissing-prototypes

OUT_DIR = ../_build

# Core
CORE_SRC = core/func.c core/nest.c core/obj.c core/bc.c
CORE_OBJ = $(CORE_SRC:%.c=$(OUT_DIR)/obj/%.o)

# Assembler
ASM_SRC = asm/asm.c asm/lexer.c asm/parser.c asm/util.c asm/token.c
ASM_OBJ = $(ASM_SRC:%.c=$(OUT_DIR)/obj/%.o)

# Runtime
RT_OUT = $(OUT_DIR)/libviper.a
RT_SRC = runtime/call.c runtime/runtime.c runtime/vm.c
RT_OBJ = $(RT_SRC:%.c=$(OUT_DIR)/obj/%.o)

# CLI
CLI_OUT = $(OUT_DIR)/viper
CLI_SRC = cli/main.c
CLI_OBJ = $(CLI_SRC:%.c=$(OUT_DIR)/obj/%.o)

default all: $(CLI_OUT)

$(CLI_OUT): $(CLI_OBJ) $(ASM_OBJ) $(RT_OUT)
	@echo "LINK    $(subst $(OUT_DIR)/,,$@)"
	@$(CC) -o $@ $^

$(RT_OUT): $(CORE_OBJ) $(RT_OBJ)
	@echo "AR      $(subst $(OUT_DIR)/,,$@)"
	@$(AR) $@ $^

$(OUT_DIR)/obj/%.o: %.c
	@echo "CC      $(subst $(OUT_DIR)/obj/,,$@)"
	@mkdir -p $(@D)
	@$(CC) $(CCOPT) $(CWARN) -c -o $@ $<