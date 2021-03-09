HDR_X_RANDR      = method-x-randr.h
PARAMS_X_RANDR   = LIBGAMMA_METHOD_X_RANDR x_randr 16 ramps16
CPPFLAGS_X_RANDR = -DHAVE_LIBGAMMA_METHOD_X_RANDR
CFLAGS_X_RANDR   = $$(pkg-config --cflags xcb xcb-randr)
LDFLAGS_X_RANDR  = $$(pkg-config --libs xcb xcb-randr)
DEPS_X_RANDR     = xcb xcb-randr

OBJ_X_RANDR =\
	libgamma_x_randr_method_capabilities.o\
	libgamma_x_randr_site_initialise.o\
	libgamma_x_randr_site_destroy.o\
	libgamma_x_randr_site_restore.o\
	libgamma_x_randr_partition_initialise.o\
	libgamma_x_randr_partition_destroy.o\
	libgamma_x_randr_partition_restore.o\
	libgamma_x_randr_crtc_initialise.o\
	libgamma_x_randr_crtc_destroy.o\
	libgamma_x_randr_crtc_restore.o\
	libgamma_x_randr_get_crtc_information.o\
	libgamma_x_randr_crtc_get_gamma_ramps16.o\
	libgamma_x_randr_crtc_set_gamma_ramps16.o\
	libgamma_x_randr_internal_translate_error.o
