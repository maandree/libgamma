/* See LICENSE file for copyright and license details. */
#ifndef LIBGAMMA_FACADE_H
#define LIBGAMMA_FACADE_H

#if !defined(LIBGAMMA_CONFIG_H) && !defined(DEBUG)
# error libgamma-facade.h should not be included directly, include libgamma.h instead
#endif


#include "libgamma-method.h"

#include <stddef.h>
#include <stdint.h>

#ifndef __GNUC__
# define __attribute__(x)
#endif



/**
 * Mapping function from [0, 1] float encoding value to [0, 2⁸ − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2⁸ − 1] integer output value
 */
typedef uint8_t libgamma_gamma_ramps8_fun(float);

/**
 * Mapping function from [0, 1] float encoding value to [0, 2¹⁶ − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2¹⁶ − 1] integer output value
 */
typedef uint16_t libgamma_gamma_ramps16_fun(float);

/**
 * Mapping function from [0, 1] float encoding value to [0, 2³² − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2³² − 1] integer output value
 */
typedef uint32_t libgamma_gamma_ramps32_fun(float);

/**
 * Mapping function from [0, 1] float encoding value to [0, 2⁶⁴ − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2⁶⁴ − 1] integer output value
 */
typedef uint64_t libgamma_gamma_ramps64_fun(float);

/**
 * Mapping function from [0, 1] float encoding value to [0, 1] float output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 1] float output value
 */
typedef float libgamma_gamma_rampsf_fun(float);

/**
 * Mapping function from [0, 1] double precision float encoding
 * value to [0, 1] double precision float output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 1] float output value
 */
typedef double libgamma_gamma_rampsd_fun(double);



/**
 * List available adjustment methods by their order of preference based on the environment
 * 
 * @param  methods    Output array of methods, should be able to hold `LIBGAMMA_METHOD_COUNT` elements
 * @param  buf_size   The number of elements that fits in `methods`, it should be `LIBGAMMA_METHOD_COUNT`,
 *                    This is used to avoid writing outside the output buffer if this library adds new
 *                    adjustment methods without the users of the library recompiling
 * @param  operation  Allowed values:
 *                      0: Methods that the environment suggests will work, excluding fake
 *                      1: Methods that the environment suggests will work, including fake
 *                      2: All real non-fake methods
 *                      3: All real methods
 *                      4: All methods
 *                    Other values invoke undefined behaviour
 * @return            The number of element that have been stored in `methods`, or should
 *                    have been stored if the buffer was large enough
 */
size_t libgamma_list_methods(int *restrict, size_t, int);

/**
 * Check whether an adjustment method is available, non-existing (invalid) methods will be
 * identified as not available under the rationale that the library may be out of date
 * 
 * @param   method  The adjustment method
 * @return          Whether the adjustment method is available
 */
int libgamma_is_method_available(int) __attribute__((const));

/**
 * Return the capabilities of an adjustment method
 * 
 * @param  this    The data structure to fill with the method's capabilities
 * @param  method  The adjustment method (display server and protocol)
 */
void libgamma_method_capabilities(libgamma_method_capabilities_t *restrict, int);

/**
 * Return the default site for an adjustment method
 * 
 * @param   method  The adjustment method (display server and protocol)
 * @return          The default site, `NULL` if it cannot be determined or
 *                  if multiple sites are not supported by the adjustment
 *                  method; this value should not be `free`:d
 */
char *libgamma_method_default_site(int);

/**
 * Return the default variable that determines
 * the default site for an adjustment method
 * 
 * @param   method  The adjustment method (display server and protocol)
 * @return          The environ variables that is used to determine the
 *                  default site, `NULL` if there is none, that is, if
 *                  the method does not support multiple sites; this
 *                  value should not be `free`:d
 */
const char *libgamma_method_default_site_variable(int) __attribute__((const));


/**
 * Initialise an allocated site state
 * 
 * @param   this    The site state to initialise
 * @param   method  The adjustment method (display server and protocol)
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. Once the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocate on the stack. Note however that it will
 *                  not be `free`:d if this function fails.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
int libgamma_site_initialise(libgamma_site_state_t *restrict, int, char *restrict);

/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void libgamma_site_destroy(libgamma_site_state_t *restrict);

/**
 * Release all resources held by a site state
 * and free the site state pointer
 * 
 * @param  this  The site state
 */
