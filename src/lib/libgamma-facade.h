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
#ifndef LIBGAMMA_FACADE_H
#define LIBGAMMA_FACADE_H

#if !defined(LIBGAMMA_CONFIG_H) && !defined(DEBUG)
# error libgamma-facade.h should not be included directly, include libgamma.h instead
#endif


#include "libgamma-method.h"

#include <stddef.h>
#include <stdint.h>



/**
 * Mapping function from [0, 1] float encoding value to [0, 2¹⁶ − 1] integer output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 2¹⁶ − 1] integer output value.
 */
typedef uint16_t libgamma_gamma_ramps_fun(float encoding);

/**
 * Mapping function from [0, 1] float encoding value to [0, 2³² − 1] integer output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 2³² − 1] integer output value.
 */
typedef uint32_t libgamma_gamma_ramps32_fun(float encoding);

/**
 * Mapping function from [0, 1] float encoding value to [0, 2⁶⁴ − 1] integer output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 2⁶⁴ − 1] integer output value.
 */
typedef uint64_t libgamma_gamma_ramps64_fun(float encoding);

/**
 * Mapping function from [0, 1] float encoding value to [0, 1] float output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 1] float output value.
 */
typedef float libgamma_gamma_rampsf_fun(float encoding);

/**
 * Mapping function from [0, 1] double precision float encoding
 * value to [0, 1] double precision float output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 1] float output value.
 */
typedef double libgamma_gamma_rampsd_fun(double encoding);



/**
 * List available adjustment methods by their order of preference based on the environment.
 * 
 * @param  methods    Output array of methods, should be able to hold `LIBGAMMA_METHOD_COUNT` elements.
 * @param  buf_size   The number of elements that fits in `methods`, it should be `LIBGAMMA_METHOD_COUNT`,
 *                    This is used to avoid writing outside the output buffer if this library adds new
 *                    adjustment methods without the users of the library recompiling.
 * @param  operation  Allowed values:
 *                      0: Methods that the environment suggests will work, excluding fake.
 *                      1: Methods that the environment suggests will work, including fake.
 *                      2: All real non-fake methods.
 *                      3: All real methods.
 *                      4: All methods.
 *                    Other values invoke undefined behaviour.
 * @return            The number of element that have been stored in `methods`, or should
 *                    have been stored if the buffer was large enough.
 */
size_t libgamma_list_methods(int* restrict methods, size_t buf_size, int operation);

/**
 * Check whether an adjustment method is available, non-existing (invalid) methods will be
 * identified as not available under the rationale that the library may be out of date.
 * 
 * @param   method  The adjustment method.
 * @return          Whether the adjustment method is available.
 */
int libgamma_is_method_available(int method) __attribute__((const));

/**
 * Return the capabilities of an adjustment method.
 * 
 * @param  this    The data structure to fill with the method's capabilities
 * @param  method  The adjustment method (display server and protocol).
 */
void libgamma_method_capabilities(libgamma_method_capabilities_t* restrict this, int method);

/**
 * Return the capabilities of an adjustment method.
 * 
 * @param   method  The adjustment method (display server and protocol.)
 * @return          The default site, `NULL` if it cannot be determined or
 *                  if multiple sites are not supported by the adjustment
 *                  method. This value should not be `free`:d.
 */
char* libgamma_method_default_site(int method);

/**
 * Return the capabilities of an adjustment method.
 * 
 * @param   method  The adjustment method (display server and protocol.)
 * @return          The environ variables that is used to determine the
 *                  default site. `NULL` if there is none, that is, if
 *                  the method does not support multiple sites.
 *                  This value should not be `free`:d.
 */
const char* libgamma_method_default_site_variable(int method) __attribute__((const));


/**
 * Initialise an allocated site state.
 * 
 * @param   this    The site state to initialise.
 * @param   method  The adjustment method (display server and protocol.)
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. One the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocate on the stack. Note however that it will
 *                  not be `free`:d if this function fails.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library.
 */
int libgamma_site_initialise(libgamma_site_state_t* restrict this,
			     int method, char* restrict site);

/**
 * Release all resources held by a site state.
 * 
 * @param  this  The site state.
 */
void libgamma_site_destroy(libgamma_site_state_t* restrict this);

/**
 * Release all resources held by a site state
 * and free the site state pointer.
 * 
 * @param  this  The site state.
 */
void libgamma_site_free(libgamma_site_state_t* restrict this);

/**
 * Restore the gamma ramps all CRTC:s with a site to the system settings.
 * 
 * @param   this  The site state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_site_restore(libgamma_site_state_t* restrict this);


/**
 * Initialise an allocated partition state.
 * 
 * @param   this       The partition state to initialise.
 * @param   site       The site state for the site that the partition belongs to.
 * @param   partition  The the index of the partition within the site.
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library.
 */
