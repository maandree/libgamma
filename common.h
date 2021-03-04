/* See LICENSE file for copyright and license details. */
#include "libgamma.h"

#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"


#if !defined(HAVE_INT128) && defined(__GNUC__)
# define HAVE_INT128
#endif



#define LIST_ERRORS(_)\
	_(LIBGAMMA_ERRNO_SET, NULL)\
	_(LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD, "No such adjustment method")\
	_(LIBGAMMA_NO_SUCH_SITE, "No such site")\
	_(LIBGAMMA_NO_SUCH_PARTITION, "No such partition")\
	_(LIBGAMMA_NO_SUCH_CRTC, "No such CRTC")\
	_(LIBGAMMA_IMPOSSIBLE_AMOUNT, "Impossible amount")\
	_(LIBGAMMA_CONNECTOR_DISABLED, "Connector is disabled")\
	_(LIBGAMMA_OPEN_CRTC_FAILED, "Could not open CRTC")\
	_(LIBGAMMA_CRTC_INFO_NOT_SUPPORTED, "CRTC information is not supported")\
	_(LIBGAMMA_GAMMA_RAMP_READ_FAILED, "Could not read readm gamma ramp")\
	_(LIBGAMMA_GAMMA_RAMP_WRITE_FAILED, "Could not write to gamma ramp")\
	_(LIBGAMMA_GAMMA_RAMP_SIZE_CHANGED, "Gamma ramp size changed")\
	_(LIBGAMMA_MIXED_GAMMA_RAMP_SIZE, "Mixed gamma ramp size")\
	_(LIBGAMMA_WRONG_GAMMA_RAMP_SIZE, "Wrong gamma ramp size")\
	_(LIBGAMMA_SINGLETON_GAMMA_RAMP, "Single-stop gamma ramp")\
	_(LIBGAMMA_LIST_CRTCS_FAILED, "Could not retrieve list of CRTCs")\
	_(LIBGAMMA_ACQUIRING_MODE_RESOURCES_FAILED, "Could not acquire mode resources")\
	_(LIBGAMMA_NEGATIVE_PARTITION_COUNT, "Number of partitions is negative")\
	_(LIBGAMMA_NEGATIVE_CRTC_COUNT, "Number of CRTCs is negative")\
	_(LIBGAMMA_DEVICE_RESTRICTED, "Device is restricted to root")\
	_(LIBGAMMA_DEVICE_ACCESS_FAILED, "Failed to access device")\
	_(LIBGAMMA_DEVICE_REQUIRE_GROUP, "Device requires group membership")\
	_(LIBGAMMA_GRAPHICS_CARD_REMOVED, "Graphics card was removed")\
	_(LIBGAMMA_STATE_UNKNOWN, "Unknown state")\
	_(LIBGAMMA_CONNECTOR_UNKNOWN, "Unknwon connector")\
	_(LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED, "Connector type not recognised")\
	_(LIBGAMMA_SUBPIXEL_ORDER_NOT_RECOGNISED, "Subpixel order not recognised")\
	_(LIBGAMMA_EDID_LENGTH_UNSUPPORTED, "Unsupported EDID length")\
	_(LIBGAMMA_EDID_WRONG_MAGIC_NUMBER, "Incorrect magic number in EDID")\
	_(LIBGAMMA_EDID_REVISION_UNSUPPORTED, "Unsupported EDID revision")\
	_(LIBGAMMA_GAMMA_NOT_SPECIFIED, "No gamma information specified")\
	_(LIBGAMMA_EDID_CHECKSUM_ERROR, "Incorrect checksum in EDID")\
	_(LIBGAMMA_GAMMA_NOT_SPECIFIED_AND_EDID_CHECKSUM_ERROR, "No gamma information specified and incorrect checksum in EDID")\
	_(LIBGAMMA_GAMMA_RAMPS_SIZE_QUERY_FAILED, "Could not retrieve gamma ramp size")\
	_(LIBGAMMA_OPEN_PARTITION_FAILED, "Could not open partition")\
	_(LIBGAMMA_OPEN_SITE_FAILED, "Could not open site")\
	_(LIBGAMMA_PROTOCOL_VERSION_QUERY_FAILED, "Could not retreive protocol version")\
	_(LIBGAMMA_PROTOCOL_VERSION_NOT_SUPPORTED, "Protocol version not supported")\
	_(LIBGAMMA_LIST_PARTITIONS_FAILED, "Could not retrieve list of partitions")\
	_(LIBGAMMA_NULL_PARTITION, "Null partition retrieved")\
	_(LIBGAMMA_NOT_CONNECTED, "Not connected to subsystem")\
	_(LIBGAMMA_REPLY_VALUE_EXTRACTION_FAILED, "Could not extract value from reply")\
	_(LIBGAMMA_EDID_NOT_FOUND, "No EDID found")\
	_(LIBGAMMA_LIST_PROPERTIES_FAILED, "Could not list properties")\
	_(LIBGAMMA_PROPERTY_VALUE_QUERY_FAILED, "Could not query the value of a property")\
	_(LIBGAMMA_OUTPUT_INFORMATION_QUERY_FAILED, "Could not query information for output")

