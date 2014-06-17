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
#ifndef HAVE_LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS
# error Compiling gamma-quartz-cg.c without HAVE_LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS
#endif

#include "gamma-quartz-cg.h"

#include "libgamma-error.h"

#ifdef FAKE_LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS
# include "fake-quartz-cg.h"
#else
# include <CoreGraphics/CGDirectDisplay.h>
# define close_fake_quartz_cg() /* For compatibility with "fake-quartz-cg.h". */
#endif

#include <stdlib.h>
#include <errno.h>


/**
 * Return the capabilities of the adjustment method.
 * 
 * @param  this  The data structure to fill with the method's capabilities.
 */
void libgamma_quartz_cg_method_capabilities(libgamma_method_capabilities_t* restrict this)
{
  /* Gamma ramps size and depth can be queried. */
  this->crtc_information = LIBGAMMA_CRTC_INFO_GAMMA_SIZE
			 | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH;
  /* Quartz/CoreGraphics does not support sites or partitions. */
  this->default_site_known = 1;
  this->multiple_sites = 0;
  this->multiple_partitions = 0;
  /* Quartz/CoreGraphics does support CRTC:s. */
  this->multiple_crtcs = 1;
  /* Partitions are not support... */
  this->partitions_are_graphics_cards = 0;
  /* CoreGraphics have support for system restore. */
  this->site_restore = 1;
  this->partition_restore = 1;
  /* But not for individual CRTC:s. */
  this->crtc_restore = 0;
  /* Gamma ramp sizes are identifical but not fixed. */
  this->identical_gamma_sizes = 1;
  this->fixed_gamma_size = 0;
  /* Gamma ramp depths are fixed. */
  this->fixed_gamma_depth = 1;
  /* Quartz/CoreGraphics is a real adjustment method that can be faked. */
#ifdef FAKE_LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS
  /* It is considered real but fake if it is translated to X RandR. */
  this->fake = 1;
# ifdef HAVE_LIBGAMMA_METHOD_X_RANDR
  this->real = 1;
# else
  this->real = 0;
# endif
#else
  /* It is real and not fake if we are running on Mac OS X. */
  this->fake = 0;
  this->real = 1;
#endif
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
int libgamma_quartz_cg_site_initialise(libgamma_site_state_t* restrict this,
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
void libgamma_quartz_cg_site_destroy(libgamma_site_state_t* restrict this)
{
  (void) this;
  close_fake_quartz_cg();
}


/**
 * Restore the gamma ramps all CRTC:s with a site to the system settings.
 * 
 * @param   this  The site state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_quartz_cg_site_restore(libgamma_site_state_t* restrict this)
{
  (void) this;
  CGDisplayRestoreColorSyncSettings();
  return 0;
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
int libgamma_quartz_cg_partition_initialise(libgamma_partition_state_t* restrict this,
					    libgamma_site_state_t* restrict site, size_t partition)
{
  uint32_t cap = 4, n;
  CGDirectDisplayID* crtcs;
  CGDirectDisplayID* crtcs_old;
  
  (void) site;
  
  this->data = NULL;
  
  if (partition != 0)
    return LIBGAMMA_NO_SUCH_PARTITION;
  
  /* Allocate array of CRTC ID:s. */
  if ((crtcs = malloc((size_t)cap * sizeof(CGDirectDisplayID))) == NULL)
    return LIBGAMMA_ERRNO_SET;
  
  /* It is not possible to ask CoreGraphics how many CRTC:s
     are available. We have to ask it to give us a ID:s of
     a number of CRTC:s and ask for more if we got as many
     as we asked for. */
  for (;;)
    {
      /* Ask for CRTC ID:s */
      if (CGGetOnlineDisplayList(cap, crtcs, &n) != kCGErrorSuccess)
	return free(crtcs), LIBGAMMA_LIST_CRTCS_FAILED;
      /* If we did not get as many as we asked for then we have all. */
      if (n < cap)
	break;
      /* Increase the number CRTC ID:s to ask for. */
      if ((cap <<= 1) == 0) /* We could also test ~0, but it is still too many. */
	return free(crtcs), LIBGAMMA_IMPOSSIBLE_AMOUNT;
      /* Grow the array of CRTC ID:s so that it can fit all we are asking for. */
      if ((crtcs = realloc(crtcs_old = crtcs, (size_t)cap * sizeof(CGDirectDisplayID))) == NULL)
	return free(crtcs_old), LIBGAMMA_ERRNO_SET;
    }
  
  /* Store CRTC ID:s and CRTC count. */
  this->data = crtcs;
  this->crtcs_available = (size_t)n;
  return 0;
}


/**
 * Release all resources held by a partition state.
 * 
 * @param  this  The partition state.
 */
void libgamma_quartz_cg_partition_destroy(libgamma_partition_state_t* restrict this)
{
  free(this->data);
}


/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings.
 * 
 * @param   this  The partition state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_quartz_cg_partition_restore(libgamma_partition_state_t* restrict this)
{
  return libgamma_quartz_cg_site_restore(this->site);
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
int libgamma_quartz_cg_crtc_initialise(libgamma_crtc_state_t* restrict this,
				       libgamma_partition_state_t* restrict partition, size_t crtc)
{
  (void) this;
  return crtc >= partition->crtcs_available ? LIBGAMMA_NO_SUCH_CRTC : 0;
}


/**
 * Release all resources held by a CRTC state.
 * 
 * @param  this  The CRTC state.
 */
void libgamma_quartz_cg_crtc_destroy(libgamma_crtc_state_t* restrict this)
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
int libgamma_quartz_cg_crtc_restore(libgamma_crtc_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}



/**
 * Read information about a CRTC.
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC.
 * @param   crtc    The state of the CRTC whose information should be read.
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read.
 * @return          Zero on success, -1 on error. On error refer to the error reports in `this`.
 */
int libgamma_quartz_cg_get_crtc_information(libgamma_crtc_information_t* restrict this,
					    libgamma_crtc_state_t* restrict crtc, int32_t fields)
{
#define SUPPORTED_FIELDS  (LIBGAMMA_CRTC_INFO_GAMMA_SIZE | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH)
#define _E(FIELD)  ((fields & FIELD) ? LIBGAMMA_CRTC_INFO_NOT_SUPPORTED : 0)
  
  /* Quartz/CoreGraphics does not support EDID or monitor dimensions. */
  this->edid_error = _E(LIBGAMMA_CRTC_INFO_EDID);
  this->width_mm_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM);
  this->height_mm_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM);
  this->width_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID);
  this->height_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID);
  /* Quartz/CoreGraphics does support gamma ramp size query.
     The gamma ramps are identical but not fixed, and the query can fail. */
  this->gamma_size_error = 0;
  if ((fields & LIBGAMMA_CRTC_INFO_GAMMA_SIZE))
    {
      CGDirectDisplayID* restrict crtcs = crtc->partition->data;
      size_t gamma_size = CGDisplayGammaTableCapacity(crtcs[crtc->crtc]);
      this->red_gamma_size = this->green_gamma_size = this->blue_gamma_size = (size_t)gamma_size;
      this->gamma_size_error = gamma_size < 2 ? LIBGAMMA_SINGLETON_GAMMA_RAMP : 0;
    }
  /* Quartz/CoreGraphics uses `float` ramps. */
  this->gamma_depth = -1;
  this->gamma_depth_error = 0;
  /* Quartz/CoreGraphics does not support gamma ramp support queries. */
  this->gamma_support_error = _E(LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT);
  /* Quartz/CoreGraphics does not support EDID or connector information. */
  this->subpixel_order_error = _E(LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER);
  this->active_error = _E(LIBGAMMA_CRTC_INFO_ACTIVE);
  this->connector_name_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_NAME);
  this->connector_type_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE);
  this->gamma_error = _E(LIBGAMMA_CRTC_INFO_GAMMA);
  
  /* We failed if gamma ramp size query failed or if an unsupport field was queried. */
  return this->gamma_size_error || (fields & ~SUPPORTED_FIELDS) ? -1 : 0;
  
