# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.


# Include configurations from `./configure`.
include config.mk


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

# Options for the C compiler.
C_FLAGS = $(OPTIMISE) $(WARN) -std=$(STD) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS)  \
          -ftree-vrp -fstrict-aliasing -fipa-pure-const -fstack-usage       \
          -fstrict-overflow -funsafe-loop-optimizations -fno-builtin        \
	  $(DEBUG_FLAGS) $(DEFINITIONS)


# Object files for the library.
LIBOBJ = libgamma-facade


# Build rules.

.PHONY: all
all: bin/libgamma.so


bin/libgamma.so: $(foreach O,$(LIBOBJ),obj/$(O).o)
	mkdir -p $(shell dirname $@)
	$(CC) $(C_FLAGS) -shared -o $@ $^

obj/%.o: src/%.c src/*.h
	mkdir -p $(shell dirname $@)
	$(CC) $(C_FLAGS) -fPIC -c -o $@ $<


# Clean rules.

.PHONY: clean
clean:
	-rm -rf obj bin