#define X(A, B) -1
# if (LIST_ERRORS(X)) != LIBGAMMA_ERROR_MIN
#  error There is a mismatch between LIST_ERRORS and LIBGAMMA_ERROR_MIN
# endif
#undef X



#ifdef HAVE_LIBGAMMA_METHOD_DUMMY
# define HAVE_METHOD_DUMMY 1
#else
# define HAVE_METHOD_DUMMY 0
#endif

#ifdef HAVE_LIBGAMMA_METHOD_X_RANDR
# define HAVE_METHOD_X_RANDR 1
#else
# define HAVE_METHOD_X_RANDR 0
#endif

#ifdef HAVE_LIBGAMMA_METHOD_X_VIDMODE
# define HAVE_METHOD_X_VIDMODE 1
#else
# define HAVE_METHOD_X_VIDMODE 0
#endif

#ifdef HAVE_LIBGAMMA_METHOD_LINUX_DRM
# define HAVE_METHOD_LINUX_DRM 1
#else
# define HAVE_METHOD_LINUX_DRM 0
#endif

#ifdef HAVE_LIBGAMMA_METHOD_W32_GDI
# define HAVE_METHOD_W32_GDI 1
#else
# define HAVE_METHOD_W32_GDI 0
#endif

#ifdef HAVE_LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS
# define HAVE_METHOD_QUARTZ_CORE_GRAPHICS 1
#else
# define HAVE_METHOD_QUARTZ_CORE_GRAPHICS 0
#endif

#define LIST_METHODS(_)\
	_(LIBGAMMA_METHOD_X_RANDR, randr, x_randr, HAVE_METHOD_X_RANDR)\
	_(LIBGAMMA_METHOD_X_VIDMODE, vidmode, x_vidmode, HAVE_METHOD_X_VIDMODE)\
	_(LIBGAMMA_METHOD_LINUX_DRM, drm, linux_drm, HAVE_METHOD_LINUX_DRM)\
	_(LIBGAMMA_METHOD_W32_GDI, w32gdi, w32_gdi, HAVE_METHOD_W32_GDI)\
	_(LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS, quartz, quartz_cg, HAVE_METHOD_QUARTZ_CORE_GRAPHICS)\
	_(LIBGAMMA_METHOD_DUMMY, dummy, dummy, HAVE_METHOD_DUMMY)

#define X(...) +1
# if (LIST_METHODS(X)) != LIBGAMMA_METHOD_COUNT
#  error There is a mismatch between LIST_METHODS and LIBGAMMA_METHOD_COUNT
# endif
#undef X



#define LIST_CONNECTOR_TYPES(_)\
	_(LIBGAMMA_CONNECTOR_TYPE_Unknown, "Unknown")\
	_(LIBGAMMA_CONNECTOR_TYPE_VGA, "VGA")\
	_(LIBGAMMA_CONNECTOR_TYPE_DVI, "DVI")\
	_(LIBGAMMA_CONNECTOR_TYPE_DVII, "DVI-I")\
	_(LIBGAMMA_CONNECTOR_TYPE_DVID, "DVI-D")\
	_(LIBGAMMA_CONNECTOR_TYPE_DVIA, "DVI-A")\
	_(LIBGAMMA_CONNECTOR_TYPE_Composite, "Composite")\
	_(LIBGAMMA_CONNECTOR_TYPE_SVIDEO, "S-video")\
	_(LIBGAMMA_CONNECTOR_TYPE_LVDS, "LVDS")\
	_(LIBGAMMA_CONNECTOR_TYPE_Component, "Component")\
	_(LIBGAMMA_CONNECTOR_TYPE_9PinDIN, "9 pin DIN")\
	_(LIBGAMMA_CONNECTOR_TYPE_DisplayPort, "DisplayPort")\
	_(LIBGAMMA_CONNECTOR_TYPE_HDMI, "HDMI")\
	_(LIBGAMMA_CONNECTOR_TYPE_HDMIA, "HDMI-A")\
	_(LIBGAMMA_CONNECTOR_TYPE_HDMIB, "HDMI-B")\
	_(LIBGAMMA_CONNECTOR_TYPE_TV, "TV")\
	_(LIBGAMMA_CONNECTOR_TYPE_eDP, "eDP")\
	_(LIBGAMMA_CONNECTOR_TYPE_VIRTUAL, "Virtual")\
	_(LIBGAMMA_CONNECTOR_TYPE_DSI, "DSI")\
	_(LIBGAMMA_CONNECTOR_TYPE_LFP, "LFP")

