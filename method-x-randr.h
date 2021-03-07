/* See LICENSE file for copyright and license details. */


#ifdef IN_LIBGAMMA_X_RANDR
# include <xcb/xcb.h>
# include <xcb/randr.h>


/**
 * The major version of RandR the library expects
 */
# define RANDR_VERSION_MAJOR  1

/**
 * The minor version of RandR the library expects
 */
# define RANDR_VERSION_MINOR  3


/**
 * Data structure for partition data
 */
struct libgamma_x_randr_partition_data {
	/**
	 * Mapping from CRTC indices to CRTC identifiers
	 */
	xcb_randr_crtc_t *crtcs;

	/**
	 * Mapping from output indices to output identifiers
	 */
	xcb_randr_output_t *outputs;

	/**
	 * The number of outputs available
	 */
	size_t outputs_count;

	/**
	 * Mapping from CRTC indices to output indices.
	 * CRTC's without an output (should be impossible)
	 * have the value `SIZE_MAX` which is impossible
	 * for an existing mapping
	 */
	size_t *crtc_to_output;

	/**
	 * Screen configuration timestamp
	 */
	xcb_timestamp_t config_timestamp;
};
#endif



/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_x_randr_method_capabilities(struct libgamma_method_capabilities *restrict);

/**
 * Initialise an allocated site state
 * 
 * @param   this    The site state to initialise
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. Once the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocate on the stack. Note however that it will
 *                  not be free:d if this function fails
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__(1), __warn_unused_result__)))
int libgamma_x_randr_site_initialise(struct libgamma_site_state *restrict, char *restrict);

/**
 * Release all resources held by a site state.
 * 
 * @param  this  The site state.
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_x_randr_site_destroy(struct libgamma_site_state *restrict);

/**
 * Restore the gamma ramps all CRTC:s with a site to the system settings
 * 
 * @param   this  The site state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_x_randr_site_restore(struct libgamma_site_state *restrict);


/**
 * Initialise an allocated partition state
 * 
 * @param   this       The partition state to initialise
 * @param   site       The site state for the site that the partition belongs to
 * @param   partition  The the index of the partition within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_x_randr_partition_initialise(struct libgamma_partition_state *restrict, struct libgamma_site_state *restrict, size_t);

/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_x_randr_partition_destroy(struct libgamma_partition_state *restrict);

/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_x_randr_partition_restore(struct libgamma_partition_state *restrict);


/**
 * Initialise an allocated CRTC state
 * 
 * @param   this       The CRTC state to initialise
 * @param   partition  The partition state for the partition that the CRTC belongs to
 * @param   crtc       The the index of the CRTC within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_x_randr_crtc_initialise(struct libgamma_crtc_state *restrict, struct libgamma_partition_state *restrict, size_t);

/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_x_randr_crtc_destroy(struct libgamma_crtc_state *restrict);

/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_x_randr_crtc_restore(struct libgamma_crtc_state *restrict);


/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error; on error refer to the error reports in `this`
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_x_randr_get_crtc_information(struct libgamma_crtc_information *restrict,
                                          struct libgamma_crtc_state *restrict, unsigned long long);

/**
 * Get the current gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_x_randr_crtc_get_gamma_ramps16(struct libgamma_crtc_state *restrict, struct libgamma_gamma_ramps16 *restrict);

/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_x_randr_crtc_set_gamma_ramps16(struct libgamma_crtc_state *restrict, const struct libgamma_gamma_ramps16 *restrict);



#ifdef IN_LIBGAMMA_X_RANDR
/**
 * Translate an xcb error into a libgamma error
 * 
 * @param   error_code     The xcb error
 * @param   default_error  The libgamma error to use if the xcb error is not recognised
 * @param   return_errno   Whether an `errno` value may be returned
 * @return                 The libgamma error
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__)))
int libgamma_x_randr_internal_translate_error(int, int, int);



/* xcb violates the rule to never return struct:s */
# ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Waggregate-return"
# endif
#endif
