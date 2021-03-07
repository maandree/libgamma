/* See LICENSE file for copyright and license details. */
#if !defined(LIBGAMMA_DUMMY_GET_RAMPS) && !defined(LIBGAMMA_DUMMY_SET_RAMPS)


#ifdef IN_LIBGAMMA_DUMMY
/**
 * Configuration set for the dummy adjustment method
 */
struct libgamma_dummy_configurations {
	/**
	 * The method's capabilities
	 * 
	 * Some fields are ignored:
	 * - real
	 * - fake
	 */
	struct libgamma_method_capabilities capabilities;

	/**
	 * Template for CRTC:s information
	 * 
	 * Some fields are ignored:
	 * - width_mm_edid
	 * - width_mm_edid_error
	 * - height_mm_edid
	 * - height_mm_edid_error
	 * - gamma_red
	 * - gamma_green
	 * - gamma_blue
	 * - gamma_error
	 */
	struct libgamma_crtc_information crtc_info_template;

	/**
	 * The adjustment method to use
	 */
	int real_method;

	/**
	 * The number of sites on the system
	 */
	size_t site_count;

	/**
	 * The number of paritions on a site before it has been configured
	 */
	size_t default_partition_count;

	/**
	 * The number of CRTC:s on a paritions before it has been configured
	 */
	size_t default_crtc_count;

	/**
	 * Whether the sites should be inherited from the real method
	 */
	unsigned inherit_sites : 1;

	/**
	 * Whether the partitions should be inherited from the real method
	 */
	unsigned inherit_partition_count : 1;

	/**
	 * Whether the CRTC:s should be inherited from the real method
	 */
	unsigned inherit_crtc_count : 1;

	/**
	 * When a site has been created, stall until the partition count has
	 * been configured
	 */
	unsigned stall_for_partition_count : 1;

	/**
	 * When a parition has been created, stall until the CRTC count has
	 * been configured
	 */
	unsigned stall_for_crtc_count : 1;

	/**
	 * Methods should stall until the system has been configured
	 * unless $LIBGAMMA_DUMMY_STALL is not true
	 */
	unsigned stalled_start : 1;

	/**
	 * Whether to print what is going on in the phony system
	 */
	unsigned verbose : 1;
};


/**
 * Dummy adjustment method internal data for a CRTC
 */
struct libgamma_dummy_crtc {
	/**
	 * The gamma ramp for the red channel
	 */
	void *restrict gamma_red;

	/**
	 * The gamma ramp for the green channel
	 */
	void *restrict gamma_green;

	/**
	 * The gamma ramp for the blue channel
	 */
	void *restrict gamma_blue;

	/**
	 * Information about the CRTC and monitor
	 * 
	 * Some fields are ignored:
	 * - width_mm_edid
	 * - width_mm_edid_error
	 * - height_mm_edid
	 * - height_mm_edid_error
	 * - gamma_red
	 * - gamma_green
	 * - gamma_blue
	 * - gamma_error
	 */
	struct libgamma_crtc_information info;

	/**
	 * Partition state that contains this information
	 */
	struct libgamma_crtc_state *state;
};


/**
 * Dummy adjustment method internal data for a partition
 */
struct libgamma_dummy_partition {
	/**
	 * The CRTC:s on the system
	 */
	struct libgamma_dummy_crtc *crtcs;

	/**
	 * The number of CRTC:s on the system
	 */
	size_t crtc_count;

	/**
	 * Partition state that contains this information
	 */
	struct libgamma_partition_state *state;
};


/**
 * Dummy adjustment method internal data for a site
 */
struct libgamma_dummy_site {
	/**
	 * The partitions on the system
	 */
	struct libgamma_dummy_partition *partitions;

	/**
	 * The number of partitions on the system
	 */
	size_t partition_count;

	/**
	 * Site state that contains this information
	 */
	struct libgamma_site_state *state;
};



/**
 * Configurations for the dummy adjustment method
 */
extern struct libgamma_dummy_configurations libgamma_dummy_internal_configurations;
#endif



/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_dummy_method_capabilities(struct libgamma_method_capabilities *restrict);

