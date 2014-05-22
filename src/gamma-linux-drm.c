/**
 * libgamma — Display server abstraction layer for gamma ramp adjustments
 * Copyright © 2014  Mattias Andrée (maandree@member.fsf.org)
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
#ifndef HAVE_GAMMA_METHOD_LINUX_DRM
# error Compiling gamma-linux-drm.c without HAVE_GAMMA_METHOD_LINUX_DRM
#endif

#define _GNU_SOURCE

#include "gamma-linux-drm.h"

#include "libgamma-error.h"

#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <grp.h>
#include <unistd.h>
#include <stdio.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#ifndef O_CLOEXEC
# define O_CLOEXEC  02000000
#endif
#ifndef NGROUPS_MAX
# define NGROUPS_MAX  65536
#endif
#ifndef PATH_MAX
# define PATH_MAX  4096
#endif



/**
 * Graphics card data for the Direct Rendering Manager adjustment method
 */
typedef struct libgamma_drm_card_data
{
  /**
   * File descriptor for the connection to the graphics card
   */
  int fd;
  
  /**
   * The graphics card's mode resources
   */
  drmModeRes* res;
  
} libgamma_drm_card_data_t;



/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
void libgamma_linux_drm_method_capabilities(libgamma_method_capabilities_t* restrict this)
{
  this->crtc_information = CRTC_INFO_EDID
			 | CRTC_INFO_WIDTH_MM
			 | CRTC_INFO_HEIGHT_MM
			 | CRTC_INFO_WIDTH_MM_EDID
			 | CRTC_INFO_HEIGHT_MM_EDID
			 | CRTC_INFO_GAMMA_SIZE
			 | CRTC_INFO_GAMMA_DEPTH
			 | CRTC_INFO_SUBPIXEL_ORDER
			 | CRTC_INFO_CONNECTOR_NAME
			 | CRTC_INFO_CONNECTOR_TYPE
			 | CRTC_INFO_GAMMA;
  this->default_site_known = 1;
  this->multiple_sites = 0;
  this->multiple_partitions = 1;
  this->multiple_crtcs = 1;
  this->partitions_are_graphics_cards = 1;
  this->site_restore = 0;
  this->partition_restore = 0;
  this->crtc_restore = 0;
  this->identical_gamma_sizes = 1;
  this->fixed_gamma_size = 0;
  this->fixed_gamma_depth = 1;
  this->real = 1;
  this->fake = 0;
}


/**
 * Initialise an allocated site state
 * 
 * @param   this    The site state to initialise
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. One the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocate on the stack. Note however that it will
 *                  not be free:d if this function fails.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
int libgamma_linux_drm_site_initialise(libgamma_site_state_t* restrict this,
				       char* restrict site)
{
  char pathname[PATH_MAX];
  struct stat _attr;
  
  if (site != NULL)
    return LIBGAMMA_NO_SUCH_SITE;
  
  /* Count the number of available graphics cards by
   * stat:ing there existence in an API filesystem. */
  this->partitions_available = 0;
  for (;;)
    {
      snprintf(pathname, sizeof(pathname) / sizeof(char),
	       DRM_DEV_NAME, DRM_DIR_NAME, (int)(this->partitions_available));
      if (stat(pathname, &_attr))
	break;
      if (this->partitions_available++ > INT_MAX)
	return LIBGAMMA_IMPOSSIBLE_AMOUNT;
    }
  return 0;
}


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void libgamma_linux_drm_site_destroy(libgamma_site_state_t* restrict this)
{
  (void) this;
}


