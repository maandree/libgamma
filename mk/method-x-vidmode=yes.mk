HDR_METHODS      += gamma-x-vidmode.h
METHODS_PARAMS   += LIBGAMMA_METHOD_X_VIDMODE x_vidmode 16 ramps16
CPPFLAGS_METHODS += -DHAVE_LIBGAMMA_METHOD_X_VIDMODE
CFLAGS_METHODS   += $$(pkg-config --cflags x11 xxf86vm)
LDFLAGS_METHODS  += $$(pkg-config --libs x11 xxf86vm)
