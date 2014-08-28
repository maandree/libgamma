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
#ifndef HAVE_LIBGAMMA_METHOD_DUMMY
# error Compiling gamma-dummy.c without HAVE_LIBGAMMA_METHOD_DUMMY
#endif

#include "gamma-dummy.h"

#include "libgamma-error.h"
#include "libgamma-method.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>



/**
 * Configuration set for the dummy adjustment method.
 */
typedef struct libgamma_dummy_configurations
{
  /**
   * The method's capabilities.
   */
  libgamma_method_capabilities_t capabilities;
  
  /**
   * The adjustment method to use.
   */
  int real_method;
  
  /**
   * The number of sites on the system.
   */
  size_t site_count;
  
  /**
   * The number of paritions on a site before it has been configured.
   */
  size_t default_partition_count;
  
  /**
   * The number of CRTC:s on a paritions before it has been configured.
   */
  size_t default_crtc_count;
  
  /**
   * Whether the sites should be inherited from the real method.
   */
  unsigned inherit_sites : 1;
  
  /**
   * Whether the partitions should be inherited from the real method.
   */
  unsigned inherit_partition_count : 1;
  
  /**
   * Whether the CRTC:s should be inherited from the real method.
   */
  unsigned inherit_crtc_count : 1;
  
  /**
   * When a site has been created, stall until the partition count has
   * been configured.
   */
  unsigned stall_for_partition_count : 1;
  
  /**
   * When a parition has been created, stall until the CRTC count has
   * been configured.
   */
  unsigned stall_for_crtc_count : 1;
  
  /**
   * Methods should stall until the system has been configured
   * unless $LIBGAMMA_DUMMY_STALL is not true.
   */
  unsigned stalled_start : 1;
  
  /**
   * Whether to print what is going on in the phony system.
   */
  unsigned verbose : 1;
  
} libgamma_dummy_configurations_t;


/**
 * Dummy adjustment method internal data for a CRTC.
 */
typedef struct libgamma_dummy_crtc
{
  /**
   * The gamma ramp for the red channel.
   */
  void* restrict gamma_red;
  
  /**
   * The gamma ramp for the green channel.
   */
  void* restrict gamma_green;
  
  /**
   * The gamma ramp for the blue channel.
   */
  void* restrict gamma_blue;
  
  /**
   * The number of stops in the gamma ramps.
   */
  size_t gamma_size;
  
  /**
   * The depth of the gamma ramps, -1 for `float`, -2 for `double`.
   */
  int gamma_depth;
  
  /**
   * CRTC state that contains this information.
   */
  libgamma_crtc_state_t* state;
  
} libgamma_dummy_crtc_t;


/**
 * Dummy adjustment method internal data for a partition.
 */
typedef struct libgamma_dummy_partition
{
  /**
   * The CRTC:s on the system.
   */
  libgamma_dummy_crtc_t* crtcs;
  
  /**
   * The number of CRTC:s on the system.
   */
  size_t crtc_count;
  
  /**
   * Partition state that contains this information.
   */
  libgamma_partition_state_t* state;
  
} libgamma_dummy_partition_t;


/**
 * Dummy adjustment method internal data for a site.
 */
typedef struct libgamma_dummy_site
{
  /**
   * The partitions on the system.
   */
  libgamma_dummy_partition_t* partitions;
  
  /**
   * The number of partitions on the system.
   */
  size_t partition_count;
  
  /**
   * Site state that contains this information.
   */
  libgamma_site_state_t* state;
  
} libgamma_dummy_site_t;



/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * and ignore the method's capabilities.
 * 
 * @param   this  The CRTC data
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
static int libgamma_dummy_crtc_restore_forced(libgamma_dummy_crtc_t* restrict data);



/**
 * Configurations for the dummy adjustment method.
 */
