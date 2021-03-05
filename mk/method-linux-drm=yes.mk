HDR_LINUX_DRM      = method-linux-drm.h
PARAMS_LINUX_DRM   = LIBGAMMA_METHOD_LINUX_DRM linux_drm 16 ramps16
CPPFLAGS_LINUX_DRM = -DHAVE_LIBGAMMA_METHOD_LINUX_DRM
CFLAGS_LINUX_DRM   = $$(pkg-config --cflags libdrm)
LDFLAGS_LINUX_DRM  = $$(pkg-config --libs libdrm)

OBJ_LINUX_DRM =\
	libgamma_linux_drm_method_capabilities.o\
	libgamma_linux_drm_site_initialise.o\
	libgamma_linux_drm_site_destroy.o\
	libgamma_linux_drm_site_restore.o\
	libgamma_linux_drm_partition_initialise.o\
	libgamma_linux_drm_partition_destroy.o\
	libgamma_linux_drm_partition_restore.o\
	libgamma_linux_drm_crtc_initialise.o\
	libgamma_linux_drm_crtc_destroy.o\
	libgamma_linux_drm_crtc_restore.o\
	libgamma_linux_drm_get_crtc_information.o\
	libgamma_linux_drm_crtc_get_gamma_ramps16.o\
	libgamma_linux_drm_crtc_set_gamma_ramps16.o\
	libgamma_linux_drm_internal_release_connectors_and_encoders.o