/**
 * Initialise an allocated site state
 * 
 * @param   this  The site state to initialise
 * @param   site  The site identifier, unless it is `NULL` it must a
 *                `free`:able. Once the state is destroyed the library
 *                will attempt to free it. There you should not free
 *                it yourself, and it must not be a string constant
 *                or allocate on the stack. Note however that it will
 *                not be free:d if this function fails.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__(1), __warn_unused_result__)))
int libgamma_dummy_site_initialise(struct libgamma_site_state *restrict, char *restrict);

/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_dummy_site_destroy(struct libgamma_site_state *restrict);

/**
 * Restore the gamma ramps all CRTC:s with a site to the system settings
 * 
 * @param   this  The site state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_site_restore(struct libgamma_site_state *restrict);


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
int libgamma_dummy_partition_initialise(struct libgamma_partition_state *restrict, struct libgamma_site_state *restrict, size_t);

/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_dummy_partition_destroy(struct libgamma_partition_state *restrict);

/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_partition_restore(struct libgamma_partition_state *restrict);


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
int libgamma_dummy_crtc_initialise(struct libgamma_crtc_state *restrict, struct libgamma_partition_state *restrict, size_t);

/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_dummy_crtc_destroy(struct libgamma_crtc_state *restrict);

/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_restore(struct libgamma_crtc_state *restrict);


/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error; on error refer to the error reports in `this`
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_get_crtc_information(struct libgamma_crtc_information *restrict,
                                        struct libgamma_crtc_state *restrict, unsigned long long);


/**
 * Get the current gamma ramps for a CRTC, 8-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_get_gamma_ramps8(struct libgamma_crtc_state *restrict, struct libgamma_gamma_ramps8 *restrict);

/**
 * Set the gamma ramps for a CRTC, 8-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_set_gamma_ramps8(struct libgamma_crtc_state *restrict, const struct libgamma_gamma_ramps8 *restrict);


/**
 * Get the current gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_get_gamma_ramps16(struct libgamma_crtc_state *restrict, struct libgamma_gamma_ramps16 *restrict);

/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_set_gamma_ramps16(struct libgamma_crtc_state *restrict, const struct libgamma_gamma_ramps16 *restrict);


/**
 * Get the current gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_get_gamma_ramps32(struct libgamma_crtc_state *restrict, struct libgamma_gamma_ramps32 *restrict);

/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_set_gamma_ramps32(struct libgamma_crtc_state *restrict, const struct libgamma_gamma_ramps32 *restrict);


/**
 * Get the current gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_get_gamma_ramps64(struct libgamma_crtc_state *restrict, struct libgamma_gamma_ramps64 *restrict);

/**
 * Set the gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_set_gamma_ramps64(struct libgamma_crtc_state *restrict, const struct libgamma_gamma_ramps64 *restrict);


/**
 * Get the current gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_get_gamma_rampsf(struct libgamma_crtc_state *restrict, struct libgamma_gamma_rampsf *restrict);

/**
 * Set the gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_set_gamma_rampsf(struct libgamma_crtc_state *restrict, const struct libgamma_gamma_rampsf *restrict);

/**
 * Get the current gamma ramps for a CRTC, `double` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_get_gamma_rampsd(struct libgamma_crtc_state *restrict, struct libgamma_gamma_rampsd *restrict);

/**
 * Set the gamma ramps for a CRTC, `double` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_crtc_set_gamma_rampsd(struct libgamma_crtc_state *restrict, const struct libgamma_gamma_rampsd *restrict);



#ifdef IN_LIBGAMMA_DUMMY
/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * and ignore the method's capabilities
 * 
 * @param   this  The CRTC data
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__)))
int libgamma_dummy_internal_crtc_restore_forced(struct libgamma_dummy_crtc *restrict);
#endif


#else

struct libgamma_dummy_crtc *data = this->data;
union gamma_ramps_any ramps_;
TYPE *r_ramp = data->gamma_red;
TYPE *g_ramp = data->gamma_green;
TYPE *b_ramp = data->gamma_blue;
size_t rn = data->info.red_gamma_size;
size_t gn = data->info.green_gamma_size;
size_t bn = data->info.blue_gamma_size;
size_t i;

#ifdef DEBUG
/* Check gamma ramp sizes */
if (libgamma_dummy_internal_configurations.capabilities.identical_gamma_sizes)
	if (ramps->red_size != ramps->green_size || ramps->red_size != ramps->blue_size)
		return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
if (ramps->red_size != rn || ramps->green_size != gn || ramps->blue_size != bn)
	return LIBGAMMA_WRONG_GAMMA_RAMP_SIZE;
#endif

if (!data->info.gamma_support)
	return LIBGAMMA_GAMMA_RAMP_READ_FAILED;

#ifdef LIBGAMMA_DUMMY_GET_RAMPS
# define TRANSLATE(TDEPTH, SUFFIX)\
do {\
	if (data->info.gamma_depth == DEPTH) {\
		ramps_.FIELD = *ramps;\
		return libgamma_internal_translated_ramp_get(this, &ramps_, DEPTH, TDEPTH, libgamma_crtc_get_gamma_ramps##SUFFIX);\
	}\
} while (0)
#else
# define TRANSLATE(TDEPTH, SUFFIX)\
do {\
	if (data->info.gamma_depth == DEPTH) {\
		ramps_.FIELD = *ramps;\
		return libgamma_internal_translated_ramp_set(this, &ramps_, DEPTH, TDEPTH, libgamma_crtc_set_gamma_ramps##SUFFIX); \
	}\
} while (0)
#endif

#if DEPTH != 8
TRANSLATE(8, 8);
#endif
#if DEPTH != 16
TRANSLATE(16, 16);
#endif
#if DEPTH != 32
TRANSLATE(32, 32);
#endif
#if DEPTH != 64
TRANSLATE(64, 64);
#endif
#if DEPTH != -1
TRANSLATE(-1, f);
#endif
#if DEPTH != -2
TRANSLATE(-2, d);
#endif

#undef TRANSLATE

#ifdef LIBGAMMA_DUMMY_GET_RAMPS
for (i = 0; i < rn; i++) ramps->red[i]   = r_ramp[i];
for (i = 0; i < gn; i++) ramps->green[i] = g_ramp[i];
for (i = 0; i < bn; i++) ramps->blue[i]  = b_ramp[i];
#else
for (i = 0; i < rn; i++) r_ramp[i] = ramps->red[i];
for (i = 0; i < gn; i++) g_ramp[i] = ramps->green[i];
for (i = 0; i < bn; i++) b_ramp[i] = ramps->blue[i];
#endif

return 0;

#endif
