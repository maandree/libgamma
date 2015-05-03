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
#ifndef HAVE_LIBGAMMA_METHOD_WAYLAND
# error Compiling gamma-wayland.c without HAVE_LIBGAMMA_METHOD_WAYLAND
#endif

#include "gamma-wayland.h"
#ifdef __GCC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wcast-qual"
#endif
#include "wayland-gamma-control-client-protocol.h"
#ifdef __GCC__
# pragma GCC diagnostic pop
#endif

#include "libgamma-error.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#ifdef DEBUG
# include <stdio.h>
#endif

#include <wayland-client.h>



/**
 * This function is called when the display server
 * announces global resources.
 * 
 * @param  site_state  The site state
 * @param  registry    The event handler
 * @param  id          The ID of the resource
 * @param  interface   The name of the resource's type
 * @param  version     The version of the resource's type
 */
static void registry_global_handler(void* site_state, struct wl_registry* registry, uint32_t id,
				    const char* interface, uint32_t version);

/**
 * This function is called when the display server
 * announces that global resources have been removed.
 * 
 * @param  site_state  The site state
 * @param  registry    The event handler
 * @param  id          The ID of the resource
 */
static void registry_global_remover(void* site_state, struct wl_registry* registry, uint32_t id);

/**
 * This function is invoked when the display
 * server tells us the size of a gamma ramp.
 * 
 * @param  crtc_data  The protocol-specific data for the CRTC
 * @param  control    The gamma ramp controll for the CRTC
 * @param  size       The number of stops in the gamma ramps of the CRTC
 */
static void gamma_control_gamma_size(void* crtc, struct gamma_control* control, uint32_t size);



/**
 * Data structure for CRTC data.
 */
typedef struct libgamma_wayland_crtc_data
{
  /**
   * Wayland-representation of the output.
   */
  struct wl_output* output;
  
  /**
   * Gamma controller for the output.
   */
  struct gamma_control* gamma_control;
  
  /**
   * The size of the gamma ramps.
   */
  uint32_t gamma_size;
  
  /**
   * Whether `gamma_size` has been set.
   * 
   */
  int gamma_size_announced;
  
  /**
   * Whether gamma may be supported.
   */
  int gamma_maybe_supported;
  
  /**
   * ID of the CRTC.
   */
  uint32_t global_id;
  
  /**
   * Whether the CRTC has been removed;
   */
  int removed;
  
} libgamma_wayland_crtc_data_t;


/**
 * Data structure for site data.
 */
typedef struct libgamma_wayland_site_data
{
  /**
   * Connection to the display.
   */
  struct wl_display* display;
  
  /**
   * Event handlers for the display.
   */
  struct wl_registry* registry;
  
  /**
   * Gamma controller for the display.
   */
  struct gamma_control_manager* gamma_control_manager;
  
  /**
   * The number of CRTC:s that are available under this
   * partition. Note that the CRTC:s are not necessarily
   * online.
   */
  size_t crtcs_available;
  
  /**
   * The number of CRTC:s that are available under this
   * partition. Note that the CRTC:s are not necessarily
   * online.
   */
  size_t crtcs_allocated;
  
  /**
   * CRTC information fields.
   */
  libgamma_wayland_crtc_data_t** crtcs;
  
  /**
   * Error set from `registry_global_handler`.
   */
  int async_errno;
  
  /**
   * Whether gamma may be supported.
   */
  int gamma_maybe_supported;
  
} libgamma_wayland_site_data_t;


/**
 * Event handlers for the display.
 */
static const struct wl_registry_listener registry_listener =
  {
    registry_global_handler,
    registry_global_remover,
  };

/**
 * Gamma event handlers for CRTC:s.
 */
static const struct gamma_control_listener gamma_control_listener =
  {
    gamma_control_gamma_size,
  };



/**
 * Return the capabilities of the adjustment method.
 * 
 * @param  this  The data structure to fill with the method's capabilities.
 */
