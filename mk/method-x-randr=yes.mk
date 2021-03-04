HDR_METHODS      += gamma-x-randr.h
METHODS_PARAMS   += LIBGAMMA_METHOD_X_RANDR x_randr 16 ramps16
CPPFLAGS_METHODS += -DHAVE_LIBGAMMA_METHOD_X_RANDR
CFLAGS_METHODS   += $$(pkg-config --cflags xcb xcb-randr)
LDFLAGS_METHODS  += $$(pkg-config --libs xcb xcb-randr)