int libgamma_partition_initialise(libgamma_partition_state_t* restrict this,
				  libgamma_site_state_t* restrict site, size_t partition);

/**
 * Release all resources held by a partition state.
 * 
 * @param  this  The partition state.
 */
void libgamma_partition_destroy(libgamma_partition_state_t* restrict this);

/**
 * Release all resources held by a partition state
 * and free the partition state pointer.
 * 
 * @param  this  The partition state.
 */
void libgamma_partition_free(libgamma_partition_state_t* restrict this);

/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings.
 * 
 * @param   this  The partition state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_partition_restore(libgamma_partition_state_t* restrict this);


/**
 * Initialise an allocated CRTC state.
 * 
 * @param   this       The CRTC state to initialise.
 * @param   partition  The partition state for the partition that the CRTC belongs to.
 * @param   crtc       The the index of the CRTC within the partition.
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library.
 */
int libgamma_crtc_initialise(libgamma_crtc_state_t* restrict this,
			     libgamma_partition_state_t* restrict partition, size_t crtc);

/**
 * Release all resources held by a CRTC state.
 * 
 * @param  this  The CRTC state.
 */
void libgamma_crtc_destroy(libgamma_crtc_state_t* restrict this);

/**
 * Release all resources held by a CRTC state
 * and free the CRTC state pointer.
 * 
 * @param  this  The CRTC state.
 */
void libgamma_crtc_free(libgamma_crtc_state_t* restrict this);

/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC.
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_crtc_restore(libgamma_crtc_state_t* restrict this);


/**
 * Read information about a CRTC.
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC.
 * @param   crtc    The state of the CRTC whose information should be read.
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read.
 * @return          Zero on success, -1 on error. On error refer to the error reports in `this`.
 */
int libgamma_get_crtc_information(libgamma_crtc_information_t* restrict this,
				  libgamma_crtc_state_t* restrict crtc, int32_t fields);

/**
 * Release all resources in an information data structure for a CRTC.
 * 
 * @param  this  The CRTC information.
 */
void libgamma_crtc_information_destroy(libgamma_crtc_information_t* restrict this);

/**
 * Release all resources in an information data structure for a CRTC
 * and free the data structure pointer.
 * 
 * @param  this  The CRTC information.
 */
void libgamma_crtc_information_free(libgamma_crtc_information_t* restrict this);

/**
 * Convert a raw representation of an EDID to a lowercase hexadecimal representation.
 * 
 * @param   edid:const unsigned char*  The EDID in raw representation.
 * @param   length:size_t              The length of `edid`.
 * @return  :char*                     The EDID in lowercase hexadecimal representation,
 *                                     `NULL` on allocation error, `errno` will be set accordingly.
 */
#define libgamma_behex_edid(edid, length)  \
  libgamma_behex_edid_lowercase(edid, length)

/**
 * Convert a raw representation of an EDID to a lowercase hexadecimal representation.
 * 
 * @param   edid    The EDID in raw representation.
 * @param   length  The length of `edid`.
 * @retrun          The EDID in lowercase hexadecimal representation,
 *                  `NULL` on allocation error, `errno` will be set accordingly.
 */
char* libgamma_behex_edid_lowercase(const unsigned char* restrict edid, size_t length);

/**
 * Convert a raw representation of an EDID to an uppercase hexadecimal representation.
 * 
 * @param   edid    The EDID in raw representation.
 * @param   length  The length of `edid`.
 * @retrun          The EDID in uppercase hexadecimal representation,
 *                  NULL` on allocation error, `errno` will be set accordingly.
 */
char* libgamma_behex_edid_uppercase(const unsigned char* restrict edid, size_t length);

/**
 * Convert an hexadecimal representation of an EDID to a raw representation.
 * 
 * @param   edid  The EDID in hexadecimal representation.
 * @retrun        The EDID in raw representation, it will be half the length
 *                of `edid` (the input value). `NULL` on allocation error or
 *                if the EDID is malformated, `errno` will be set accordingly.
 */
unsigned char* libgamma_unhex_edid(const char* restrict edid);


