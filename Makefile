VERSION = 0.1.0a

INSTALL_ROOT = /usr/local
INSTALL_BIN = $(INSTALL_ROOT)/bin
INSTALL_LIB = $(INSTALL_ROOT)/lib
INSTALL_INC = $(INSTALL_ROOT)/include
OUT_DIR = _build
default all:
	@echo "== Building Viper $(VERSION) =="
	@$(MAKE) -C src
	@echo "== Successfully built Viper $(VERSION) =="

clean:
	rm -rf $(OUT_DIR)

fmt:
	clang-format -i -style=file src/*/*.c src/*/*/*.c src/*/*.h src/*/*/*.h include/*/*.h include/*/*/*.h