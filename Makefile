.POSIX:

LIB_MAJOR = 0
LIB_MINOR = 8
LIB_VERSION = $(LIB_MAJOR).$(LIB_MINOR)


X_RANDR_METHOD   = no
X_VIDMODE_METHOD = no
LINUX_DRM_METHOD = no
W32_GDI_METHOD   = no
QUARTZ_CG_METHOD = no
DUMMY_METHOD     = yes


CONFIGFILE = config.mk
include $(CONFIGFILE)

OS = linux
# Linux:   linux
# Mac OS:  macos
# Windows: windows
include mk/$(OS).mk


HDR_METHODS      =
CPPFLAGS_METHODS =
METHODS_PARAMS   =

QUARTZ_CORE_GRAPHICS_METHOD = $(QUARTZ_CG_METHOD)
include mk/method-x-randr=$(X_RANDR_METHOD).mk
include mk/method-x-vidmode=$(X_VIDMODE_METHOD).mk
include mk/method-linux-drm=$(LINUX_DRM_METHOD).mk
include mk/method-w32-gdi=$(W32_GDI_METHOD).mk
include mk/method-quartz-cg=$(QUARTZ_CORE_GRAPHICS_METHOD).mk
include mk/method-dummy=$(DUMMY_METHOD).mk


OBJ_PUBLIC =\
	libgamma_behex_edid.o\
	libgamma_behex_edid_lowercase.o\
	libgamma_behex_edid_uppercase.o\
	libgamma_connector_type_count.o\
	libgamma_const_of_connector_type.o\
	libgamma_const_of_method.o\
	libgamma_const_of_subpixel_order.o\
	libgamma_crtc_destroy.o\
	libgamma_crtc_free.o\
	libgamma_crtc_get_gamma_ramps16.o\
	libgamma_crtc_get_gamma_ramps32.o\
	libgamma_crtc_get_gamma_ramps64.o\
	libgamma_crtc_get_gamma_ramps8.o\
	libgamma_crtc_get_gamma_rampsd.o\
	libgamma_crtc_get_gamma_rampsf.o\
	libgamma_crtc_info_count.o\
	libgamma_crtc_information_destroy.o\
	libgamma_crtc_information_free.o\
	libgamma_crtc_initialise.o\
	libgamma_crtc_restore.o\
	libgamma_crtc_set_gamma_ramps16.o\
	libgamma_crtc_set_gamma_ramps16_f.o\
	libgamma_crtc_set_gamma_ramps32.o\
	libgamma_crtc_set_gamma_ramps32_f.o\
	libgamma_crtc_set_gamma_ramps64.o\
	libgamma_crtc_set_gamma_ramps64_f.o\
	libgamma_crtc_set_gamma_ramps8.o\
	libgamma_crtc_set_gamma_ramps8_f.o\
	libgamma_crtc_set_gamma_rampsd.o\
	libgamma_crtc_set_gamma_rampsd_f.o\
	libgamma_crtc_set_gamma_rampsf.o\
	libgamma_crtc_set_gamma_rampsf_f.o\
	libgamma_error_min.o\
	libgamma_gamma_ramps16_destroy.o\
	libgamma_gamma_ramps16_free.o\
	libgamma_gamma_ramps16_initialise.o\
	libgamma_gamma_ramps32_destroy.o\
	libgamma_gamma_ramps32_free.o\
	libgamma_gamma_ramps32_initialise.o\
	libgamma_gamma_ramps64_destroy.o\
	libgamma_gamma_ramps64_free.o\
	libgamma_gamma_ramps64_initialise.o\
	libgamma_gamma_ramps8_destroy.o\
	libgamma_gamma_ramps8_free.o\
	libgamma_gamma_ramps8_initialise.o\
	libgamma_gamma_rampsd_destroy.o\
	libgamma_gamma_rampsd_free.o\
	libgamma_gamma_rampsd_initialise.o\
	libgamma_gamma_rampsf_destroy.o\
	libgamma_gamma_rampsf_free.o\
	libgamma_gamma_rampsf_initialise.o\
	libgamma_get_crtc_information.o\
	libgamma_group_gid.o\
	libgamma_group_name.o\
	libgamma_is_method_available.o\
	libgamma_list_methods.o\
	libgamma_method_capabilities.o\
	libgamma_method_count.o\
	libgamma_method_default_site.o\
	libgamma_method_default_site_variable.o\
	libgamma_name_of_connector_type.o\
	libgamma_name_of_error.o\
	libgamma_name_of_method.o\
	libgamma_name_of_subpixel_order.o\
	libgamma_partition_destroy.o\
	libgamma_partition_free.o\
	libgamma_partition_initialise.o\
	libgamma_partition_restore.o\
	libgamma_perror.o\
	libgamma_site_destroy.o\
	libgamma_site_free.o\
	libgamma_site_initialise.o\
	libgamma_site_restore.o\
	libgamma_strerror.o\
	libgamma_strerror_r.o\
	libgamma_subpixel_order_count.o\
	libgamma_unhex_edid.o\
	libgamma_value_of_connector_type.o\
	libgamma_value_of_error.o\
	libgamma_value_of_method.o\
	libgamma_value_of_subpixel_order.o

