/**
 * libgamma -- Display server abstraction layer for gamma ramp adjustments
 * Copyright (C) 2014  Mattias Andrée (maandree@member.fsf.org)
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef HAVE_LIBGAMMA_METHOD_W32_GDI
# error Compiling gamma-w32-gdi.c without HAVE_LIBGAMMA_METHOD_W32_GDI
#endif

#include "gamma-w32-gdi.h"

#include "libgamma-error.h"

#ifndef WINVER
# define WINVER  0x0500
#endif
#ifdef FAKE_LIBGAMMA_METHOD_W32_GDI
# include "fake-w32-gdi.h"
#else
# include <windows.h>
# include <wingdi.h>
#endif

#include <errno.h>


/**
 * The gamma ramp size that devices will always have in Windows GDI.
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/dd372194(v=vs.85).aspx
 */
#define GAMMA_RAMP_SIZE  256


/**
 * Return the capabilities of the adjustment method.
 * 
 * @param  this  The data structure to fill with the method's capabilities.
 */
void libgamma_w32_gdi_method_capabilities(libgamma_method_capabilities_t* restrict this)
{
  /* Gamma ramps size, depth and support can be queried. */
  this->crtc_information = LIBGAMMA_CRTC_INFO_GAMMA_SIZE
			 | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH;
  /* Windows GDI does not support sites or partitions. */
  this->default_site_known = 1;
  this->multiple_sites = 0;
  this->multiple_partitions = 0;
  /* Windows GDI does support CRTC:s. */
  this->multiple_crtcs = 1;
  /* Partitions are not support... */
  this->partitions_are_graphics_cards = 0;
  /* Windows GDI does not have system restore capabilities. */
  this->site_restore = 0;
  this->partition_restore = 0;
  this->crtc_restore = 0;
  /* Ramps sizes are fixed and identical and ramp depth is too. */
  this->identical_gamma_sizes = 1;
  this->fixed_gamma_size = 1;
  this->fixed_gamma_depth = 1;
  /* Windows GDI is a real adjustment method that can be faked. */
#ifdef FAKE_LIBGAMMA_METHOD_W32_GDI
  /* It is considered real but fake if it is translated to X RandR. */
  this->fake = 1;
# ifdef HAVE_LIBGAMMA_METHOD_X_RANDR
  this->real = 1;
# else
  this->real = 0;
# endif
#else
  /* It is real and not fake if we are running on Windows. */
  this->fake = 0;
  this->real = 1;
#endif
  /* Gamma ramp adjustments are persistent. */
  this->auto_restore = 0;
}


/**
 * Initialise an allocated site state.
 * 
 * @param   this    The site state to initialise.
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. One the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocate on the stack. Note however that it will
 *                  not be free:d if this function fails.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library.
 */
int libgamma_w32_gdi_site_initialise(libgamma_site_state_t* restrict this,
				     char* restrict site)
{
  this->partitions_available = 1;
  return site != NULL ? LIBGAMMA_NO_SUCH_SITE : 0;
}


/**
 * Release all resources held by a site state.
 * 
 * @param  this  The site state.
 */
void libgamma_w32_gdi_site_destroy(libgamma_site_state_t* restrict this)
{
  (void) this;
}


/**
 * Restore the gamma ramps all CRTC:s with a site to the system settings.
 * 
 * @param   this  The site state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_w32_gdi_site_restore(libgamma_site_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}



/**
 * Initialise an allocated partition state.
 * 
 * @param   this       The partition state to initialise.
 * @param   site       The site state for the site that the partition belongs to.
 * @param   partition  The the index of the partition within the site.
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library.
 */
int libgamma_w32_gdi_partition_initialise(libgamma_partition_state_t* restrict this,
					  libgamma_site_state_t* restrict site, size_t partition)
{
  DWORD n = 0;
  DISPLAY_DEVICE display;
  
  (void) site;
  
  if (partition != 0)
    return LIBGAMMA_NO_SUCH_PARTITION;
  
  /* Count CRTC:s by iteration over all possible identifiers
     until we reach on that does not exist. */
  display.cb = sizeof(DISPLAY_DEVICE);
  while (EnumDisplayDevices(NULL, n, &display, 0))
    if (n++, n == 0)
      return LIBGAMMA_IMPOSSIBLE_AMOUNT;
  this->crtcs_available = (size_t)n;
  return 0;
}


/**
 * Release all resources held by a partition state.
 * 
 * @param  this  The partition state.
 */
void libgamma_w32_gdi_partition_destroy(libgamma_partition_state_t* restrict this)
{
  (void) this;
}


/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings.
 * 
 * @param   this  The partition state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_w32_gdi_partition_restore(libgamma_partition_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}



/**
 * Initialise an allocated CRTC state.
 * 
 * @param   this       The CRTC state to initialise.
 * @param   partition  The partition state for the partition that the CRTC belongs to.
 * @param   crtc       The the index of the CRTC within the site.
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library.
 */
