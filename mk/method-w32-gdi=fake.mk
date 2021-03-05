include mk/method-w32-gdi=yes.mk
HDR_W32_GDI      = fake-w32-gdi.h method-w32-gdi.h
OBJ_FAKE_W32_GDI = fake-w32-gdi.o
CPPFLAGS_W32_GDI = -DHAVE_LIBGAMMA_METHOD_W32_GDI -DFAKE_LIBGAMMA_METHOD_W32_GDI
CFLAGS_W32_GDI   = 
LDFLAGS_W32_GDI  = 