void libgamma_wayland_method_capabilities(libgamma_method_capabilities_t* restrict this)
{
  char* display = getenv("WAYLAND_DISPLAY");
  /* TODO */
  this->crtc_information = 0;
  /* Wayland supports multiple sites and CRTC:s, but not paritions. */
  this->default_site_known = (display && *display) ? 1 : 0;
  this->multiple_sites = 1;
  this->multiple_partitions = 0;
  this->multiple_crtcs = 1;
  this->partitions_are_graphics_cards = 0;
  /* Wayland/Orbital has per-CRTC restore capabilities. */
  this->site_restore = 0;
  this->partition_restore = 0;
  this->crtc_restore = 1;
  /* Gamma ramp sizes are identical but not fixed. */
  this->identical_gamma_sizes = 1;
  this->fixed_gamma_size = 0;
  /* Gamma ramp depths are fixed. */
  this->fixed_gamma_depth = 1;
  /* Wayland/Orbital is a real non-faked adjustment method. */
  this->real = 1;
  this->fake = 0;
}


/**
 * This function is called when the display server
 * announces global resources.
 * 
 * @param  site_state  The site state
 * @param  registry    The event handler
 * @param  id          The ID of the resource
 * @param  interface   The name of the resource's type
 * @param  version     The version of the resource's type
 */
static void registry_global_handler(void* site_state, struct wl_registry* registry, uint32_t id,
				    const char* interface, uint32_t version)
{
  libgamma_site_state_t* site = site_state;
  libgamma_wayland_site_data_t* data = site->data;
  libgamma_wayland_crtc_data_t** crtcs = data->crtcs;
  libgamma_wayland_crtc_data_t* crtc;
  size_t new_allocation, i;
  
  (void) version;
  
  /* Stop if an earlier call failed. */
  if (data->async_errno)
    return;
  
  if (!strcmp(interface, "gamma_control_manager"))
    {
      data->gamma_control_manager = wl_registry_bind(registry, id, &gamma_control_manager_interface, 1);
      if (data->gamma_control_manager == NULL)
	data->async_errno = errno;
    }
  else if (!strcmp(interface, "wl_output"))
    {
      /* Check that ID is not used. */
      for (i = 0; i < data->crtcs_available; i++)
	if (crtcs[i] && (crtcs[i]->global_id == id))
	  return;
      
      /* Reallocate CRTC resource array. */
      if (data->crtcs_available == data->crtcs_allocated)
	{
	  new_allocation = data->crtcs_allocated ? (data->crtcs_allocated << 1) : 1;
	  crtcs = realloc(crtcs, new_allocation * sizeof(libgamma_wayland_crtc_data_t*));
	  if (crtcs == NULL)
	    {
	      data->async_errno = errno;
	      return;
	    }
	  data->crtcs = crtcs;
	  data->crtcs_allocated = new_allocation;
	}
      
      /* Allocate CRTC resource. */
      crtc = calloc(1, sizeof(libgamma_wayland_crtc_data_t));
      crtcs[data->crtcs_available] = crtc;
      if (crtc == NULL)
	{
	  data->async_errno = errno;
	  return;
	}
      
      /* Set CRTC data. */
      crtc->global_id = id;
      crtc->output = wl_registry_bind(registry, id, &wl_output_interface, 1);
      crtc->removed = 0;
      
      /* Increase CRTC cound and check for overflow. */
      if (data->crtcs_available++ == (size_t)~0)
	{
	  free(crtc);
	  data->async_errno = LIBGAMMA_IMPOSSIBLE_AMOUNT;
	  return;
	}
    }
}


/**
 * This function is called when the display server
 * announces that global resources have been removed.
 * 
 * @param  site_state  The site state
 * @param  registry    The event handler
 * @param  id          The ID of the resource
 */