/**
 * Restore the gamma ramps all CRTCS with a site to the system settings
 * 
 * @param   this  The site state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_linux_drm_site_restore(libgamma_site_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}



/**
 * Initialise an allocated partition state
 * 
 * @param   this       The partition state to initialise
 * @param   site       The site state for the site that the partition belongs to
 * @param   partition  The the index of the partition within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int libgamma_linux_drm_partition_initialise(libgamma_partition_state_t* restrict this,
					    libgamma_site_state_t* restrict site, size_t partition)
{ /* FIXME: This function is too large. */
  int rc = 0;
  libgamma_drm_card_data_t* data;
  char pathname[PATH_MAX];
  
  /* Check for partition index overflow. */
  if (partition > INT_MAX)
    return LIBGAMMA_NO_SUCH_PARTITION;
  
  /* Allocate and initialise graphics card data.  */
  this->data = NULL;
  data = malloc(sizeof(libgamma_drm_card_data_t));
  if (data == NULL)
    return LIBGAMMA_ERRNO_SET;
  data->fd = -1;
  data->res = NULL;
  
  /* Get the pathname for the graphics card. */
  snprintf(pathname, sizeof(pathname) / sizeof(char),
	   DRM_DEV_NAME, DRM_DIR_NAME, (int)partition);
  
  /* Acquire access to the graphics card. */
  data->fd = open(pathname, O_RDWR | O_CLOEXEC);
  if (data->fd < 0)
    {
      if ((errno == ENXIO) || (errno == ENODEV))
	rc = LIBGAMMA_NO_SUCH_PARTITION;
      else if (errno == EACCES)
	{
	  struct stat attr;
	  int r;
	  
	  r = stat(pathname, &attr);
	  rc = LIBGAMMA_NO_SUCH_PARTITION;

#define __test(R, W) ((attr.st_mode & (R | W)) == (R | W))
	  if (r)
	    rc = errno == EACCES ? LIBGAMMA_NO_SUCH_PARTITION : LIBGAMMA_ERRNO_SET;
	  else if ((attr.st_uid == geteuid() && __test(S_IRUSR, S_IWUSR)) ||
		   (attr.st_gid == getegid() && __test(S_IRGRP, S_IWGRP)) ||
		   __test(S_IROTH, S_IWOTH))
	    rc = LIBGAMMA_DEVICE_ACCESS_FAILED;
	  else if (attr.st_gid == 0 /* root group */ || __test(S_IRGRP, S_IWGRP))
	    rc = LIBGAMMA_DEVICE_RESTRICTED;
	  else
	    {
	      gid_t supplemental_groups[NGROUPS_MAX];
	      struct group* group;
	      int i, n;
	      
	      n = getgroups(NGROUPS_MAX, supplemental_groups);
	      if (n < 0)
		{
		  rc = LIBGAMMA_ERRNO_SET;
		  goto fail_data;
		}
	      
	      for (i = 0; i < n; i++)
		if (supplemental_groups[i] == attr.st_gid)
		  break;
	      
	      if (i != n)
		{
		  rc = LIBGAMMA_DEVICE_ACCESS_FAILED;
		  goto fail_data;
		}
	      
	      rc = LIBGAMMA_DEVICE_REQUIRE_GROUP;
	      errno = 0;
	      group = getgrgid(attr.st_gid); /* TODO: Not thread-safe. */
	      libgamma_group_gid = attr.st_gid;
	      libgamma_group_name = group != NULL ? group->gr_name : NULL;
	    }
#undef __test
	}
      else
	rc = LIBGAMMA_ERRNO_SET;
      goto fail_data;
    }
  
  /* Acquire mode resources. */
  data->res = drmModeGetResources(data->fd);
  if (data->res == NULL)
    {
      rc = LIBGAMMA_ACQUIRING_MODE_RESOURCES_FAILED;
      goto fail_fd;
    }
  
  /* Get the number of CRTC:s that are available in the partition. */
  if (data->res->count_crtcs < 0)
    {
      rc = LIBGAMMA_NEGATIVE_CRTC_COUNT;
      goto fail_res;
    }
  this->crtcs_available = (size_t)(data->res->count_crtcs);
  return 0;
  
 fail_res:
  drmModeFreeResources(data->res);
 fail_fd:
  close(data->fd);
 fail_data:
  free(data);
  return rc;
}


/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void libgamma_linux_drm_partition_destroy(libgamma_partition_state_t* restrict this)
{
  libgamma_drm_card_data_t* data = this->data;
  if (data->res != NULL)
    drmModeFreeResources(data->res);
  if (data->fd >= 0)
    close(data->fd);
  free(data);
}


/**
 * Restore the gamma ramps all CRTCS with a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_linux_drm_partition_restore(libgamma_partition_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}



/**
 * Initialise an allocated CRTC state
 * 
 * @param   this       The CRTC state to initialise
 * @param   partition  The partition state for the partition that the CRTC belongs to
 * @param   crtc       The the index of the CRTC within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int libgamma_linux_drm_crtc_initialise(libgamma_crtc_state_t* restrict this,
				       libgamma_partition_state_t* restrict partition, size_t crtc)
{
  libgamma_drm_card_data_t* card;
  uint32_t crtc_id;
  
  if (crtc >= partition->crtcs_available)
    return LIBGAMMA_NO_SUCH_CRTC;
  
  card = partition->data;
  crtc_id = card->res->crtcs[crtc];
  this->data = (void*)(size_t)crtc_id;
  return 0;
}


/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void libgamma_linux_drm_crtc_destroy(libgamma_crtc_state_t* restrict this)
{
  (void) this;
}


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_linux_drm_crtc_restore(libgamma_crtc_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}



/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error. On error refer to the error reports in `this`.
 */
int libgamma_linux_drm_get_crtc_information(libgamma_crtc_information_t* restrict this,
					    libgamma_crtc_state_t* restrict crtc, int32_t fields)
{
}


/**
 * Get current the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_linux_drm_crtc_get_gamma_ramps(libgamma_crtc_state_t* restrict this,
					    libgamma_gamma_ramps_t* restrict ramps)
{
  libgamma_drm_card_data_t* card = this->partition->data;
  int r;
#ifdef DEBUG
  if ((ramps->red_size != ramps->green_size) ||
      (ramps->red_size != ramps->blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  r = drmModeCrtcGetGamma(card->fd, (uint32_t)(this->crtc), (uint32_t)(ramps->red_size),
			  ramps->red, ramps->green, ramps->blue);
  if (r < 0)
    return LIBGAMMA_GAMMA_RAMP_READ_FAILED;
  return 0;
}


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_linux_drm_crtc_set_gamma_ramps(libgamma_crtc_state_t* restrict this,
					    libgamma_gamma_ramps_t ramps)
{
}

