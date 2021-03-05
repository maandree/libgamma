HDR_W32_GDI      = method-w32-gdi.h
PARAMS_W32_GDI   = LIBGAMMA_METHOD_W32_GDI w32_gdi 16 ramps16
CPPFLAGS_W32_GDI = -DHAVE_LIBGAMMA_METHOD_W32_GDI

OBJ_W32_GDI =\
	libgamma_w32_gdi_method_capabilities.o\
	libgamma_w32_gdi_site_initialise.o\
	libgamma_w32_gdi_site_destroy.o\
	libgamma_w32_gdi_site_restore.o\
	libgamma_w32_gdi_partition_initialise.o\
	libgamma_w32_gdi_partition_destroy.o\
	libgamma_w32_gdi_partition_restore.o\
	libgamma_w32_gdi_crtc_initialise.o\
	libgamma_w32_gdi_crtc_destroy.o\
	libgamma_w32_gdi_crtc_restore.o\
	libgamma_w32_gdi_get_crtc_information.o\
	libgamma_w32_gdi_crtc_get_gamma_ramps16.o\
	libgamma_w32_gdi_crtc_set_gamma_ramps16.o
