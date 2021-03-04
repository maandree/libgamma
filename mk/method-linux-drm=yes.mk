HDR_METHODS      += gamma-linux-drm.h
METHODS_PARAMS   += LIBGAMMA_METHOD_LINUX_DRM linux_drm 16 ramps16
CPPFLAGS_METHODS += -DHAVE_LIBGAMMA_METHOD_LINUX_DRM
CFLAGS_METHODS   += $$(pkg-config --cflags libdrm)
LDFLAGS_METHODS  += $$(pkg-config --libs libdrm)
