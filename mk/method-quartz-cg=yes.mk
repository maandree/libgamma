F_APPLICATION_SERVICES = /System/Library/Frameworks/ApplicationServices.framework
I_APPLICATION_SERVICES = $(F_APPLICATION_SERVICES)/Versions/A/Frameworks/CoreGraphics.framework/Versions/A/Headers

HDR_QUARTZ_GC      = method-quartz-cg.h
PARAMS_QUARTZ_GC   = LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS quartz_cg -1 rampsf
CPPFLAGS_QUARTZ_GC = -DHAVE_LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS
CFLAGS_QUARTZ_GC   = -I$(I_APPLICATION_SERVICES) -F$(F_APPLICATION_SERVICES)
LDFLAGS_QUARTZ_GC  = -framework ApplicationServices

OBJ_QUARTZ_GC =\
	libgamma_quartz_cg_method_capabilities.o\
	libgamma_quartz_cg_site_initialise.o\
	libgamma_quartz_cg_site_destroy.o\
	libgamma_quartz_cg_site_restore.o\
	libgamma_quartz_cg_partition_initialise.o\
	libgamma_quartz_cg_partition_destroy.o\
	libgamma_quartz_cg_partition_restore.o\
	libgamma_quartz_cg_crtc_initialise.o\
	libgamma_quartz_cg_crtc_destroy.o\
	libgamma_quartz_cg_crtc_restore.o\
	libgamma_quartz_cg_get_crtc_information.o\
	libgamma_quartz_cg_crtc_get_gamma_rampsf.o\
	libgamma_quartz_cg_crtc_set_gamma_rampsf.o