static void registry_global_remover(void* site_state, struct wl_registry* registry, uint32_t id)
{
  libgamma_site_state_t* site = site_state;
  libgamma_wayland_site_data_t* data = site->data;
  libgamma_wayland_crtc_data_t* crtc;
  size_t i;
  
  (void) registry;
  
  for (i = 0; i < data->crtcs_available; i++)
    if ((crtc = data->crtcs[i]))
      if (crtc->global_id == id)
	{
	  crtc->removed = 1;
	  break;
	}
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
int libgamma_wayland_site_initialise(libgamma_site_state_t* restrict this,
				     char* restrict site)
{
  libgamma_wayland_site_data_t* data;
  
  /* Allocate adjustment method dependent data memory area.
     We use `calloc` because we want `data`'s pointers to be `NULL` if not allocated at `fail`. */
  if ((this->data = data = calloc(1, sizeof(libgamma_wayland_site_data_t))) == NULL)
    return LIBGAMMA_ERRNO_SET;
  
  /* Connect to the display server. */
  data->display = wl_display_connect(site);
  if (data->display == NULL)
    return LIBGAMMA_OPEN_SITE_FAILED;
  data->registry = wl_display_get_registry(data->display);
  if (data->registry == NULL)
    return LIBGAMMA_ERRNO_SET;
  if (wl_registry_add_listener(data->registry, &registry_listener, this) < 0)
    return LIBGAMMA_ERRNO_SET;
  
  /* Synchronise with display. */
  if (wl_display_roundtrip(data->display) < 0)
    return LIBGAMMA_ERRNO_SET;
  
  /* Check for errors. */
  if (data->async_errno > 0)
    return errno = data->async_errno, LIBGAMMA_ERRNO_SET;
  if (data->async_errno < 0)
    return data->async_errno;
  
  /* Check that we have a gamma controller. */
  if (data->gamma_control_manager)
    data->gamma_maybe_supported = 1;
  
  this->partitions_available = 1;
  return 0;
}


/**
 * Release all resources held by a site state.
 * 
 * @param  this  The site state.
 */
void libgamma_wayland_site_destroy(libgamma_site_state_t* restrict this)
{
  libgamma_wayland_site_data_t* data = this->data;
  libgamma_wayland_crtc_data_t* crtc;
  gamma_control_manager_destroy(data->gamma_control_manager);
  wl_registry_destroy(data->registry);
  wl_display_disconnect(data->display);
  while (data->crtcs_available--)
    {
      crtc = data->crtcs[data->crtcs_available];
      gamma_control_destroy(crtc->gamma_control);
      wl_output_destroy(crtc->output);
      free(crtc);
    }
  free(data->crtcs);
  free(data);
}


/**
 * Restore the gamma ramps all CRTC:s with a site to the system settings.
 * 
 * @param   this  The site state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_wayland_site_restore(libgamma_site_state_t* restrict this)
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
int libgamma_wayland_partition_initialise(libgamma_partition_state_t* restrict this,
					  libgamma_site_state_t* restrict site, size_t partition)
{
  libgamma_wayland_site_data_t* data = site->data;
  
  this->data = NULL;
  this->crtcs_available = 0;
  
  if (partition != 0)
    return LIBGAMMA_NO_SUCH_PARTITION;
  
  this->crtcs_available = data->crtcs_available;
  return 0;
}


/**
 * Release all resources held by a partition state.
 * 
 * @param  this  The partition state.
 */
void libgamma_wayland_partition_destroy(libgamma_partition_state_t* restrict this)
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
int libgamma_wayland_partition_restore(libgamma_partition_state_t* restrict this)
{
  return libgamma_wayland_site_restore(this->site);
}


/**
 * This function is invoked when the display
 * server tells us the size of a gamma ramp.
 * 
 * @param  crtc_data  The protocol-specific data for the CRTC
 * @param  control    The gamma ramp controll for the CRTC
 * @param  size       The number of stops in the gamma ramps of the CRTC
 */
static void gamma_control_gamma_size(void* crtc_data, struct gamma_control* control, uint32_t size)
{
  libgamma_wayland_crtc_data_t* data = crtc_data;
  (void) control;
  data->gamma_size = size;
  data->gamma_size_announced = 1;
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
int libgamma_wayland_crtc_initialise(libgamma_crtc_state_t* restrict this,
				     libgamma_partition_state_t* restrict partition, size_t crtc)
{
  libgamma_wayland_crtc_data_t* data;
  libgamma_wayland_site_data_t* site = partition->site->data;
  
  this->data = NULL;
  
  if (crtc >= partition->crtcs_available)
    return LIBGAMMA_NO_SUCH_CRTC;
  
  data = site->crtcs[crtc];
  
  data->gamma_control = gamma_control_manager_get_gamma_control(site->gamma_control_manager, data->output);
  if (data->gamma_control)
    {
      data->gamma_maybe_supported = 1;
      gamma_control_add_listener(data->gamma_control, &gamma_control_listener, data);
    }
  if (wl_display_roundtrip(site->display) < 0)
    return LIBGAMMA_ERRNO_SET;
  if (data->gamma_size_announced == 0)
    data->gamma_maybe_supported = 0;
  else if (data->gamma_size < 2)
    return LIBGAMMA_SINGLETON_GAMMA_RAMP;
  
  this->data = data;
  
  return 0;
}


/**
 * Release all resources held by a CRTC state.
 * 
 * @param  this  The CRTC state.
 */
void libgamma_wayland_crtc_destroy(libgamma_crtc_state_t* restrict this)
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
int libgamma_wayland_crtc_restore(libgamma_crtc_state_t* restrict this)
{
  libgamma_wayland_crtc_data_t* data = this->data;
  libgamma_wayland_site_data_t* site = this->partition->site->data;
  gamma_control_reset_gamma(data->gamma_control);
  if (wl_display_flush(site->display) < 0)
    return LIBGAMMA_ERRNO_SET;
  return 0;
}



# pragma GCC diagnostic ignored "-Wsuggest-attribute=const" /* TODO */
/**
 * Read information about a CRTC.
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC.
 * @param   crtc    The state of the CRTC whose information should be read.
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read.
 * @return          Zero on success, -1 on error. On error refer to the error reports in `this`..
 */
int libgamma_wayland_get_crtc_information(libgamma_crtc_information_t* restrict this,
					  libgamma_crtc_state_t* restrict crtc, int32_t fields)
{
  return 0; (void) this, (void) crtc, (void) fields; /* TODO */
}


/**
 * Get the current gamma ramps for a CRTC, 16-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_wayland_crtc_get_gamma_ramps16(libgamma_crtc_state_t* restrict this,
					    libgamma_gamma_ramps16_t* restrict ramps)
{
  (void) this;
  (void) ramps;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_wayland_crtc_set_gamma_ramps16(libgamma_crtc_state_t* restrict this,
					    libgamma_gamma_ramps16_t ramps)
{
  libgamma_wayland_site_data_t* site = this->partition->site->data;
  libgamma_wayland_crtc_data_t* crtc = this->data;
  struct wl_array red;
  struct wl_array green;
  struct wl_array blue;
  
  /* TODO Verify that gamma can be set. */
  
#ifdef DEBUG
  /* Gamma ramp sizes are identical but not fixed. */
  if ((ramps.red_size != ramps.green_size) ||
      (ramps.red_size != ramps.blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  
  /* Translate gamma ramps to Wayland structure. */
  red  .size = red  .alloc = ramps.  red_size * sizeof(uint16_t);
  green.size = green.alloc = ramps.green_size * sizeof(uint16_t);
  blue .size = blue .alloc = ramps. blue_size * sizeof(uint16_t);
  red  .data = ramps.red;
  green.data = ramps.green;
  blue .data = ramps.blue;
  
  /* Apply gamma ramps. */
  gamma_control_set_gamma(crtc->gamma_control, &red, &green, &blue);
  wl_display_flush(site->display);
  
  /* TODO Check for errors. */
  
  return 0;
}

