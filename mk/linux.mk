LINUX_DRM_METHOD = yes
X_RANDR_METHOD   = yes
X_VIDMODE_METHOD = yes

LIBEXT      = so
LIBFLAGS    = -shared -Wl,-soname,libgamma.$(LIBEXT).$(LIB_MAJOR)
LIBMAJOREXT = $(LIBEXT).$(LIB_MAJOR)
LIBMINOREXT = $(LIBEXT).$(LIB_VERSION)

FIX_INSTALL_NAME = :