int libgamma_w32_gdi_crtc_initialise(libgamma_crtc_state_t* restrict this,
				     libgamma_partition_state_t* restrict partition, size_t crtc)
{
  DISPLAY_DEVICE display;
  HDC context;
  
  (void) partition;
  
  this->data = NULL;
  
  /* Windows's API mandates this... */
  display.cb = sizeof(DISPLAY_DEVICE);
  /* Get identifier for selected CRTC. */
  if (!EnumDisplayDevices(NULL, (DWORD)crtc, &display, 0))
    return LIBGAMMA_NO_SUCH_CRTC;
  /* Check that the connector is enabled,
   * newer versions of Windows will always pass.
   * (According to w32's documentation, but that
   * that is a load of crap.)*/
  if (!(display.StateFlags & DISPLAY_DEVICE_ACTIVE))
    return LIBGAMMA_CONNECTOR_DISABLED;
  /* Acquire CRTC connection. */
  context = CreateDC(TEXT("DISPLAY"), display.DeviceName, NULL, NULL);
  if (context == NULL)
    return LIBGAMMA_OPEN_CRTC_FAILED;
  this->data = context;
  return 0;
}


/**
 * Release all resources held by a CRTC state.
 * 
 * @param  this  The CRTC state.
 */
void libgamma_w32_gdi_crtc_destroy(libgamma_crtc_state_t* restrict this)
{
  if (this->data)
    ReleaseDC(NULL, this->data);
}


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC.
 * 
 * @param   this  The CRTC state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_w32_gdi_crtc_restore(libgamma_crtc_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}



/**
 * Read information about a CRTC.
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC.
 * @param   crtc    The state of the CRTC whose information should be read.
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error. On error refer to the error reports in `this`.
 */
int libgamma_w32_gdi_get_crtc_information(libgamma_crtc_information_t* restrict this,
					  libgamma_crtc_state_t* restrict crtc, int32_t fields)
{
#define KNOWN_FIELDS  (LIBGAMMA_CRTC_INFO_GAMMA_SIZE | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH)
#define _E(FIELD)  ((fields & FIELD) ? LIBGAMMA_CRTC_INFO_NOT_SUPPORTED : 0)
  
  (void) crtc;
  
  /* Windows GDI does not support EDID or monitor dimensions. */
  this->edid_error = _E(LIBGAMMA_CRTC_INFO_EDID);
  this->width_mm_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM);
  this->height_mm_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM);
  this->width_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID);
  this->height_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID);
  /* Windows GDI have fixed gamma ramp sizes. */
  this->red_gamma_size = GAMMA_RAMP_SIZE;
  this->green_gamma_size = GAMMA_RAMP_SIZE;
  this->blue_gamma_size = GAMMA_RAMP_SIZE;
  this->gamma_size_error = 0;
  /* Windows GDI have fixed gamma ramp depth. */
  this->gamma_depth = 16;
  this->gamma_depth_error = 0;
  /* It is possible to query Windows GDI whether the device
     have gamma ramp support. It cannot fail. However, I think
     the result is incorrect if multiple monitors are active,
     so we cannot include this. */
  /*
  if ((fields & LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT))
    this->gamma_support = GetDeviceCaps(crtc->data, COLORMGMTCAPS) == CM_GAMMA_RAMP;
  this->gamma_support_error = 0;
  */
  this->gamma_support_error = _E(LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT);
  /* Windows GDI does not support EDID or connector information. */
  this->subpixel_order_error = _E(LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER);
  this->active_error = _E(LIBGAMMA_CRTC_INFO_ACTIVE);
  this->connector_name_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_NAME);
  this->connector_type_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE);
  this->gamma_error = _E(LIBGAMMA_CRTC_INFO_GAMMA);
  
  /* There was a failure if and only if unsupport field was requested. */
  return (fields & ~KNOWN_FIELDS) ? -1 : 0;
  
#undef _E
#undef  KNOWN_FIELDS
}


/**
 * Get the current gamma ramps for a CRTC, 16-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_w32_gdi_crtc_get_gamma_ramps16(libgamma_crtc_state_t* restrict this,
					    libgamma_gamma_ramps16_t* restrict ramps)
{
#ifdef DEBUG
  /* Windows GDI have fixed gamma ramp sizes. */
  if ((ramps->  red_size != GAMMA_RAMP_SIZE) ||
      (ramps->green_size != GAMMA_RAMP_SIZE) ||
      (ramps-> blue_size != GAMMA_RAMP_SIZE))
    return LIBGAMMA_WRONG_GAMMA_RAMP_SIZE;
#endif
  /* Read current gamma ramps. */
  if (!GetDeviceGammaRamp(this->data, ramps->red))
    return LIBGAMMA_GAMMA_RAMP_READ_FAILED;
  return 0;
}


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_w32_gdi_crtc_set_gamma_ramps16(libgamma_crtc_state_t* restrict this,
					    libgamma_gamma_ramps16_t ramps)
{
#ifdef DEBUG
  /* Windows GDI have fixed gamma ramp sizes. */
  if ((ramps.  red_size != GAMMA_RAMP_SIZE) ||
      (ramps.green_size != GAMMA_RAMP_SIZE) ||
      (ramps. blue_size != GAMMA_RAMP_SIZE))
    return LIBGAMMA_WRONG_GAMMA_RAMP_SIZE;
#endif
  /* Apply gamma ramps. */
  if (!SetDeviceGammaRamp(this->data, ramps.red))
    return LIBGAMMA_GAMMA_RAMP_WRITE_FAILED;
  return 0;
}

