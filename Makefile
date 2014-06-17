# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.


# The package path prefix, if you want to install to another root, set DESTDIR to that root.
PREFIX ?= /usr
# The library path excluding prefix.
LIB ?= /lib
# The library header path excluding prefix.
INCLUDE ?= /include
# The resource path excluding prefix.
DATA ?= /share
# The library path including prefix.
LIBDIR ?= $(PREFIX)$(LIB)
# The library header including prefix.
INCLUDEDIR ?= $(PREFIX)$(INCLUDE)
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

# General-preprocess command. (https://github.com/maandree/gpp)
GPP ?= gpp

# C compiler, GNU C Compiler by default
CC ?= gcc


# Enabled warnings.
WARN = -Wall -Wextra -pedantic -Wformat=2 -Winit-self -Wmissing-include-dirs   \
       -Wfloat-equal -Wshadow -Wmissing-prototypes -Wmissing-declarations      \
       -Wredundant-decls -Wnested-externs -Winline -Wno-variadic-macros        \
       -Wswitch-default -Wconversion -Wcast-align -Wstrict-overflow            \
       -Wdeclaration-after-statement -Wundef -Wcast-qual -Wbad-function-cast   \
       -Wwrite-strings -Waggregate-return -Wpacked -Wstrict-prototypes         \
       -Wold-style-definition

ifeq ($(CC),gcc)
WARN += -Wdouble-promotion -Wtrampolines -Wsign-conversion -Wsync-nand  \
        -Wlogical-op -Wvector-operation-performance                     \
        -Wunsuffixed-float-constants -Wsuggest-attribute=const          \
        -Wsuggest-attribute=noreturn -Wsuggest-attribute=pure           \
        -Wsuggest-attribute=format -Wnormalized=nfkc                    \
        -Wunsafe-loop-optimizations
endif


# The C standard used in the code.
STD = c99

# Library linking flags for the linker.
LIBS_LD =
# Library linking flags for the C compiler.
LIBS_C =

# Object files for the library.
LIBOBJ = libgamma-facade libgamma-method libgamma-error gamma-helper edid

# Header files for the library are parsed for the info manual.
HEADERS_INFO = libgamma-error libgamma-facade libgamma-method

# Header files for the library, as installed.
HEADERS = libgamma libgamma-config $(HEADERS_INFO)

# Object files for the test.
TESTOBJ = test methods errors

# The version of the library.
LIB_MAJOR = 0
LIB_MINOR = 1
LIB_VERSION = $(LIB_MAJOR).$(LIB_MINOR)

# Include configurations from `./configure`.
include .config.mk

# Optimisation level (and debug flags.)
ifeq ($(DEBUG),y)
ifeq ($(CC),gcc)
OPTIMISE = -Og -g
else
OPTIMISE = -g
endif
else
ifeq ($(CC),gcc)
OPTIMISE = -Ofast
else
OPTIMISE = -O
endif
endif

# C compiler debug flags.
DEBUG_FLAGS =
ifeq ($(DEBUG),y)
DEBUG_FLAGS += -D'DEBUG'
endif

# Options for the C compiler for the test.
TEST_FLAGS = $(OPTIMISE) $(WARN) -std=$(STD) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS)  \
             -fstrict-aliasing -fstrict-overflow -fno-builtin

ifeq ($(CC),gcc)
TEST_FLAGS += -fstack-usage -ftree-vrp -fipa-pure-const -funsafe-loop-optimizations
endif


# Options for the C compiler for the library.
LIB_FLAGS = $(TEST_FLAGS) $(DEBUG_FLAGS) $(DEFINITIONS) -DLIBGAMMA_CONFIG_H


ifeq ($(CC),gcc)
TEST_FLAGS += -D__GCC__
LIB_FLAGS += -D__GCC__
endif


# Build rules.

.PHONY: default
default: lib test info

.PHONY: all
all: lib test doc


.PHONY: lib
lib: bin/libgamma.so.$(LIB_VERSION) bin/libgamma.so.$(LIB_MAJOR) bin/libgamma.so

bin/libgamma.so.$(LIB_VERSION): $(foreach O,$(LIBOBJ),obj/lib/$(O).o)
	mkdir -p $(shell dirname $@)
	$(CC) $(LIB_FLAGS) $(LIBS_LD) -shared -Wl,-soname,libgamma.so.$(LIB_MAJOR) -o $@ $^

bin/libgamma.so.$(LIB_MAJOR):
	mkdir -p $(shell dirname $@)
	ln -sf libgamma.so.$(LIB_VERSION) $@

bin/libgamma.so:
	mkdir -p $(shell dirname $@)
	ln -sf libgamma.so.$(LIB_VERSION) $@