void libgamma_site_free(libgamma_site_state_t *restrict);

/**
 * Restore the gamma ramps all CRTC:s within a site to the system settings
 * 
 * @param   this  The site state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_site_restore(libgamma_site_state_t *restrict);


/**
 * Initialise an allocated partition state
 * 
 * @param   this       The partition state to initialise
 * @param   site       The site state for the site that the partition belongs to
 * @param   partition  The index of the partition within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int libgamma_partition_initialise(libgamma_partition_state_t *restrict, libgamma_site_state_t *restrict, size_t);

/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void libgamma_partition_destroy(libgamma_partition_state_t *restrict);

/**
 * Release all resources held by a partition state
 * and free the partition state pointer
 * 
 * @param  this  The partition state
 */
void libgamma_partition_free(libgamma_partition_state_t *restrict);

/**
 * Restore the gamma ramps all CRTC:s within a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_partition_restore(libgamma_partition_state_t *restrict);


/**
 * Initialise an allocated CRTC state
 * 
 * @param   this       The CRTC state to initialise
 * @param   partition  The partition state for the partition that the CRTC belongs to
 * @param   crtc       The index of the CRTC within the partition
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int libgamma_crtc_initialise(libgamma_crtc_state_t *restrict, libgamma_partition_state_t *restrict, size_t);

/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void libgamma_crtc_destroy(libgamma_crtc_state_t *restrict);

/**
 * Release all resources held by a CRTC state
 * and free the CRTC state pointer
 * 
 * @param  this  The CRTC state
 */
void libgamma_crtc_free(libgamma_crtc_state_t *restrict);

/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_crtc_restore(libgamma_crtc_state_t *restrict);


/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error; on error refer to the error reports in `this`
 */
int libgamma_get_crtc_information(libgamma_crtc_information_t *restrict, libgamma_crtc_state_t *restrict, int32_t);

/**
 * Release all resources in an information data structure for a CRTC
 * 
 * @param  this  The CRTC information
 */
void libgamma_crtc_information_destroy(libgamma_crtc_information_t *restrict);

/**
 * Release all resources in an information data structure for a CRTC
 * and free the data structure pointer
 * 
 * @param  this  The CRTC information
 */
void libgamma_crtc_information_free(libgamma_crtc_information_t *restrict);

/**
 * Convert a raw representation of an EDID to a lowercase hexadecimal representation
 * 
 * @param   edid:const unsigned char*  The EDID in raw representation
 * @param   length:size_t              The length of `edid`
 * @return  :char*                     The EDID in lowercase hexadecimal representation,
 *                                     `NULL` on allocation error, `errno` will be set accordingly
 */
#define libgamma_behex_edid(edid, length) libgamma_behex_edid_lowercase((edid), (length))

/**
 * Convert a raw representation of an EDID to a lowercase hexadecimal representation
 * 
 * @param   edid    The EDID in raw representation
 * @param   length  The length of `edid`
 * @return          The EDID in lowercase hexadecimal representation
 *                  `NULL` on allocation error, `errno` will be set accordingly
 */
char *libgamma_behex_edid_lowercase(const unsigned char *restrict, size_t);

/**
 * Convert a raw representation of an EDID to an uppercase hexadecimal representation
 * 
 * @param   edid    The EDID in raw representation
 * @param   length  The length of `edid`
 * @return          The EDID in uppercase hexadecimal representation,
 *                  NULL` on allocation error, `errno` will be set accordingly
 */
char *libgamma_behex_edid_uppercase(const unsigned char *restrict, size_t);

/**
 * Convert an hexadecimal representation of an EDID to a raw representation
 * 
 * @param   edid  The EDID in hexadecimal representation
 * @return        The EDID in raw representation, it will be half the length
 *                of `edid` (the input value); `NULL` on allocation error or
 *                if the EDID is malformated, `errno` will be set accordingly
 */