#define X(A, B) +1
# if (LIST_CONNECTOR_TYPES(X)) != LIBGAMMA_CONNECTOR_TYPE_COUNT
#  error There is a mismatch between LIST_CONNECTOR_TYPES and LIBGAMMA_CONNECTOR_TYPE_COUNT
# endif
#undef X



#define LIST_SUBPIXEL_ORDERS(_)\
	_(LIBGAMMA_SUBPIXEL_ORDER_UNKNOWN, "Unknown")\
	_(LIBGAMMA_SUBPIXEL_ORDER_NONE, "None")\
	_(LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB, "Horizontal RGB")\
	_(LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_BGR, "Horizontal BGR")\
	_(LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_RGB, "Vertical RGB")\
	_(LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_BGR, "Vertical BGR")

#define X(A, B) +1
# if (LIST_SUBPIXEL_ORDERS(X)) != LIBGAMMA_SUBPIXEL_ORDER_COUNT
#  error There is a mismatch between LIST_SUBPIXEL_ORDERS and LIBGAMMA_SUBPIXEL_ORDER_COUNT
# endif
#undef X



/**
 * Gamma ramp structure union for different depths
 */
typedef union gamma_ramps_any {
	/**
	 * 8-bit gamma ramps
	 */
	libgamma_gamma_ramps8_t bits8;

	/**
	 * 16-bit gamma ramps
	 */
	libgamma_gamma_ramps16_t bits16;

	/**
	 * 32-bit gamma ramps
	 */
	libgamma_gamma_ramps32_t bits32;

	/**
	 * 64-bit gamma ramps
	 */
	libgamma_gamma_ramps64_t bits64;

	/**
	 * Single precision float gamma ramps
	 */
	libgamma_gamma_rampsf_t float_single;

	/**
	 * Double precision float gamma ramps
	 */
	libgamma_gamma_rampsd_t float_double;

} gamma_ramps_any_t;


/**
 * A function for reading the gamma ramps from a CRTC
 *
 * @param   this   The CRTC state
 * @param   ramps  The store for the gamma ramps
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
typedef int get_ramps_any_fun(libgamma_crtc_state_t *restrict, gamma_ramps_any_t *restrict);

/**
 * A function for writing the gamma ramps to a CRTC
 *
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
typedef int set_ramps_any_fun(libgamma_crtc_state_t *restrict, gamma_ramps_any_t);



/**
 * Get the current gamma ramps for a CRTC, re-encoding version
 * 
 * @param   this          The CRTC state
 * @param   ramps         The gamma ramps to fill with the current values
 * @param   depth_user    The depth of the gamma ramps that are provided by the user,
 *                        `-1` for `float`, `-2` for `double`
 * @param   depth_system  The depth of the gamma ramps as required by the adjustment method,
 *                        `-1` for `float`, `-2` for `double`
 * @param   fun           Function that is to be used read the ramps, its parameters have
 *                        the same function as those of this function with the same names,
 *                        and the return value too is identical
 * @return                Zero on success, otherwise (negative) the value of an
 *                        error identifier provided by this library
 */
#define libgamma_internal_translated_ramp_get(this, ramps, depth_user, depth_system, fun)\
	libgamma_internal_translated_ramp_get_((this), (ramps), (depth_user), (depth_system), (get_ramps_any_fun *)(fun))

