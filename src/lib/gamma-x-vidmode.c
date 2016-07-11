/**
 * libgamma -- Display server abstraction layer for gamma ramp adjustments
 * Copyright (C) 2014, 2015  Mattias Andrée (maandree@member.fsf.org)
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
#ifndef HAVE_LIBGAMMA_METHOD_X_VIDMODE
# error Compiling gamma-x-vidmode.c without HAVE_LIBGAMMA_METHOD_X_VIDMODE
#endif

#include "gamma-x-vidmode.h"

#include "libgamma-error.h"

#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>

#include <stdlib.h>
#include <errno.h>


/**
 * Return the capabilities of the adjustment method.
 * 
 * @param  this  The data structure to fill with the method's capabilities.
 */
void libgamma_x_vidmode_method_capabilities(libgamma_method_capabilities_t* restrict this)
{
  char* restrict display = getenv("DISPLAY");
  /* Gamma ramps size and depth can be queried. */
  this->crtc_information = LIBGAMMA_CRTC_INFO_GAMMA_SIZE
			 | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH;
  /* X VidMode supports multiple sites and partitions but not CRTC:s. */
  this->default_site_known = (display && *display) ? 1 : 0;
  this->multiple_sites = 1;
  this->multiple_partitions = 1;
  this->multiple_crtcs = 0;
  /* Partitions are screens and not graphics cards in X. */
  this->partitions_are_graphics_cards = 0;
  /* X does not have system restore capabilities. */
  this->site_restore = 0;
  this->partition_restore = 0;
  this->crtc_restore = 0;
  /* Gamma ramp sizes are identical but not fixed. */
  this->identical_gamma_sizes = 1;
  this->fixed_gamma_size = 0;
  /* Gamma ramp depths are fixed. */
  this->fixed_gamma_depth = 1;
  /* X VidMode is a real non-faked adjustment method */
  this->real = 1;
  this->fake = 0;
  /* Gamma ramp adjustments are persistent. */
  this->auto_restore = 0;
}


/**
 * Initialise an allocated site state.
 * 
 * @param   this    The site state to initialise.
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. Once the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocate on the stack. Note however that it will
 *                  not be free:d if this function fails.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library.
 */
int libgamma_x_vidmode_site_initialise(libgamma_site_state_t* restrict this,
				       char* restrict site)
{
  /* Connect to the display. */
  Display* restrict connection = XOpenDisplay(site);
  int _major, _minor, screens;
  if ((this->data = connection) == NULL)
    return LIBGAMMA_OPEN_SITE_FAILED;
  /* Query X VidMode extension protocol version. */
  if (!XF86VidModeQueryVersion(connection, &_major, &_minor))
    return XCloseDisplay(connection), LIBGAMMA_PROTOCOL_VERSION_QUERY_FAILED;
  /* Query the number of available screens. */
  if ((screens = ScreenCount(connection)) < 0)
    return XCloseDisplay(connection), LIBGAMMA_NEGATIVE_PARTITION_COUNT;
  this->partitions_available = (size_t)screens;
  return 0;
}


/**
 * Release all resources held by a site state.
 * 
 * @param  this  The site state.
 */
void libgamma_x_vidmode_site_destroy(libgamma_site_state_t* restrict this)
{
  XCloseDisplay((Display*)(this->data));
}


/**
 * Restore the gamma ramps all CRTC:s with a site to the system settings.
 * 
 * @param   this  The site state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_x_vidmode_site_restore(libgamma_site_state_t* restrict this)
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
int libgamma_x_vidmode_partition_initialise(libgamma_partition_state_t* restrict this,
					    libgamma_site_state_t* restrict site, size_t partition)
{
  this->crtcs_available = 1;
  return partition >= site->partitions_available ? LIBGAMMA_NO_SUCH_PARTITION : 0;
}


/**
 * Release all resources held by a partition state.
 * 
 * @param  this  The partition state.
 */
void libgamma_x_vidmode_partition_destroy(libgamma_partition_state_t* restrict this)
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
int libgamma_x_vidmode_partition_restore(libgamma_partition_state_t* restrict this)
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
int libgamma_x_vidmode_crtc_initialise(libgamma_crtc_state_t* restrict this,
				       libgamma_partition_state_t* restrict partition, size_t crtc)
{
  (void) this;
  (void) partition;
  return crtc == 0 ? 0 : LIBGAMMA_NO_SUCH_CRTC;
}


