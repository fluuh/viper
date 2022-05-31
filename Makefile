VERSION = 0.1.0a

INSTALL_ROOT = /usr/local
INSTALL_BIN = $(INSTALL_ROOT)/bin
INSTALL_LIB = $(INSTALL_ROOT)/lib
INSTALL_INC = $(INSTALL_ROOT)/include
OUT_DIR = build
default all:
	@echo "== Building Viper $(VERSION) =="
	@$(MAKE) -C src
	@echo "== Successfully built Viper $(VERSION) =="

clean:
	rm -rf build/

fmt:
	clang-format -i -style=file src/*/*.c src/*/*/*.c src/*/*.h src/*/*/*.h