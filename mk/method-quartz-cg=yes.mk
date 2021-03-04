F_APPLICATION_SERVICES = /System/Library/Frameworks/ApplicationServices.framework
I_APPLICATION_SERVICES = $(F_APPLICATION_SERVICES)/Versions/A/Frameworks/CoreGraphics.framework/Versions/A/Headers

HDR_METHODS      += gamma-quartz-cg.h
METHODS_PARAMS   += LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS quartz_cg -1 rampsf
CPPFLAGS_METHODS += -DHAVE_LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS
CFLAGS_METHODS   += -I$(I_APPLICATION_SERVICES) -F$(F_APPLICATION_SERVICES)
LDFLAGS_METHODS  += -framework ApplicationServices