/**
 * Release all resources held by a CRTC state.
 * 
 * @param  this  The CRTC state.
 */
void libgamma_x_vidmode_crtc_destroy(libgamma_crtc_state_t* restrict this)
{
  (void) this;
}


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC.
 * 
 * @param   this  The CRTC state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_x_vidmode_crtc_restore(libgamma_crtc_state_t* restrict this)
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
int libgamma_x_vidmode_get_crtc_information(libgamma_crtc_information_t* restrict this,
					    libgamma_crtc_state_t* restrict crtc, int32_t fields)
{
#define _E(FIELD)  ((fields & FIELD) ? LIBGAMMA_CRTC_INFO_NOT_SUPPORTED : 0)
  
  /* X VidMode does not support EDID or monitor dimensions. */
  this->edid_error = _E(LIBGAMMA_CRTC_INFO_EDID);
  this->width_mm_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM);
  this->height_mm_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM);
  this->width_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID);
  this->height_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID);
  this->gamma_size_error = 0;
  /* X VidMode does support gamma ramp size query. The gamma
     ramps are identical but not fixed, and the query can fail. */
  if ((fields & LIBGAMMA_CRTC_INFO_GAMMA_SIZE))
    {
      Display* restrict connection = crtc->partition->site->data;
      int stops = 0;
      if (!XF86VidModeGetGammaRampSize(connection, (int)(crtc->partition->partition), &stops))
	this->gamma_size_error = LIBGAMMA_GAMMA_RAMPS_SIZE_QUERY_FAILED;
      else if (stops < 2)
	this->gamma_size_error = LIBGAMMA_SINGLETON_GAMMA_RAMP;
      this->red_gamma_size = this->green_gamma_size = this->blue_gamma_size = (size_t)stops;
    }
  /* X VidMode uses 16-bit integer ramps. */
  this->gamma_depth = 16;
  this->gamma_depth_error = 0;
  /* X VidMode does not support gamma ramp support queries. */
  this->gamma_support_error = _E(LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT);
  /* X VidMode does not support EDID or connector information. */
  this->subpixel_order_error = _E(LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER);
  this->active_error = _E(LIBGAMMA_CRTC_INFO_ACTIVE);
  this->connector_name_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_NAME);
  this->connector_type_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE);
  this->gamma_error = _E(LIBGAMMA_CRTC_INFO_GAMMA);
  
  /* We failed if gamma ramp size query failed or if an unsupport field was queried. */
  return this->gamma_size_error || (fields & ~(LIBGAMMA_CRTC_INFO_GAMMA_DEPTH | LIBGAMMA_CRTC_INFO_GAMMA_SIZE)) ? -1 : 0;
  
#undef _E
}


/**
 * Get the current gamma ramps for a CRTC, 16-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_x_vidmode_crtc_get_gamma_ramps16(libgamma_crtc_state_t* restrict this,
					      libgamma_gamma_ramps16_t* restrict ramps)
{
#ifdef DEBUG
  /* Gamma ramp sizes are identical but not fixed. */
  if ((ramps->red_size != ramps->green_size) ||
      (ramps->red_size != ramps->blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  /* Read current gamma ramps. */
  if (!XF86VidModeGetGammaRamp((Display*)(this->partition->site->data), (int)(this->partition->partition),
			       (int)(ramps->red_size), ramps->red, ramps->green, ramps->blue))
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
int libgamma_x_vidmode_crtc_set_gamma_ramps16(libgamma_crtc_state_t* restrict this,
					      libgamma_gamma_ramps16_t ramps)
{
#ifdef DEBUG
  /* Gamma ramp sizes are identical but not fixed. */
  if ((ramps.red_size != ramps.green_size) ||
      (ramps.red_size != ramps.blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  /* Apply gamma ramps. */
  if (!XF86VidModeSetGammaRamp((Display*)(this->partition->site->data), (int)(this->partition->partition),
			       (int)(ramps.red_size), ramps.red, ramps.green, ramps.blue))
    return LIBGAMMA_GAMMA_RAMP_WRITE_FAILED;
  return 0;
}

