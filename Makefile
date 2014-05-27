# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.


# The package path prefix, if you want to install to another root, set DESTDIR to that root.
PREFIX ?= /usr
# The library path excluding prefix.
LIB ?= /lib
# The resource path excluding prefix.
DATA ?= /share
# The library path including prefix.
LIBDIR ?= $(PREFIX)$(LIB)
# The resource path including prefix.
DATADIR ?= $(PREFIX)$(DATA)
# The generic documentation path including prefix.
DOCDIR ?= $(DATADIR)/doc
# The info manual documentation path including prefix.
INFODIR ?= $(DATADIR)/info
# The license base path including prefix.
LICENSEDIR ?= $(DATADIR)/licenses

# The name of the package as it should be installed.
PKGNAME ?= libgamma


# Optimisation level (and debug flags.)
ifeq ($(DEBUG),y)
OPTIMISE = -Og -g
else
OPTIMISE = -Ofast
endif

# Enabled warnings.
WARN = -Wall -Wextra -pedantic -Wdouble-promotion -Wformat=2 -Winit-self       \
       -Wmissing-include-dirs -Wtrampolines -Wfloat-equal -Wshadow             \
       -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls           \
       -Wnested-externs -Winline -Wno-variadic-macros -Wsign-conversion        \
       -Wswitch-default -Wconversion -Wsync-nand -Wunsafe-loop-optimizations   \
       -Wcast-align -Wstrict-overflow -Wdeclaration-after-statement -Wundef    \
       -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wlogical-op            \
       -Waggregate-return -Wstrict-prototypes -Wold-style-definition -Wpacked  \
       -Wvector-operation-performance -Wunsuffixed-float-constants             \
       -Wsuggest-attribute=const -Wsuggest-attribute=noreturn                  \
       -Wsuggest-attribute=pure -Wsuggest-attribute=format -Wnormalized=nfkc

# The C standard used in the code.
STD = c99

# C compiler debug flags.
DEBUG_FLAGS =
ifeq ($(DEBUG),y)
DEBUG_FLAGS += -D'DEBUG'
endif

# Options for the C compiler for the test.
TEST_FLAGS = $(OPTIMISE) $(WARN) -std=$(STD) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS)  \
             -ftree-vrp -fstrict-aliasing -fipa-pure-const -fstack-usage       \
             -fstrict-overflow -funsafe-loop-optimizations -fno-builtin

# Options for the C compiler for the library.
LIB_FLAGS = $(TEST_FLAGS) $(DEBUG_FLAGS) $(DEFINITIONS) -DLIBGAMMA_CONFIG_H

# Library linking flags for the linker.
LIBS_LD =
# Library linking flags for the C compiler.
LIBS_C =

# Object files for the library.
LIBOBJ = libgamma-facade libgamma-method libgamma-error gamma-helper edid

# Object files for the test.
TESTOBJ = test

# The version of the library.
LIB_MAJOR = 1
LIB_MINOR = 0
LIB_VERSION = $(LIB_MAJOR).$(LIB_MINOR)


# Include configurations from `./configure`.
include .config.mk



# Build rules.

.PHONY: default
default: lib test info

.PHONY: all
all: lib test doc

.PHONY: lib
lib: bin/libgamma.so.$(LIB_VERSION) bin/libgamma.so.$(LIB_MAJOR) bin/libgamma.so

.PHONY: test
test: bin/test

.PHONY: doc
doc: info pdf dvi ps


bin/libgamma.so.$(LIB_VERSION): $(foreach O,$(LIBOBJ),obj/$(O).o)
	mkdir -p $(shell dirname $@)
	$(CC) $(LIB_FLAGS) $(LIBS_LD) -shared -Wl,-soname,libgamma.so.$(LIB_MAJOR) -o $@ $^

bin/libgamma.so.$(LIB_MAJOR):
	mkdir -p $(shell dirname $@)
	ln -s libgamma.so.$(LIB_VERSION) $@

bin/libgamma.so:
	mkdir -p $(shell dirname $@)
	ln -s libgamma.so.$(LIB_VERSION) $@

obj/%.o: src/%.c src/*.h
	mkdir -p $(shell dirname $@)
	$(CC) $(LIB_FLAGS) $(LIBS_C) -fPIC -c -o $@ $<

bin/test: $(foreach O,$(TESTOBJ),obj/$(O).o) bin/libgamma.so.$(LIB_VERSION) bin/libgamma.so
	mkdir -p $(shell dirname $@)
	$(CC) $(TEST_FLAGS) $(LIBS_LD) -Lbin -lgamma -o $@ $(foreach O,$(TESTOBJ),obj/$(O).o)

obj/%.o: test/%.c
	mkdir -p $(shell dirname $@)
	$(CC) $(TEST_FLAGS) -Isrc -c -o $@ $<

.PHONY: info
info: libgamma.info
%.info: info/%.texinfo info/fdl.texinfo
	makeinfo $<

.PHONY: pdf
pdf: libgamma.pdf
%.pdf: info/%.texinfo info/fdl.texinfo
	mkdir -p obj
	cd obj ; yes X | texi2pdf ../$<
	mv obj/$@ $@

.PHONY: dvi
dvi: libgamma.dvi
%.dvi: info/%.texinfo info/fdl.texinfo
	mkdir -p obj
	cd obj ; yes X | $(TEXI2DVI) ../$<
	mv obj/$@ $@

.PHONY: ps
ps: libgamma.ps
%.ps: info/%.texinfo info/fdl.texinfo
	mkdir -p obj
	cd obj ; yes X | texi2pdf --ps ../$<
	mv obj/$@ $@


# Clean rules.

.PHONY: clean
clean:
	-rm -rf obj bin libgamma.{info,pdf,ps,dvi}

.PHONY: distclean
distclean: clean
	-rm -f .config.mk src/libgamma-config.h