#undef _E
#undef SUPPORTED_FIELDS
}


/**
 * Get current the gamma ramps for a CRTC, `float` version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_quartz_cg_crtc_get_gamma_rampsf(libgamma_crtc_state_t* restrict this,
					     libgamma_gamma_rampsf_t* restrict ramps)
{
  CGDirectDisplayID* restrict crtcs = this->partition->data;
  CGDirectDisplayID crtc_id = crtcs[this->crtc];
  uint32_t gamma_size_out;
  CGError r;
#ifdef DEBUG
  /* Gamma ramps sizes are identical but not fixed. */
  if ((ramps->red_size != ramps->green_size) ||
      (ramps->red_size != ramps->blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  /* Read current gamma ramps. */
  r = CGGetDisplayTransferByTable(crtc_id, (uint32_t)(ramps->red_size),
				  ramps->red, ramps->green, ramps->blue, &gamma_size_out);
  if (r != kCGErrorSuccess)
    return LIBGAMMA_GAMMA_RAMP_READ_FAILED;
  /* I hope that it will not actually ever change,
     but it does return the the gamma ramp size despite
     that it can be queried without querying for more. */
  if (gamma_size_out != ramps->red_size)
    return LIBGAMMA_GAMMA_RAMP_SIZE_CHANGED;
  return 0;
}


/**
 * Set the gamma ramps for a CRTC, `float` version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_quartz_cg_crtc_set_gamma_rampsf(libgamma_crtc_state_t* restrict this,
					     libgamma_gamma_rampsf_t ramps)
{
  CGDirectDisplayID* restrict crtcs = this->partition->data;
  CGDirectDisplayID crtc_id = crtcs[this->crtc];
  CGError r;
#ifdef DEBUG
  /* Gamma ramps sizes are identical but not fixed. */
  if ((ramps.red_size != ramps.green_size) ||
      (ramps.red_size != ramps.blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  /* Apply gamma ramps. */
  r = CGSetDisplayTransferByTable(crtc_id, (uint32_t)(ramps.red_size),
				  ramps.red, ramps.green, ramps.blue);
  return r == kCGErrorSuccess ? 0 : LIBGAMMA_GAMMA_RAMP_WRITE_FAILED;
}