OBJ_INTERNAL =\
	libgamma_internal_allocated_any_ramp.o\
	libgamma_internal_parse_edid.o\
	libgamma_internal_translated_ramp_get_.o\
	libgamma_internal_translated_ramp_set_.o\
	libgamma_internal_translate_from_64.o\
	libgamma_internal_translate_to_64.o\

OBJ = $(OBJ_PUBLIC) $(OBJ_INTERNAL)
LOBJ = $(OBJ:.o=.lo)

HDR =\
	common.h\
	config.h\
	get_ramps.h\
	libgamma.h\
	set_ramps.h\
	set_ramps_fun.h\
	$(HDR_METHODS)


all: libgamma.a libgamma.$(LIBEXT)
$(OBJ): $(@:.o=.c) $(HDR)
$(LOBJ): $(@:.lo=.c) $(HDR)

config.h: FORCE
	printf '/* This file is auto-generated.mk */\n' > $@~
	printf '#include "%s"\n' $(HDR_METHODS) >> $@~
	printf '#define LIST_AVAILABLE_METHODS(_)' >> $@~
	printf '\\\n\t_(%s, %s, %s, %s)' $(METHODS_PARAMS) >> $@~
	printf '\n' >> $@~
	if ! test -f $@ || ! test "$$(cat < $@)" = "$$(cat < $@~)"; then mv -- $@~ $@; fi

libgamma.a: $(OBJ)
	-rm -f -- $@
	$(AR) -rc $@ $(OBJ)
	$(AR) -s $@

libgamma.$(LIBEXT): $(LOBJ)
	$(CC) $(LIBFLAGS) $(LDFLAGS_METHODS) -o $@ $(LOBJ) $(LDFLAGS)

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(CFLAGS_METHODS) $(CPPFLAGS) $(CPPFLAGS_METHODS)

.c.lo:
	$(CC) -fPIC -c -o $@ $< $(CFLAGS) $(CFLAGS_METHODS) $(CPPFLAGS) $(CPPFLAGS_METHODS)

install: libgamma.a libgamma.$(LIBEXT)
	mkdir -p -- "$(DESTDIR)$(PREFIX)/lib/"
	mkdir -p -- "$(DESTDIR)$(PREFIX)/include/"
	cp -- libgamma.$(LIBEXT) "$(DESTDIR)$(PREFIX)/lib/libgamma.$(LIBMINOREXT)"
	ln -sf -- libgamma.$(LIBMINOREXT) "$(DESTDIR)$(PREFIX)/lib/libgamma.$(LIBMAJOREXT)"
	ln -sf -- libgamma.$(LIBMAJOREXT) "$(DESTDIR)$(PREFIX)/lib/libgamma.$(LIBEXT)"
	cp -- libgamma.a "$(DESTDIR)$(PREFIX)/lib/"
	cp -- libgamma.h "$(DESTDIR)$(PREFIX)/include/"

uninstall:
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/libgamma.$(LIBMAJOREXT)"
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/libgamma.$(LIBMINOREXT)"
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/libgamma.$(LIBEXT)"
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/libgamma.a"
	-rm -f -- "$(DESTDIR)$(PREFIX)/include/libgamma.h"

clean:
	-rm -f -- *.o *.lo *.su *.a *.$(LIBEXT)

.SUFFIXES:
.SUFFIXES: .lo .o .c

FORCE:
.PHONY: all install uninstall clean FORCE
