/* See LICENSE file for copyright and license details. */

#ifdef IN_LIBGAMMA_LINUX_DRM 
# include <xf86drm.h>
# include <xf86drmMode.h>

/**
 * Graphics card data for the Direct Rendering Manager adjustment method
 */
typedef struct libgamma_drm_card_data {
	/**
	 * File descriptor for the connection to the graphics card
	 */
	int fd;

	/**
	 * The graphics card's mode resources
	 */
	drmModeRes *res;

	/**
	 * Resources for open connectors
	 */
	drmModeConnector **connectors;

	/**
	 * Resources for open encoders
	 */
	drmModeEncoder **encoders;

} libgamma_drm_card_data_t;
#endif



/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_linux_drm_method_capabilities(libgamma_method_capabilities_t *restrict);

/**
 * Initialise an allocated site state
 * 
 * @param   this    The site state to initialise
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. Once the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocate on the stack. Note however that it will
 *                  not be free:d if this function fails.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__(1), __warn_unused_result__)))
int libgamma_linux_drm_site_initialise(libgamma_site_state_t *restrict, char *restrict);

/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_linux_drm_site_destroy(libgamma_site_state_t *restrict);

/**
 * Restore the gamma ramps all CRTC:s with a site to the system settings
 * 
 * @param   this  The site state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_linux_drm_site_restore(libgamma_site_state_t *restrict);


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
int libgamma_linux_drm_partition_initialise(libgamma_partition_state_t *restrict, libgamma_site_state_t *restrict, size_t);

/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_linux_drm_partition_destroy(libgamma_partition_state_t *restrict);

/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_linux_drm_partition_restore(libgamma_partition_state_t *restrict);


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
int libgamma_linux_drm_crtc_initialise(libgamma_crtc_state_t *restrict, libgamma_partition_state_t *restrict, size_t);

/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_linux_drm_crtc_destroy(libgamma_crtc_state_t *restrict);

/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_linux_drm_crtc_restore(libgamma_crtc_state_t *restrict);


/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error; on error refer to the error reports in `this`
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_linux_drm_get_crtc_information(libgamma_crtc_information_t *restrict,
                                            libgamma_crtc_state_t *restrict, unsigned long long);

/**
 * Get the current gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_linux_drm_crtc_get_gamma_ramps16(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps16_t *restrict);

/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_linux_drm_crtc_set_gamma_ramps16(libgamma_crtc_state_t *restrict, const libgamma_gamma_ramps16_t *restrict);



#ifdef IN_LIBGAMMA_LINUX_DRM 
/**
 * Release all connectors and encoders
 * 
 * @param  this  The graphics card data
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_linux_drm_internal_release_connectors_and_encoders(libgamma_drm_card_data_t *restrict);
#endif