obj/lib/%.o: src/lib/%.c src/lib/*.h
	mkdir -p $(shell dirname $@)
	$(CC) $(LIB_FLAGS) $(LIBS_C) -fPIC -c -o $@ $<

obj/lib/%.o: obj/lib/%.c src/lib/*.h
	$(CC) $(LIB_FLAGS) $(LIBS_C) -fPIC -iquote"$$(dirname "$<" | sed -e 's:^obj:src:')" -c -o $@ $<

obj/%: src/%.gpp src/extract/libgamma-*-extract
	mkdir -p $(shell dirname $@)
	$(GPP) --symbol '$$' --input $< --output $@


.PHONY: test
test: bin/test

bin/test: $(foreach O,$(TESTOBJ),obj/test/$(O).o) bin/libgamma.so.$(LIB_VERSION) bin/libgamma.so
	mkdir -p $(shell dirname $@)
	$(CC) $(TEST_FLAGS) $(LIBS_LD) -Lbin -lgamma -o $@ $(foreach O,$(TESTOBJ),obj/test/$(O).o)

obj/test/%.o: src/test/%.c src/test/*.h src/lib/libgamma*.h
	mkdir -p $(shell dirname $@)
	$(CC) $(TEST_FLAGS) -Isrc/lib -c -o $@ $<


.PHONY: doc
doc: info pdf dvi ps

obj/libgamma.texinfo: info/libgamma.texinfo $(foreach H,$(HEADERS_INFO),src/lib/$(H).h) \
	              src/extract/libgamma-*-extract info/texise info/behead
	mkdir -p obj
	$(GPP) --symbol '%' --input $< --output $@

obj/%.texinfo: info/%.texinfo
	mkdir -p obj
	cp $< $@

.PHONY: info
info: libgamma.info
%.info: obj/%.texinfo obj/fdl.texinfo
	makeinfo $<

.PHONY: pdf
pdf: libgamma.pdf
%.pdf: obj/%.texinfo obj/fdl.texinfo
	cd obj ; yes X | texi2pdf ../$<
	mv obj/$@ $@

.PHONY: dvi
dvi: libgamma.dvi
%.dvi: obj/%.texinfo obj/fdl.texinfo
	cd obj ; yes X | $(TEXI2DVI) ../$<
	mv obj/$@ $@

.PHONY: ps
ps: libgamma.ps
%.ps: obj/%.texinfo obj/fdl.texinfo
	cd obj ; yes X | texi2pdf --ps ../$<
	mv obj/$@ $@


# Install rules.

.PHONY: install
install: install-base install-info

.PHONY: install
install-all: install-base install-doc

.PHONY: install-base
install-base: install-lib install-include install-copyright


.PHONY: install-lib
install-lib: bin/libgamma.so.$(LIB_VERSION)
	install -dm755 -- "$(DESTDIR)$(LIBDIR)"
	install -m755 $< -- "$(DESTDIR)$(LIBDIR)/libgamma.so.$(LIB_VERSION)"
	ln -sf libgamma.so.$(LIB_VERSION) -- "$(DESTDIR)$(LIBDIR)/libgamma.so.$(LIB_MAJOR)"
	ln -sf libgamma.so.$(LIB_VERSION) -- "$(DESTDIR)$(LIBDIR)/libgamma.so"

.PHONY: install-include
install-include:
	install -dm755 -- "$(DESTDIR)$(INCLUDEDIR)"
	install -m644 $(foreach H,$(HEADERS),src/lib/$(H).h) -- "$(DESTDIR)$(INCLUDEDIR)"


.PHONY: install-copyright
install-copyright: install-copying install-license

.PHONY: install-copying
install-copying:
	install -dm755 -- "$(DESTDIR)$(LICENSEDIR)/$(PKGNAME)"
	install -m644 COPYING -- "$(DESTDIR)$(LICENSEDIR)/$(PKGNAME)/COPYING"

.PHONY: install-license
install-license:
	install -dm755 -- "$(DESTDIR)$(LICENSEDIR)/$(PKGNAME)"
	install -m644 LICENSE -- "$(DESTDIR)$(LICENSEDIR)/$(PKGNAME)/LICENSE"


.PHONY: install-doc
install-doc: install-info install-pdf install-ps install-dvi

.PHONY: install-info
install-info: libgamma.info
	install -dm755 -- "$(DESTDIR)$(INFODIR)"
	install -m644 $< -- "$(DESTDIR)$(INFODIR)/$(PKGNAME).info"

.PHONY: install-pdf
install-pdf: libgamma.pdf
	install -dm755 -- "$(DESTDIR)$(DOCDIR)"
	install -m644 $< -- "$(DESTDIR)$(DOCDIR)/$(PKGNAME).pdf"

.PHONY: install-ps
install-ps: libgamma.ps
	install -dm755 -- "$(DESTDIR)$(DOCDIR)"
	install -m644 $< -- "$(DESTDIR)$(DOCDIR)/$(PKGNAME).ps"

.PHONY: install-dvi
install-dvi: libgamma.dvi
	install -dm755 -- "$(DESTDIR)$(DOCDIR)"
	install -m644 $< -- "$(DESTDIR)$(DOCDIR)/$(PKGNAME).dvi"


# Uninstall rules.

.PHONY: uninstall
uninstall:
	-rm -- "$(DESTDIR)$(LIBDIR)/libgamma.so.$(LIB_VERSION)"
	-rm -- "$(DESTDIR)$(LIBDIR)/libgamma.so.$(LIB_MAJOR)"
	-rm -- "$(DESTDIR)$(LIBDIR)/libgamma.so"
	-rm -- $(foreach H,$(HEADERS),"$(DESTDIR)$(INCLUDEDIR)/$(H).h")
	-rm -- "$(DESTDIR)$(LICENSEDIR)/$(PKGNAME)/COPYING"
	-rm -- "$(DESTDIR)$(LICENSEDIR)/$(PKGNAME)/LICENSE"
	-rmdir -- "$(DESTDIR)$(LICENSEDIR)/$(PKGNAME)"
	-rm -- "$(DESTDIR)$(INFODIR)/$(PKGNAME).info"
	-rm -- "$(DESTDIR)$(DOCDIR)/$(PKGNAME).pdf"
	-rm -- "$(DESTDIR)$(DOCDIR)/$(PKGNAME).ps"
	-rm -- "$(DESTDIR)$(DOCDIR)/$(PKGNAME).dvi"


# Clean rules.

.PHONY: clean
clean:
	-rm -rf obj bin libgamma.{info,pdf,ps,dvi}

.PHONY: distclean
distclean: clean
	-rm -f .config.mk src/lib/libgamma-config.h

