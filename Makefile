
INC = -Iinclude

CC      = clang
LD      = $(CC)
LDFLAGS =
CWARNS  = -Wall -Wshadow -Wpointer-arith -Wcast-qual \
        -Wstrict-prototypes -Wmissing-prototypes
CFLAGS  = -m64 -std=c99 $(INC) -O2 -c -march=native -g $(CWARNS)
AR      = ar rcs
RM      = rm -f

OUT_DIR = build

CORE_NAME = libviper.a
CORE_LIB  = $(OUT_DIR)/$(CORE_NAME)

BIN_NAME = viper
BIN_LIB  = $(OUT_DIR)/$(BIN_NAME)

CORE_SRC = src/util/dbuff.c src/util/mem.c src/nest.c \
	   src/func.c src/linker.c src/api.c src/asm/asm.c src/emit/build.c \
	   src/emit/emit.c src/obj.c src/loader/writer.c src/state.c \
	   src/vm/vm.c src/loader/loader.c src/builder/build.c \
	   src/builder/emit.c src/builder/verify.c src/builder/builder.c
CORE_OBJ = $(CORE_SRC:src/%.c=$(OUT_DIR)/obj/%.o)

BIN_SRC = src/main.c
BIN_OBJ = $(BIN_SRC:src/%.c=$(OUT_DIR)/obj/%.o)

all: core bin

bin: $(BIN_LIB)

core: $(CORE_LIB)

$(BIN_LIB): $(BIN_OBJ) $(CORE_LIB)
	@echo "Linking $(subst $(OUT_DIR)/,,$@)"
	@$(LD) $(LDFLAGS) -o $@ $^
	@echo "Bin built successfully :-D"

$(BIN_OBJ): $(OUT_DIR)/obj/%.o:src/%.c
	@echo "Compiling $(subst $(OUT_DIR)/obj/,,$@)"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<

$(CORE_LIB): $(CORE_OBJ)
	@echo "Creating archive $(subst $(OUT_DIR)/,,$@)"
	@$(AR) $@ $^
	@echo "Core built successfully :3"

$(CORE_OBJ): $(OUT_DIR)/obj/%.o:src/%.c
	@echo "Compiling $(subst $(OUT_DIR)/obj/,,$@)"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) -r $(OUT_DIR)