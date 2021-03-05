HDR_X_VIDMODE      = method-x-vidmode.h
PARAMS_X_VIDMODE   = LIBGAMMA_METHOD_X_VIDMODE x_vidmode 16 ramps16
CPPFLAGS_X_VIDMODE = -DHAVE_LIBGAMMA_METHOD_X_VIDMODE
CFLAGS_X_VIDMODE   = $$(pkg-config --cflags x11 xxf86vm)
LDFLAGS_X_VIDMODE  = $$(pkg-config --libs x11 xxf86vm)

OBJ_X_VIDMODE =\
	libgamma_x_vidmode_method_capabilities.o\
	libgamma_x_vidmode_site_initialise.o\
	libgamma_x_vidmode_site_destroy.o\
	libgamma_x_vidmode_site_restore.o\
	libgamma_x_vidmode_partition_initialise.o\
	libgamma_x_vidmode_partition_destroy.o\
	libgamma_x_vidmode_partition_restore.o\
	libgamma_x_vidmode_crtc_initialise.o\
	libgamma_x_vidmode_crtc_destroy.o\
	libgamma_x_vidmode_crtc_restore.o\
	libgamma_x_vidmode_get_crtc_information.o\
	libgamma_x_vidmode_crtc_get_gamma_ramps16.o\
	libgamma_x_vidmode_crtc_set_gamma_ramps16.o