/**
 * Get current the gamma ramps for a CRTC, 16-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_crtc_get_gamma_ramps(libgamma_crtc_state_t* restrict this,
				  libgamma_gamma_ramps_t* restrict ramps);

/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_crtc_set_gamma_ramps(libgamma_crtc_state_t* restrict this,
				  libgamma_gamma_ramps_t ramps) __attribute__((hot));


/**
 * Get current the gamma ramps for a CRTC, 32-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_crtc_get_gamma_ramps32(libgamma_crtc_state_t* restrict this,
				    libgamma_gamma_ramps32_t* restrict ramps);

/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_crtc_set_gamma_ramps32(libgamma_crtc_state_t* restrict this,
				    libgamma_gamma_ramps32_t ramps);


/**
 * Get current the gamma ramps for a CRTC, 64-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_crtc_get_gamma_ramps64(libgamma_crtc_state_t* restrict this,
				    libgamma_gamma_ramps64_t* restrict ramps);

/**
 * Set the gamma ramps for a CRTC, 64-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_crtc_set_gamma_ramps64(libgamma_crtc_state_t* restrict this,
				    libgamma_gamma_ramps64_t ramps);


/**
 * Set the gamma ramps for a CRTC, `float` version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_crtc_set_gamma_rampsf(libgamma_crtc_state_t* restrict this,
				   libgamma_gamma_rampsf_t ramps);

/**
 * Get current the gamma ramps for a CRTC, `float` version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_crtc_get_gamma_rampsf(libgamma_crtc_state_t* restrict this,
				   libgamma_gamma_rampsf_t* restrict ramps);


/**
 * Get current the gamma ramps for a CRTC, `double` version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_crtc_get_gamma_rampsd(libgamma_crtc_state_t* restrict this,
				   libgamma_gamma_rampsd_t* restrict ramps);

/**
 * Set the gamma ramps for a CRTC, `double` version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_crtc_set_gamma_rampsd(libgamma_crtc_state_t* restrict this,
				   libgamma_gamma_rampsd_t ramps);


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth function version.
 * 
 * Note that this will probably involve the library allocating temporary data.
 * 
 * @param   this            The CRTC state.
 * @param   red_function    The function that generates the the gamma ramp for the red channel.
 * @param   green_function  The function that generates the the gamma ramp for the green channel.
 * @param   blue_function   The function that generates the the gamma ramp for the blue channel.
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library.
 */
int libgamma_crtc_set_gamma_ramps_f(libgamma_crtc_state_t* restrict this,
				    libgamma_gamma_ramps_fun* red_function,
				    libgamma_gamma_ramps_fun* green_function,
				    libgamma_gamma_ramps_fun* blue_function) __attribute__((cold));

/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth function version.
 * 
 * Note that this will probably involve the library allocating temporary data.
 * 
 * @param   this            The CRTC state.
 * @param   red_function    The function that generates the the gamma ramp for the red channel.
 * @param   green_function  The function that generates the the gamma ramp for the green channel.
 * @param   blue_function   The function that generates the the gamma ramp for the blue channel.
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library.
 */
int libgamma_crtc_set_gamma_ramps32_f(libgamma_crtc_state_t* restrict this,
				      libgamma_gamma_ramps32_fun* red_function,
				      libgamma_gamma_ramps32_fun* green_function,
				      libgamma_gamma_ramps32_fun* blue_function) __attribute__((cold));

/**
 * Set the gamma ramps for a CRTC, 64-bit gamma-depth function version.
 * 
 * Note that this will probably involve the library allocating temporary data.
 * 
 * @param   this            The CRTC state.
 * @param   red_function    The function that generates the the gamma ramp for the red channel.
 * @param   green_function  The function that generates the the gamma ramp for the green channel.
 * @param   blue_function   The function that generates the the gamma ramp for the blue channel.
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library.
 */
int libgamma_crtc_set_gamma_ramps64_f(libgamma_crtc_state_t* restrict this,
				      libgamma_gamma_ramps64_fun* red_function,
				      libgamma_gamma_ramps64_fun* green_function,
				      libgamma_gamma_ramps64_fun* blue_function) __attribute__((cold));

/**
 * Set the gamma ramps for a CRTC, `float` function version.
 * 
 * Note that this will probably involve the library allocating temporary data.
 * 
 * @param   this            The CRTC state.
 * @param   red_function    The function that generates the the gamma ramp for the red channel.
 * @param   green_function  The function that generates the the gamma ramp for the green channel.
 * @param   blue_function   The function that generates the the gamma ramp for the blue channel.
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library.
 */
int libgamma_crtc_set_gamma_rampsf_f(libgamma_crtc_state_t* restrict this,
				     libgamma_gamma_rampsf_fun* red_function,
				     libgamma_gamma_rampsf_fun* green_function,
				     libgamma_gamma_rampsf_fun* blue_function) __attribute__((cold));

/**
 * Set the gamma ramps for a CRTC, `double` function version.
 * 
 * Note that this will probably involve the library allocating temporary data.
 * 
 * @param   this            The CRTC state.
 * @param   red_function    The function that generates the the gamma ramp for the red channel.
 * @param   green_function  The function that generates the the gamma ramp for the green channel.
 * @param   blue_function   The function that generates the the gamma ramp for the blue channel.
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library.
 */
int libgamma_crtc_set_gamma_rampsd_f(libgamma_crtc_state_t* restrict this,
				     libgamma_gamma_rampsd_fun* red_function,
				     libgamma_gamma_rampsd_fun* green_function,
				     libgamma_gamma_rampsd_fun* blue_function) __attribute__((cold));


#endif