/**
 * Set the gamma ramps for a CRTC, re-encoding version
 * 
 * @param   this          The CRTC state
 * @param   ramps         The gamma ramps to apply
 * @param   depth_user    The depth of the gamma ramps that are provided by the user,
 *                        `-1` for `float`, `-2` for `double`
 * @param   depth_system  The depth of the gamma ramps as required by the adjustment method,
 *                        `-1` for `float`, `-2` for `double`
 * @param   fun           Function that is to be used write the ramps, its parameters have
 *                        the same function as those of this function with the same names,
 *                        and the return value too is identical
 * @return                Zero on success, otherwise (negative) the value of an
 *                        error identifier provided by this library
 */
#define libgamma_internal_translated_ramp_set(this, ramps, depth_user, depth_system, fun)\
	libgamma_internal_translated_ramp_set_((this), (ramps), (depth_user), (depth_system), (set_ramps_any_fun *)fun)

/**
 * Get the current gamma ramps for a CRTC, re-encoding version
 * 
 * @param   this          The CRTC state
 * @param   ramps         The gamma ramps to fill with the current values
 * @param   depth_user    The depth of the gamma ramps that are provided by the user,
 *                        `-1` for `float`, `-2` for `double`
 * @param   depth_system  The depth of the gamma ramps as required by the adjustment method,
 *                        `-1` for `float`, `-2` for `double`
 * @param   fun           Function that is to be used read the ramps, its parameters have
 *                        the same function as those of this function with the same names,
 *                        and the return value too is identical
 * @return                Zero on success, otherwise (negative) the value of an
 *                        error identifier provided by this library
 */
int libgamma_internal_translated_ramp_get_(libgamma_crtc_state_t *restrict, gamma_ramps_any_t *restrict,
                                           signed, signed, get_ramps_any_fun *);

/**
 * Set the gamma ramps for a CRTC, re-encoding version
 * 
 * @param   this          The CRTC state
 * @param   ramps         The gamma ramps to apply
 * @param   depth_user    The depth of the gamma ramps that are provided by the user,
 *                        `-1` for `float`, `-2` for `double`
 * @param   depth_system  The depth of the gamma ramps as required by the adjustment method,
 *                        `-1` for `float`, `-2` for `double`
 * @param   fun           Function that is to be used write the ramps, its parameters have
 *                        the same function as those of this function with the same names,
 *                        and the return value too is identical
 * @return                Zero on success, otherwise (negative) the value of an
 *                        error identifier provided by this library
 */
int libgamma_internal_translated_ramp_set_(libgamma_crtc_state_t *restrict, gamma_ramps_any_t,
                                           signed, signed, set_ramps_any_fun *);

/**
 * Convert any set of gamma ramps into a 64-bit integer array with all channels
 * 
 * @param  depth  The depth of the gamma ramp, `-1` for `float`, `-2` for `double`
 * @param  n      The grand size of gamma ramps (sum of all channels' sizes)
 * @param  out    Output array
 * @param  in     Input gamma ramps
 */
void libgamma_internal_translate_to_64(signed, size_t, uint64_t *restrict, gamma_ramps_any_t);

/**
 * Undo the actions of `libgamma_internal_translate_to_64`
 * 
 * @param  depth  The depth of the gamma ramp, `-1` for `float`, `-2` for `double`
 * @param  n      The grand size of gamma ramps (sum of all channels' sizes)
 * @param  out    Output gamma ramps
 * @param  in     Input array, may be modified
 */
void libgamma_internal_translate_from_64(signed, size_t, gamma_ramps_any_t, uint64_t *restrict);

/**
 * Allocate and initalise a gamma ramp with any depth
 * 
 * @param   ramps_sys  Output gamma ramps
 * @param   ramps      The gamma ramps whose sizes should be duplicated
 * @param   depth      The depth of the gamma ramps to allocate,
 *                     `-1` for `float`, `-2` for `double`
 * @param   elements   Output reference for the grand size of the gamma ramps
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int libgamma_internal_allocated_any_ramp(gamma_ramps_any_t *restrict, gamma_ramps_any_t, signed, size_t *restrict);


/**
 * Parse the EDID of a monitor
 * 
 * @param   this    Instance of a data structure to fill with the information about the EDID;
 *                  it must contain the EDID and its length
 * @param   fields  OR:ed identifiers for the information about the EDID that should be parsed;
 *                  fields that do not have to do with EDID are ignored
 * @return          Non-zero on error
 */
int libgamma_internal_parse_edid(libgamma_crtc_information_t *restrict, int32_t);