static libgamma_dummy_configurations_t libgamma_dummy_configurations =
  {
    .capabilities =
      {
	.crtc_information = 0, /* TODO */
	.default_site_known = 1,
	.multiple_sites = 1,
	.multiple_partitions = 1,
	.multiple_crtcs = 1,
	.partitions_are_graphics_cards = 1,
	.site_restore = 1,
	.partition_restore = 1,
	.crtc_restore = 1,
	.identical_gamma_sizes = 0,
	.fixed_gamma_size = 0,
	.fixed_gamma_depth = 0,
	.real = 0,
	.fake = 0
      },
    .real_method = LIBGAMMA_METHOD_DUMMY,
    .site_count = 2,
    .default_partition_count = 2,
    .default_crtc_count = 2,
    .inherit_sites = 1,
    .inherit_partition_count = 1,
    .inherit_crtc_count = 1,
    .stall_for_partition_count = 0,
    .stall_for_crtc_count = 0,
    .stalled_start = 1,
    .verbose = 0
  };



/**
 * Return the capabilities of the adjustment method.
 * 
 * @param  this  The data structure to fill with the method's capabilities.
 */
void libgamma_dummy_method_capabilities(libgamma_method_capabilities_t* restrict this)
{
  *this = libgamma_dummy_configurations.capabilities;
  this->real = libgamma_dummy_configurations.real_method != LIBGAMMA_METHOD_DUMMY;
  this->fake = this->real;
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
int libgamma_dummy_site_initialise(libgamma_site_state_t* restrict this,
				   char* restrict site)
{
  libgamma_dummy_site_t* data = NULL;
  size_t i, sites, crtcs;
  
  sites = libgamma_dummy_configurations.site_count;
  if (libgamma_dummy_configurations.capabilities.multiple_sites == 0)
    sites = sites == 0 ? 0 : 1;
  this->data = NULL;
  
  if ((site != NULL) && (*site) && ((atoll(site) < 0) || (sites <= (unsigned long long)atoll(site))))
    return LIBGAMMA_NO_SUCH_SITE;
  
  data = malloc(sizeof(libgamma_dummy_site_t));
  if (data == NULL)
    goto fail;
  
  this->data = data;
  data->state = this;
  
  data->partition_count = libgamma_dummy_configurations.default_partition_count;
  if (libgamma_dummy_configurations.capabilities.multiple_partitions == 0)
    data->partition_count = data->partition_count == 0 ? 0 : 1;
  
  crtcs = libgamma_dummy_configurations.default_crtc_count;
  if (libgamma_dummy_configurations.capabilities.multiple_crtcs == 0)
    crtcs = crtcs == 0 ? 0 : 1;
  
  data->partitions = malloc(data->partition_count * sizeof(libgamma_dummy_partition_t));
  if (data->partitions == NULL)
    goto fail;
  
  for (i = 0; i < data->partition_count; i++)
    data->partitions[i].crtc_count = crtcs;
  
  return 0;
  
 fail:
  free(data);
  this->data = NULL;
  return LIBGAMMA_ERRNO_SET;
}


/**
 * Release all resources held by a site state.
 * 
 * @param  this  The site state.
 */
void libgamma_dummy_site_destroy(libgamma_site_state_t* restrict this)
{
  libgamma_dummy_site_t* data = this->data;
  if (data == NULL)
    return;
  
  free(data->partitions);
  free(data);
}


/**
 * Restore the gamma ramps all CRTC:s with a site to the system settings.
 * 
 * @param   this  The site state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_dummy_site_restore(libgamma_site_state_t* restrict this)
{
  libgamma_dummy_site_t* data = this->data;
  size_t i, j;
  
  if (libgamma_dummy_configurations.capabilities.site_restore == 0)
    return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
  
  for (j = 0; j < data->partition_count; j++)
    for (i = 0; i < data->partitions[j].crtc_count; i++)
      if (libgamma_dummy_crtc_restore_forced(data->partitions[j].crtcs + i) < 0)
	return -1;
  
  return 0;
}



/**
 * Initialise an allocated partition state.
 * 
 * @param   this       The partition state to initialise.
 * @param   site       The site state for the site that the partition belongs to
 * @param   partition  The the index of the partition within the site.
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library.
 */
int libgamma_dummy_partition_initialise(libgamma_partition_state_t* restrict this,
					libgamma_site_state_t* restrict site, size_t partition)
{
  libgamma_dummy_site_t* site_data = site->data;
  libgamma_dummy_partition_t* data = site_data->partitions + partition;
  size_t i;
  
  this->data = NULL;
  
  if (partition >= site_data->partition_count)
    return LIBGAMMA_NO_SUCH_PARTITION;
  
  this->data = data;
  data->state = this;
  
  data->crtcs = malloc(data->crtc_count * sizeof(libgamma_dummy_crtc_t));
  if (data->crtcs == NULL)
    goto fail;
  for (i = 0; i < data->crtc_count; i++)
    {
      data->crtcs[i].gamma_size = 2048;
      data->crtcs[i].gamma_size = 64;
    }
  
  return 0;
  
 fail:
  free(data->crtcs);
  data->crtcs = NULL;
  return LIBGAMMA_ERRNO_SET;
}


/**
 * Release all resources held by a partition state.
 * 
 * @param  this  The partition state.
 */
void libgamma_dummy_partition_destroy(libgamma_partition_state_t* restrict this)
{
  libgamma_dummy_partition_t* data = this->data;
  if (data == NULL)
    return;
  
  free(data->crtcs);
  data->crtcs = NULL;
}


/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings.
 * 
 * @param   this  The partition state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_dummy_partition_restore(libgamma_partition_state_t* restrict this)
{
  libgamma_dummy_partition_t* data = this->data;
  size_t i;
  
  if (libgamma_dummy_configurations.capabilities.partition_restore == 0)
    return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
  
  for (i = 0; i < data->crtc_count; i++)
    if (libgamma_dummy_crtc_restore_forced(data->crtcs + i) < 0)
      return -1;
  
  return 0;
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
int libgamma_dummy_crtc_initialise(libgamma_crtc_state_t* restrict this,
				   libgamma_partition_state_t* restrict partition, size_t crtc)
{
  libgamma_dummy_partition_t* partition_data = partition->data;
  libgamma_dummy_crtc_t* data = partition_data->crtcs + crtc;
  size_t stop_size;
  
  this->data = NULL;
  
  if (crtc >= partition_data->crtc_count)
    return LIBGAMMA_NO_SUCH_CRTC;
  
  this->data = data;
  data->state = this;
  
  if (data->gamma_depth == -1)
    stop_size = sizeof(float);
  else if (data->gamma_depth == -2)
    stop_size = sizeof(double);
  else
    stop_size = (size_t)(data->gamma_depth) / 8;
  
  data->gamma_red   = malloc(3 * data->gamma_size * stop_size);
  data->gamma_green = ((char*)(data->gamma_red))   + data->gamma_size * stop_size;
  data->gamma_blue  = ((char*)(data->gamma_green)) + data->gamma_size * stop_size;
  
  if (data->gamma_red == NULL)
    goto fail;
  
  return libgamma_dummy_crtc_restore_forced(data);
  
 fail:
  free(data->gamma_red);
  data->gamma_red   = NULL;
  data->gamma_green = NULL;
  data->gamma_blue  = NULL;
  return LIBGAMMA_ERRNO_SET;
}


/**
 * Release all resources held by a CRTC state.
 * 
 * @param  this  The CRTC state.
 */
void libgamma_dummy_crtc_destroy(libgamma_crtc_state_t* restrict this)
{
  libgamma_dummy_crtc_t* data = this->data;
  if (data == NULL)
    return;
  
  free(data->gamma_red);
  data->gamma_red   = NULL;
  data->gamma_green = NULL;
  data->gamma_blue  = NULL;
}


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC.
 * 
 * @param   this  The CRTC state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_dummy_crtc_restore(libgamma_crtc_state_t* restrict this)
{
  if (libgamma_dummy_configurations.capabilities.crtc_restore == 0)
    return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
  
  return libgamma_dummy_crtc_restore_forced(this->data);
}


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * and ignore the method's capabilities.
 * 
 * @param   this  The CRTC data
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
static int libgamma_dummy_crtc_restore_forced(libgamma_dummy_crtc_t* restrict data)
{
  size_t i, n = data->gamma_size;
  if (data->gamma_red == NULL)
    return 0;
  
  if (data->gamma_depth == 8)
    {
      int8_t* red   = data->gamma_red;
      int8_t* green = data->gamma_green;
      int8_t* blue  = data->gamma_blue;
      double max = (double)INT8_MAX;
      for (i = 0; i < n; i++)
	red[i] = green[i] = blue[i] =
	  (int8_t)(max * ((double)i / (double)(n - 1)));
    }
  else if (data->gamma_depth == 16)
    {
      int16_t* red   = data->gamma_red;
      int16_t* green = data->gamma_green;
      int16_t* blue  = data->gamma_blue;
      double max = (double)INT16_MAX;
      for (i = 0; i < n; i++)
	red[i] = green[i] = blue[i] =
	  (int16_t)(max * ((double)i / (double)(n - 1)));
    }
  else if (data->gamma_depth == 32)
    {
      int32_t* red   = data->gamma_red;
      int32_t* green = data->gamma_green;
      int32_t* blue  = data->gamma_blue;
      double max = (double)INT32_MAX;
      for (i = 0; i < n; i++)
	red[i] = green[i] = blue[i] =
	  (int32_t)(max * ((double)i / (double)(n - 1)));
    }
  else if (data->gamma_depth == 64)
    {
      int64_t* red   = data->gamma_red;
      int64_t* green = data->gamma_green;
      int64_t* blue  = data->gamma_blue;
      double max = (double)INT64_MAX;
      for (i = 0; i < n; i++)
	red[i] = green[i] = blue[i] =
	  (int64_t)(max * ((double)i / (double)(n - 1)));
    }
  else if (data->gamma_depth == -1)
    {
      float* red   = data->gamma_red;
      float* green = data->gamma_green;
      float* blue  = data->gamma_blue;
      for (i = 0; i < n; i++)
	red[i] = green[i] = blue[i] =
	  (float)((double)i / (double)(n - 1));
    }
  else
    {
      double* red   = data->gamma_red;
      double* green = data->gamma_green;
      double* blue  = data->gamma_blue;
      for (i = 0; i < n; i++)
	red[i] = green[i] = blue[i] =
	  (double)i / (double)(n - 1);
    }
  
  return 0;
}



/**
 * Read information about a CRTC.
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC.
 * @param   crtc    The state of the CRTC whose information should be read.
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read.
 * @return          Zero on success, -1 on error. On error refer to the error reports in `this`.
 */
int libgamma_dummy_get_crtc_information(libgamma_crtc_information_t* restrict this,
					libgamma_crtc_state_t* restrict crtc, int32_t fields)
{
}


/**
 * Get current the gamma ramps for a CRTC, 16-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_dummy_crtc_get_gamma_ramps(libgamma_crtc_state_t* restrict this,
					libgamma_gamma_ramps_t* restrict ramps)
{
}


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_dummy_crtc_set_gamma_ramps(libgamma_crtc_state_t* restrict this,
					libgamma_gamma_ramps_t ramps)
{
}



/**
 * Get current the gamma ramps for a CRTC, 32-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_dummy_crtc_get_gamma_ramps32(libgamma_crtc_state_t* restrict this,
					  libgamma_gamma_ramps32_t* restrict ramps)
{
}


/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_dummy_crtc_set_gamma_ramps32(libgamma_crtc_state_t* restrict this,
					  libgamma_gamma_ramps32_t ramps)
{
}



/**
 * Get current the gamma ramps for a CRTC, 64-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_dummy_crtc_get_gamma_ramps64(libgamma_crtc_state_t* restrict this,
					  libgamma_gamma_ramps64_t* restrict ramps)
{
}


/**
 * Set the gamma ramps for a CRTC, 64-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_dummy_crtc_set_gamma_ramps64(libgamma_crtc_state_t* restrict this,
					  libgamma_gamma_ramps64_t ramps)
{
}



/**
 * Get current the gamma ramps for a CRTC, `float` version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_dummy_crtc_get_gamma_rampsf(libgamma_crtc_state_t* restrict this,
					 libgamma_gamma_rampsf_t* restrict ramps)
{
}


/**
 * Set the gamma ramps for a CRTC, `float` version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_dummy_crtc_set_gamma_rampsf(libgamma_crtc_state_t* restrict this,
					 libgamma_gamma_rampsf_t ramps)
{
}


/**
 * Get current the gamma ramps for a CRTC, `double` version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_dummy_crtc_get_gamma_rampsd(libgamma_crtc_state_t* restrict this,
					 libgamma_gamma_rampsd_t* restrict ramps)
{
}


/**
 * Set the gamma ramps for a CRTC, `double` version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_dummy_crtc_set_gamma_rampsd(libgamma_crtc_state_t* restrict this,
					 libgamma_gamma_rampsd_t ramps)
{
}