unsigned char *libgamma_unhex_edid(const char *restrict);


/**
 * Get the current gamma ramps for a CRTC, 8-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_ramps8(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps8_t *restrict);

/**
 * Set the gamma ramps for a CRTC, 8-bit gamma-depth version.
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps8(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps8_t);


/**
 * Get the current gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_ramps16(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps16_t *restrict);

/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps16(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps16_t) __attribute__((hot));


/**
 * Get the current gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_ramps32(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps32_t *restrict);

/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps32(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps32_t);


/**
 * Get the current gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_ramps64(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps64_t *restrict);

/**
 * Set the gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps64(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps64_t);


/**
 * Set the gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_rampsf(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsf_t);

/**
 * Get the current gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_rampsf(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsf_t *restrict);


/**
 * Get the current gamma ramps for a CRTC, `double` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_rampsd(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsd_t *restrict);

/**
 * Set the gamma ramps for a CRTC, `double` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_rampsd(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsd_t);


/**
 * Set the gamma ramps for a CRTC, 8-bit gamma-depth function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this            The CRTC state
 * @param   red_function    The function that generates the gamma ramp for the red channel
 * @param   green_function  The function that generates the gamma ramp for the green channel
 * @param   blue_function   The function that generates the gamma ramp for the blue channel
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library
 */
__attribute__((cold))
int libgamma_crtc_set_gamma_ramps8_f(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps8_fun *,
                                     libgamma_gamma_ramps8_fun *, libgamma_gamma_ramps8_fun *);

/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this            The CRTC state
 * @param   red_function    The function that generates the gamma ramp for the red channel
 * @param   green_function  The function that generates the gamma ramp for the green channel
 * @param   blue_function   The function that generates the gamma ramp for the blue channel
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library
 */
__attribute__((cold))
int libgamma_crtc_set_gamma_ramps16_f(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps16_fun *,
                                      libgamma_gamma_ramps16_fun *, libgamma_gamma_ramps16_fun *);

/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this            The CRTC state
 * @param   red_function    The function that generates the gamma ramp for the red channel
 * @param   green_function  The function that generates the gamma ramp for the green channel
 * @param   blue_function   The function that generates the gamma ramp for the blue channel
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library
 */
__attribute__((cold))
int libgamma_crtc_set_gamma_ramps32_f(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps32_fun *,
                                      libgamma_gamma_ramps32_fun *, libgamma_gamma_ramps32_fun *);

/**
 * Set the gamma ramps for a CRTC, 64-bit gamma-depth function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this            The CRTC state
 * @param   red_function    The function that generates the gamma ramp for the red channel
 * @param   green_function  The function that generates the gamma ramp for the green channel
 * @param   blue_function   The function that generates the gamma ramp for the blue channel
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library
 */
__attribute__((cold));
int libgamma_crtc_set_gamma_ramps64_f(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps64_fun *,
                                      libgamma_gamma_ramps64_fun *, libgamma_gamma_ramps64_fun *);

/**
 * Set the gamma ramps for a CRTC, `float` function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this            The CRTC state
 * @param   red_function    The function that generates the gamma ramp for the red channel
 * @param   green_function  The function that generates the gamma ramp for the green channel
 * @param   blue_function   The function that generates the gamma ramp for the blue channel
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library
 */
__attribute__((cold))
int libgamma_crtc_set_gamma_rampsf_f(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsf_fun *,
                                     libgamma_gamma_rampsf_fun *, libgamma_gamma_rampsf_fun *);

/**
 * Set the gamma ramps for a CRTC, `double` function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this            The CRTC state
 * @param   red_function    The function that generates the gamma ramp for the red channel
 * @param   green_function  The function that generates the gamma ramp for the green channel
 * @param   blue_function   The function that generates the gamma ramp for the blue channel
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library
 */
__attribute__((cold))
int libgamma_crtc_set_gamma_rampsd_f(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsd_fun *,
                                     libgamma_gamma_rampsd_fun *, libgamma_gamma_rampsd_fun *);


#ifndef __GNUC__
# undef __attribute__
#endif

#endif
