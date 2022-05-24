
INC = -Iinclude

CC = clang
CWARNS = -Wall -Wshadow -Wpointer-arith -Wcast-qual \
        -Wstrict-prototypes -Wmissing-prototypes
CFLAGS = -m64 -std=c99 $(INC) -O0 -c -march=native -g $(CWARNS)
AR     = ar rcs
RM     = rm -f

# build
OUT_DIR = build

CORE_NAME = libviper.a
CORE_LIB  = $(OUT_DIR)/$(CORE_NAME)

BIN_NAME = viper
BIN_LIB  = $(OUT_DIR)/$(BIN_NAME)

CORE_SRC = src/util/dbuff.c src/util/mem.c src/nest.c src/emit/emit.c \
           src/emit/build.c src/func.c src/linker.c src/api.c src/asm/asm.c \
	   src/obj.c src/loader/writer.c src/state.c src/vm/vm.c \
	   src/loader/loader.c src/builder/build.c src/builder/emit.c \
	   src/builder/verify.c
CORE_OBJ = $(CORE_SRC:src/%.c=$(OUT_DIR)/obj/%.o)

BIN_SRC = src/main.c
BIN_OBJ = $(BIN_SRC:src/%.c=$(OUT_DIR)/obj/%.o)

all: core bin

bin: $(BIN_LIB)

core: $(CORE_LIB)

$(BIN_LIB): $(BIN_OBJ) $(CORE_LIB)
	@$(CC) $^ -o $@
	@echo "Bin built successfully :-D"

$(BIN_OBJ): $(OUT_DIR)/obj/%.o:src/%.c
	@echo Compiling $<
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<

$(CORE_LIB): $(CORE_OBJ)
	@$(AR) $@ $^
	@echo "Core built successfully :3"

$(CORE_OBJ): $(OUT_DIR)/obj/%.o:src/%.c
	@echo Compiling $<
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) -r $(OUT_DIR)