HDR_DUMMY      = method-dummy.h
PARAMS_DUMMY   = LIBGAMMA_METHOD_DUMMY dummy 0 ramps16
CPPFLAGS_DUMMY = -DHAVE_LIBGAMMA_METHOD_DUMMY

OBJ_DUMMY =\
	libgamma_dummy_method_capabilities.o\
	libgamma_dummy_site_initialise.o\
	libgamma_dummy_site_destroy.o\
	libgamma_dummy_site_restore.o\
	libgamma_dummy_partition_initialise.o\
	libgamma_dummy_partition_destroy.o\
	libgamma_dummy_partition_restore.o\
	libgamma_dummy_crtc_initialise.o\
	libgamma_dummy_crtc_destroy.o\
	libgamma_dummy_crtc_restore.o\
	libgamma_dummy_get_crtc_information.o\
	libgamma_dummy_crtc_get_gamma_ramps8.o\
	libgamma_dummy_crtc_set_gamma_ramps8.o\
	libgamma_dummy_crtc_get_gamma_ramps16.o\
	libgamma_dummy_crtc_set_gamma_ramps16.o\
	libgamma_dummy_crtc_get_gamma_ramps32.o\
	libgamma_dummy_crtc_set_gamma_ramps32.o\
	libgamma_dummy_crtc_get_gamma_ramps64.o\
	libgamma_dummy_crtc_set_gamma_ramps64.o\
	libgamma_dummy_crtc_get_gamma_rampsf.o\
	libgamma_dummy_crtc_set_gamma_rampsf.o\
	libgamma_dummy_crtc_get_gamma_rampsd.o\
	libgamma_dummy_crtc_set_gamma_rampsd.o\
	libgamma_dummy_internal_configurations.o\
	libgamma_dummy_internal_crtc_restore_forced.o
