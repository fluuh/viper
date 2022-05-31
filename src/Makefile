
AR = ar rcs

CC = clang
<<<<<<< HEAD
CCOPT = -march=native -std=c99 -Iinclude -g
=======
CCOPT = -march=native -std=c99 -I../include -g
>>>>>>> bb5c964 (fuck it, full rewrite)
# -Wcast-qual 
CWARN = -Wall -Wshadow -Wpointer-arith \
	-Wstrict-prototypes -Wmissing-prototypes

<<<<<<< HEAD
OUT_DIR = ../build

# ASM: Builder Core
ASM_SRC = asm/asm.c
ASM_OBJ = $(ASM_SRC:%.c=$(OUT_DIR)/obj/%.o)

# Builder: Core
BLD_SRC = builder/bind.c builder/build.c builder/builder.c builder/emit.c \
	builder/verify.c
BLD_OBJ = $(BLD_SRC:%.c=$(OUT_DIR)/obj/%.o)

# CLI: VM Loader Core
CLI_OUT = $(OUT_DIR)/viper

CLI_SRC = cli/main.c
CLI_OBJ = $(CLI_SRC:%.c=$(OUT_DIR)/obj/%.o)

# Core & Util
UTIL_SRC = core/util/dbuff.c
CORE_SRC = core/api.c core/func.c core/linker.c core/nest.c core/obj.c \
	$(UTIL_SRC)
CORE_OBJ = $(CORE_SRC:%.c=$(OUT_DIR)/obj/%.o)

# VM: Loader Core
VM_OUT = $(OUT_DIR)/libvipervm.a
VM_SRC = vm/state.c vm/vm.c
VM_OBJ = $(VM_SRC:%.c=$(OUT_DIR)/obj/%.o)

# Loader: Core
LD_SRC = loader/loader.c loader/writer.c
LD_OBJ = $(LD_SRC:%.c=$(OUT_DIR)/obj/%.o)

default all: $(CLI_OUT)

$(CLI_OUT): $(CLI_OBJ) $(VM_OUT) $(ASM_OBJ) $(BLD_OBJ)
	@echo "LINK    $(subst $(OUT_DIR)/,,$@)"
	@$(CC) -o $@ $^

$(VM_OUT): $(CORE_OBJ) $(LD_OBJ) $(VM_OBJ)
=======
OUT_DIR = ../_build

# Core
CORE_SRC = core/func.c core/nest.c core/obj.c
CORE_OBJ = $(CORE_SRC:%.c=$(OUT_DIR)/obj/%.o)

# Runtime
RT_OUT = $(OUT_DIR)/libviper.a
RT_SRC = runtime/call.c runtime/runtime.c runtime/vm.c
RT_OBJ = $(RT_SRC:%.c=$(OUT_DIR)/obj/%.o)

# CLI
CLI_OUT = $(OUT_DIR)/viper
CLI_SRC = cli/main.c
CLI_OBJ = $(CLI_SRC:%.c=$(OUT_DIR)/obj/%.o)

default all: $(CLI_OUT)

$(CLI_OUT): $(CLI_OBJ) $(RT_OUT)
	@echo "LINK    $(subst $(OUT_DIR)/,,$@)"
	@$(CC) -o $@ $^

$(RT_OUT): $(CORE_OBJ) $(RT_OBJ)
>>>>>>> bb5c964 (fuck it, full rewrite)
	@echo "AR      $(subst $(OUT_DIR)/,,$@)"
	@$(AR) $@ $^

$(OUT_DIR)/obj/%.o: %.c
	@echo "CC      $(subst $(OUT_DIR)/obj/,,$@)"
	@mkdir -p $(@D)
	@$(CC) $(CCOPT) $(CWARN) -c -o $@ $<