/* See LICENSE file for copyright and license details. */
#ifndef LIBGAMMA_H
#define LIBGAMMA_H

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>


#ifdef __GNUC__
# define LIBGAMMA_GCC_ONLY__(X)  X
#else
# define LIBGAMMA_GCC_ONLY__(X)
#endif



/**
 * `errno` has be set with a standard error number
 * to indicate the what has gone wrong
 */
#define LIBGAMMA_ERRNO_SET (-1)

/**
 * The selected adjustment method does not exist
 * or has been excluded at compile-time
 */
#define LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD (-2)

/**
 * The selected site does not exist
 */
#define LIBGAMMA_NO_SUCH_SITE (-3)

/**
 * The selected partition does not exist
 */
#define LIBGAMMA_NO_SUCH_PARTITION (-4)

/**
 * The selected CRTC does not exist
 */
#define LIBGAMMA_NO_SUCH_CRTC (-5)

/**
 * Counter overflowed when counting the number
 * of available items
 */
#define LIBGAMMA_IMPOSSIBLE_AMOUNT (-6)

/**
 * The selected connector is disabled, it does
 * not have a CRTC
 */
#define LIBGAMMA_CONNECTOR_DISABLED (-7)

/**
 * The selected CRTC could not be opened,
 * reason unknown
 */
#define LIBGAMMA_OPEN_CRTC_FAILED (-8)

/**
 * The CRTC information field is not supported
 * by the adjustment method
 */
#define LIBGAMMA_CRTC_INFO_NOT_SUPPORTED (-9)

/**
 * Failed to read the current gamma ramps for
 * the selected CRTC, reason unknown
 */
#define LIBGAMMA_GAMMA_RAMP_READ_FAILED (-10)

/**
 * Failed to write the current gamma ramps for
 * the selected CRTC, reason unknown
 */
#define LIBGAMMA_GAMMA_RAMP_WRITE_FAILED (-11)

/**
 * The specified ramp sizes does not match the
 * ramps sizes returned by the adjustment methods
 * in response to the query/command
 */
#define LIBGAMMA_GAMMA_RAMP_SIZE_CHANGED (-12)

/**
 * The specified ramp sizes are not identical
 * which is required by the adjustment method
 * 
 * (Only returned in debug mode)
 */
#define LIBGAMMA_MIXED_GAMMA_RAMP_SIZE (-13)

/**
 * The specified ramp sizes are not supported
 * by the adjustment method
 * 
 * (Only returned in debug mode)
 */
#define LIBGAMMA_WRONG_GAMMA_RAMP_SIZE (-14)

/**
 * The adjustment method reported that the gamma
 * ramps size is 1, or perhaps even zero or negative
 */
#define LIBGAMMA_SINGLETON_GAMMA_RAMP (-15)

/**
 * The adjustment method failed to list
 * available CRTC:s, reason unknown
 */
#define LIBGAMMA_LIST_CRTCS_FAILED (-16)

/**
 * Failed to acquire mode resources from the
 * adjustment method
 */
#define LIBGAMMA_ACQUIRING_MODE_RESOURCES_FAILED (-17)

/**
 * The adjustment method reported that a negative
 * number of partitions exists in the site
 */
#define LIBGAMMA_NEGATIVE_PARTITION_COUNT (-18)

/**
 * The adjustment method reported that a negative
 * number of CRTC:s exists in the partition
 */
#define LIBGAMMA_NEGATIVE_CRTC_COUNT (-19)

/**
 * Device cannot be access becauses of
 * insufficient permissions
 */
#define LIBGAMMA_DEVICE_RESTRICTED (-20)

/**
 * Device cannot be access, reason unknown
 */
#define LIBGAMMA_DEVICE_ACCESS_FAILED (-21)

/**
 * Device cannot be access, membership of the
 * `libgamma_group_gid` (named by `libgamma_group_name`
 * (can be `NULL`, if so `errno` may have been set
 * to tell why)) is required
 */
#define LIBGAMMA_DEVICE_REQUIRE_GROUP (-22)

/**
 * The graphics card appear to have been removed
 */
#define LIBGAMMA_GRAPHICS_CARD_REMOVED (-23)

/**
 * The state of the requested information is unknown
 */
#define LIBGAMMA_STATE_UNKNOWN (-24)

/**
 * Failed to determine which connector the
 * CRTC belongs to
 */
#define LIBGAMMA_CONNECTOR_UNKNOWN (-25)

/**
 * The detected connector type is not listed
 * in this library and has to be updated
 */
#define LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED (-26)

/**
 * The detected subpixel order is not listed
 * in this library and has to be updated
 */
#define LIBGAMMA_SUBPIXEL_ORDER_NOT_RECOGNISED (-27)

/**
 * The length of the EDID does not match that
 * of any supported EDID structure revision
 */
#define LIBGAMMA_EDID_LENGTH_UNSUPPORTED (-28)

/**
 * The magic number in the EDID does not match
 * that of any supported EDID structure revision
 */
#define LIBGAMMA_EDID_WRONG_MAGIC_NUMBER (-29)

/**
 * The EDID structure revision used by the
 * monitor is not supported
 */
#define LIBGAMMA_EDID_REVISION_UNSUPPORTED (-30)

/**
 * The gamma characteristics field in the EDID
 * is left unspecified
 * 
 * (This could be considered a non-error)
 */
#define LIBGAMMA_GAMMA_NOT_SPECIFIED (-31)

/**
 * The checksum in the EDID is incorrect, all
 * request information has been provided
 * by you cannot count on it
 */
#define LIBGAMMA_EDID_CHECKSUM_ERROR (-32)

/**
 * Both of the errors `LIBGAMMA_GAMMA_NOT_SPECIFIED`
 * and `LIBGAMMA_EDID_CHECKSUM_ERROR` have occurred
 */
#define LIBGAMMA_GAMMA_NOT_SPECIFIED_AND_EDID_CHECKSUM_ERROR (-33)

/**
 * Failed to query the gamma ramps size from the
 * adjustment method, reason unknown
 */
#define LIBGAMMA_GAMMA_RAMPS_SIZE_QUERY_FAILED (-34)

/**
 * The selected partition could not be opened,
 * reason unknown
 */
#define LIBGAMMA_OPEN_PARTITION_FAILED (-35)

/**
 * The selected site could not be opened,
 * reason unknown
 */
#define LIBGAMMA_OPEN_SITE_FAILED (-36)

/**
 * Failed to query the adjustment method for
 * its protocol version, reason unknown
 */
#define LIBGAMMA_PROTOCOL_VERSION_QUERY_FAILED (-37)

/**
 * The adjustment method's version of its
 * protocol is not supported
 */
#define LIBGAMMA_PROTOCOL_VERSION_NOT_SUPPORTED (-38)

/**
 * The adjustment method failed to list
 * available partitions, reason unknown
 */
#define LIBGAMMA_LIST_PARTITIONS_FAILED (-39)

/**
 * Partition exists by index, but the partition
 * at that index does not exist
 */
#define LIBGAMMA_NULL_PARTITION (-40)

/**
 * There is not monitor connected to the
 * connector of the selected CRTC
 */
#define LIBGAMMA_NOT_CONNECTED (-41)

/**
 * Data extraction from a reply from the
 * adjustment method failed, reason unknown
 */
#define LIBGAMMA_REPLY_VALUE_EXTRACTION_FAILED (-42)

/**
 * No EDID property was found on the output
 */
#define LIBGAMMA_EDID_NOT_FOUND (-43)

/**
 * Failed to list properties on the output,
 * reason unknown
 */
#define LIBGAMMA_LIST_PROPERTIES_FAILED (-44)

/**
 * Failed to query a property's value from
 * the output, reason unknown
 */
#define LIBGAMMA_PROPERTY_VALUE_QUERY_FAILED (-45)

/**
 * A request for information on an output
 * failed, reason unknown
 */
#define LIBGAMMA_OUTPUT_INFORMATION_QUERY_FAILED (-46)

/* DEVELOPERS: Remember to update LIBGAMMA_ERROR_MIN below and LIST_ERRORS in common.h when adding errors */

/**
 * The number of the libgamma error with the
 * lowest number in the version of the library
 * that the program is compiled against
 */
#define LIBGAMMA_ERROR_MIN (-46)

/**
 * The number of the libgamma error with the
 * lowest number in the version of the library
 * that the program is linked against
 */
extern const int libgamma_error_min;



/**
 * The identifier for the dummy adjustment method
 * 
 * This method can be configured and is useful for
 * testing your program's ability to handle errors
 */
#define LIBGAMMA_METHOD_DUMMY 0

/**
 * The identifier for the adjustment method with
 * uses the RandR protocol under the X display server
 */
#define LIBGAMMA_METHOD_X_RANDR 1

/**
 * The identifier for the adjustment method with
 * uses the VidMode protocol under the X display server
 * 
 * This is an older alternative to RandR that can
 * work on some drivers that are not supported by RandR,
 * however it can only control the primary CRTC per
 * screen (partition)
 */
#define LIBGAMMA_METHOD_X_VIDMODE 2

/**
 * The identifier for the Direct Rendering Manager
 * adjustment method that is available in Linux
 * (built in to the Linux kernel with a userland
 * library for access) and is a part of the
 * Direct Rendering Infrastructure.
 * 
 * This adjustment method will work when you are
 * in non-graphical mode; however a display server
 * cannot be started while this is running, but it
 * can be started while a display server is running
 */
#define LIBGAMMA_METHOD_LINUX_DRM 3

/**
 * The identifier for the Graphics Device Interface
 * adjustment method that is available in Windows
 * 
 * This method is not well tested; it can be compiled
 * to be available under X.org using a translation layer
 */
#define LIBGAMMA_METHOD_W32_GDI 4

/**
 * The identifier for the CoreGraphics adjustment
 * method that is available in Mac OS X that can
 * adjust gamma ramps under the Quartz display server
 * 
 * This method is not well tested; it can be compiled
 * to be available under X.org using a translation layer
 */
#define LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS 5

/* DEVELOPERS: Remember to update LIBGAMMA_METHOD_COUNT below and LIST_METHODS in common.h when adding methods */

/**
 * The number adjustment methods provided by the
 * version this library the program is compiled
 * against
 * 
 * This number includes both compile-time enabled
 * and compile-time disabled adjustment methods
 */
#define LIBGAMMA_METHOD_COUNT 6

/**
 * The number adjustment methods provided by the
 * version this library the program is linked
 * against
 * 
 * This number includes both compile-time enabled
 * and compile-time disabled adjustment methods
 */
extern const int libgamma_method_count;



/**
 * Types for connectors
 */
typedef enum libgamma_connector_type {
	/**
	 * The adjustment method does not know the connector's type
	 * 
	 * (This could be considered an error)
	 */
	LIBGAMMA_CONNECTOR_TYPE_Unknown = 0,

	/**
	 * Video Graphics Array (VGA)
	 */
	LIBGAMMA_CONNECTOR_TYPE_VGA,

	/**
	 * Digital Visual Interface, unknown type
	 */
	LIBGAMMA_CONNECTOR_TYPE_DVI,

	/**
	 * Digital Visual Interface, integrated (DVI-I)
	 */
	LIBGAMMA_CONNECTOR_TYPE_DVII,

	/**
	 * Digital Visual Interface, digital only (DVI-D)
	 */
	LIBGAMMA_CONNECTOR_TYPE_DVID,

	/**
	 * Digital Visual Interface, analogue only (DVI-A)
	 */
	LIBGAMMA_CONNECTOR_TYPE_DVIA,

	/**
	 * Composite video
	 */
	LIBGAMMA_CONNECTOR_TYPE_Composite,

	/**
	 * Separate Video (S-video)
	 */
	LIBGAMMA_CONNECTOR_TYPE_SVIDEO,

	/**
	 * Low-voltage differential signaling (LVDS)
	 */
	LIBGAMMA_CONNECTOR_TYPE_LVDS,

	/**
	 * Component video, usually separate cables for each channel
	 */
	LIBGAMMA_CONNECTOR_TYPE_Component,

	/**
	 * 9 pin DIN (Deutsches Institut für Normung) connector
	 */
	LIBGAMMA_CONNECTOR_TYPE_9PinDIN,

	/**
	 * DisplayPort
	 */
	LIBGAMMA_CONNECTOR_TYPE_DisplayPort,

	/**
	 * High-Definition Multimedia Interface (HDMI), unknown type
	 */
	LIBGAMMA_CONNECTOR_TYPE_HDMI,

	/**
	 * High-Definition Multimedia Interface, type A (HDMI-A)
	 */
	LIBGAMMA_CONNECTOR_TYPE_HDMIA,

	/**
	 * High-Definition Multimedia Interface, type B (HDMI-B)
	 */
	LIBGAMMA_CONNECTOR_TYPE_HDMIB,

	/**
	 * Television, unknown connector
	 */
	LIBGAMMA_CONNECTOR_TYPE_TV,

	/**
	 * Embedded DisplayPort (eDP)
	 */
	LIBGAMMA_CONNECTOR_TYPE_eDP,

	/**
	 * A virtual connector
	 */
	LIBGAMMA_CONNECTOR_TYPE_VIRTUAL,

	/**
	 * Display Serial Interface (DSI)
	 */
	LIBGAMMA_CONNECTOR_TYPE_DSI,

	/**
	 * LFP connector
	 * 
	 * (TODO What is a LFP connector?)
	 */
	LIBGAMMA_CONNECTOR_TYPE_LFP,

	/**
	 * DPI connector
	 * 
	 * (TODO What is a DPI connector?)
	 */
	LIBGAMMA_CONNECTOR_TYPE_DPI,

	/**
	 * A writeback connector
	 * 
	 * (TODO What is the difference between Virtual and Writeback?)
	 */
	LIBGAMMA_CONNECTOR_TYPE_WRITEBACK,

	/**
	 * SPI connector
	 * 
	 * (TODO What is an SPI connector?)
	 */
	LIBGAMMA_CONNECTOR_TYPE_SPI

	/* DEVELOPERS: Remember to update LIBGAMMA_CONNECTOR_TYPE_COUNT below
	 *             and LIST_CONNECTOR_TYPES in common.h when adding methods */

} libgamma_connector_type_t;

/**
 * The number of values defined in `libgamma_connector_type_t`
 * in the version of the library the program is compiled against
 */
#define LIBGAMMA_CONNECTOR_TYPE_COUNT 23

/**
 * The number of values defined in `libgamma_connector_type_t`
 * in the version of the library the program is linked against
 */
extern const int libgamma_connector_type_count;



/**
 * Orders for subpixels
 * 
 * Currently the possible values are very biased
 * to LCD, Plasma and monochrome monitors
 */
typedef enum libgamma_subpixel_order {
	/**
	 * The adjustment method does not know the order of the subpixels
	 * 
	 * (This could be considered an error)
	 */
	LIBGAMMA_SUBPIXEL_ORDER_UNKNOWN = 0,

	/**
	 * There are no subpixels in the monitor
	 */
	LIBGAMMA_SUBPIXEL_ORDER_NONE,

	/**
	 * The subpixels are ordered red, green and then blue, from left to right
	 */
	LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB,

	/**
	 * The subpixels are ordered blue, green and then red, from left to right
	 */
	LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_BGR,

	/**
	 * The subpixels are ordered red, green and then blue, from the top down
	 */
	LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_RGB,

	/**
	 * The subpixels are ordered blue, green and then red, from the top down
	 */
	LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_BGR

	/* DEVELOPERS: Remember to update LIBGAMMA_SUBPIXEL_ORDER_COUNT below
	 *             and LIST_SUBPIXEL_ORDERS in common.h when adding methods */

} libgamma_subpixel_order_t;

/**
 * The number of values defined in `libgamma_subpixel_order_t`
 * in the version of the library the program is compiled against
 */
#define LIBGAMMA_SUBPIXEL_ORDER_COUNT 6

/**
 * The number of values defined in `libgamma_subpixel_order_t`
 * in the version of the library the program is linked against
 */
extern const int libgamma_subpixel_order_count;



/**
 * Answer enum to a decision problem
 */
typedef enum libgamma_decision {
	/**
	 * The answer is negative
	 */
	LIBGAMMA_NO = 0,

	/**
	 * The answer is unknown
	 */
	LIBGAMMA_MAYBE = 1,

	/**
	 * The answer is positive
	 */
	LIBGAMMA_YES = 2

} libgamma_decision_t;



/**
 * Capabilities of adjustment methods
 */
typedef struct libgamma_method_capabilities {
	/**
	 * OR of the CRTC information fields in `libgamma_crtc_information_t`
	 * that may (but can fail) be read successfully
	 */
	int32_t crtc_information;

	/**
	 * Whether the default site is known, if true the site is integrated
	 * to the system or can be determined using environment variables
	 */
	unsigned default_site_known : 1;

	/**
	 * Whether the adjustment method supports multiple sites rather
	 * than just the default site
	 */
	unsigned multiple_sites : 1;

	/**
	 * Whether the adjustment method supports multiple partitions
	 * per site
	 */
	unsigned multiple_partitions : 1;

	/**
	 * Whether the adjustment method supports multiple CRTC:s
	 * per partition per site
	 */
	unsigned multiple_crtcs : 1;

	/**
	 * Whether the partition to graphics card is a bijection
	 */
	unsigned partitions_are_graphics_cards : 1;

	/**
	 * Whether the adjustment method supports `libgamma_site_restore`
	 */
	unsigned site_restore : 1;

	/**
	 * Whether the adjustment method supports `libgamma_partition_restore`
	 */
	unsigned partition_restore : 1;

	/**
	 * Whether the adjustment method supports `libgamma_crtc_restore`
	 */
	unsigned crtc_restore : 1;

	/**
	 * Whether the `red_gamma_size`, `green_gamma_size` and `blue_gamma_size`
	 * fields in `libgamma_crtc_information_t` will always have the same
	 * values as each other for the adjustment method
	 */
	unsigned identical_gamma_sizes : 1;

	/**
	 * Whether the `red_gamma_size`, `green_gamma_size` and `blue_gamma_size`
	 * fields in `libgamma_crtc_information_t` will always be filled with the
	 * same value for the adjustment method
	 */
	unsigned fixed_gamma_size : 1;

	/**
	 * Whether the `gamma_depth` field in `libgamma_crtc_information_t`
	 * will always be filled with the same value for the adjustment method
	 */
	unsigned fixed_gamma_depth : 1;

	/**
	 * Whether the adjustment method will actually perform adjustments
	 */
	unsigned real : 1;

	/**
	 * Whether the adjustment method is implement using a translation layer
	 */
	unsigned fake : 1;

	/**
	 * Whether adjustments are undone when the process disconnects from
	 * the display server
	 */
	unsigned auto_restore : 1;

} libgamma_method_capabilities_t;


/**
 * Site state
 * 
 * On operating systems that integrate a graphical environment
 * there is usually just one site. However, one systems with
 * pluggable graphics, like Unix-like systems such as GNU/Linux
 * and the BSD:s, there can usually be any (feasible) number of
 * sites. In X.org parlance they are called displays.
 */
typedef struct libgamma_site_state {
	/**
	 * Adjustment method implementation specific data
	 * 
	 * You as a user of this library should not touch this
	 */
	void *data;

	/**
	 * This field specifies, for the methods if this library,
	 * which adjustment method (display server and protocol)
	 * is used to adjust the gamma ramps
	 */
	int method;

	/**
	 * The site identifier. It can either be `NULL` or a string.
	 * `NULL` indicates the default site. On systems like the
	 * Unix-like systems, where the graphics are pluggable, this
	 * is usually resolved by an environment variable, such as
	 * "DISPLAY" for X.org.
	 */
	char *site;

	/**
	 * The number of partitions that is available on this site.
	 * Probably the majority of display server only one partition
	 * per site. However, X.org can, and traditional used to have
	 * on multi-headed environments, multiple partitions per site.
	 * In X.org partitions are called 'screens'. It is not to be
	 * confused with monitor. A screen is a collection of monitors,
	 * and the mapping from monitors to screens is a surjection.
	 * On hardware-level adjustment methods, such as Direct
	 * Rendering Manager, a partition is a graphics card.
	 */
	size_t partitions_available;

} libgamma_site_state_t;


/**
 * Partition state
 * 
 * Probably the majority of display server only one partition
 * per site. However, X.org can, and traditional used to have
 * on multi-headed environments, multiple partitions per site.
 * In X.org partitions are called 'screens'. It is not to be
 * confused with monitor. A screen is a collection of monitors,
 * and the mapping from monitors to screens is a surjection.
 * On hardware-level adjustment methods, such as Direct
 * Rendering Manager, a partition is a graphics card.
 */
typedef struct libgamma_partition_state {
	/**
	 * Adjustment method implementation specific data
	 * 
	 * You as a user of this library should not touch this
	 */
	void *data;

	/**
	 * The site this partition belongs to
	 */
	libgamma_site_state_t *site;

	/**
	 * The index of the partition
	 */
	size_t partition;

	/**
	 * The number of CRTC:s that are available
	 * under this partition
	 * 
	 * Note that the CRTC:s are not necessarily
	 * online
	 */
	size_t crtcs_available;

} libgamma_partition_state_t;


/**
 * Cathode ray tube controller state
 * 
 * The CRTC controls the gamma ramps for the
 * monitor that is plugged in to the connector
 * that the CRTC belongs to
 */
typedef struct libgamma_crtc_state {
	/**
	 * Adjustment method implementation specific data
	 * 
	 * You as a user of this library should not touch this
	 */
	void *data;

	/**
	 * The partition this CRTC belongs to
	 */
	libgamma_partition_state_t *partition;

	/**
	 * The index of the CRTC within its partition
	 */
	size_t crtc;

} libgamma_crtc_state_t;


/**
 * Cathode ray tube controller information data structure
 */
typedef struct libgamma_crtc_information {

/**
 * For a `libgamma_crtc_information_t` fill in the values for
 * `edid` and `edid_length` and report errors to `edid_error`
 */
#define LIBGAMMA_CRTC_INFO_EDID (1 << 0)

	/**
	 * The Extended Display Identification Data associated with
	 * the attached monitor.
	 * 
	 * This is raw byte array that is usually 128 bytes long.
	 * It is not NUL-terminate, rather its length is stored in
	 * `edid_length`.
	 */
	unsigned char *edid;

	/**
	 * The length of `edid`
	 */
	size_t edid_length;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int edid_error;


/**
 * For a `libgamma_crtc_information_t` fill in the value
 * for `width_mm` and report errors to `width_mm_error`
 */
#define LIBGAMMA_CRTC_INFO_WIDTH_MM (1 << 1)

	/**
	 * The phyical width, in millimetres, of the viewport of the
	 * attached monitor, as reported by the adjustment method
	 * 
	 * This value may be incorrect, which is a known issue with
	 * the X server where it is the result of the X server
	 * attempting the estimate the size on its own
	 * 
	 * Zero means that its is not applicable, which is the case
	 * for projectors
	 */
	size_t width_mm;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int width_mm_error;


/**
 * For a `libgamma_crtc_information_t` fill in the value
 * for `height_mm` and report errors to `height_mm_error`
 */
#define LIBGAMMA_CRTC_INFO_HEIGHT_MM (1 << 2)

	/**
	 * The phyical height, in millimetres, of the viewport of the
	 * attached monitor, as reported by the adjustment method
	 * 
	 * This value may be incorrect, which is a known issue with
	 * the X server where it is the result of the X server
	 * attempting the estimate the size on its own
	 * 
	 * Zero means that its is not applicable, which is the case
	 * for projectors
	 */
	size_t height_mm;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int height_mm_error;


/**
 * For a `libgamma_crtc_information_t` fill in the value for
 * `width_mm_edid` and report errors to `width_mm_edid_error`
 */
#define LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID (1 << 3)

	/**
	 * The phyical width, in millimetres, of the viewport of the
	 * attached monitor, as reported by it the monitor's Extended
	 * Display Information Data
	 * 
	 * This value can only contain whole centimetres, which means
	 * that the result is always zero modulus ten. However, this
	 * could change with revisions of the EDID structure.
	 * 
	 * Zero means that its is not applicable, which is the case
	 * for projectors.
	 */
	size_t width_mm_edid;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int width_mm_edid_error;


/**
 * For a `libgamma_crtc_information_t` fill in the value for
 * `height_mm_edid` and report errors to `height_mm_edid_error`
 */
#define LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID (1 << 4)

	/**
	 * The phyical height, in millimetres, of the viewport of the
	 * attached monitor, as reported by it the monitor's Extended
	 * Display Information Data
	 * 
	 * This value can only contain whole centimetres, which means
	 * that the result is always zero modulus ten. However, this
	 * could change with revisions of the EDID structure.
	 * 
	 * Zero means that its is not applicable, which is the case
	 * for projectors
	 */
	size_t height_mm_edid;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int height_mm_edid_error;


/**
 * For a `libgamma_crtc_information_t` fill in the values for
 * `red_gamma_size`, `green_gamma_size`, and `blue_gamma_size`,
 * and report errors to `gamma_size_error`
 */
#define LIBGAMMA_CRTC_INFO_GAMMA_SIZE (1 << 5)

	/**
	 * The size of the encoding axis of the red gamma ramp
	 */
	size_t red_gamma_size;

	/**
	 * The size of the encoding axis of the green gamma ramp
	 */
	size_t green_gamma_size;

	/**
	 * The size of the encoding axis of the blue gamma ramp
	 */
	size_t blue_gamma_size;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int gamma_size_error;


/**
 * For a `libgamma_crtc_information_t` fill in the value for
 * `gamma_depth` and report errors to `gamma_depth_error`
 */
#define LIBGAMMA_CRTC_INFO_GAMMA_DEPTH (1 << 6)

	/**
	 * The bit-depth of the value axes of gamma ramps,
	 * -1 for single precision floating point, and -2 for
	 * double precision floating point
	 */
	signed gamma_depth;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int gamma_depth_error;


/**
 * For a `libgamma_crtc_information_t` fill in the value for
 * `gamma_support` and report errors to `gamma_support_error`
 */
#define LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT (1 << 7)

	/**
	 * `LIBGAMMA_NO` indicates that the CRTC does not support
	 * gamma ramp adjustments. `LIBGAMMA_MAYBE` indicates that
	 * the CRTC may or may not support gamma ramp adjustments,
	 * meaning that the display server really does not know, but
	 * the protocol is available. `LIBGAMMA_NO` indicates that
	 * the CRTC does support gamma ramp adjustments.
	 */
	libgamma_decision_t gamma_support;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int gamma_support_error;


/**
 * For a `libgamma_crtc_information_t` fill in the value for
 * `subpixel_order` and report errors to `subpixel_order_error`
 */
#define LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER (1 << 8)

	/**
	 * The layout of the subpixels
	 * 
	 * You cannot count on this value — especially for CRT:s —
	 * but it is provided anyway as a means of distinguishing
	 * monitors
	 */
	libgamma_subpixel_order_t subpixel_order;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int subpixel_order_error;


/**
 * For a `libgamma_crtc_information_t` fill in the
 * value for `active` and report errors to `active_error`
 */
#define LIBGAMMA_CRTC_INFO_ACTIVE  (1 << 9)

	/**
	 * Whether there is a monitor connected to the CRTC
	 */
	int active;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int active_error;


/**
 * For a `libgamma_crtc_information_t` fill in the value for
 * `connector_name` and report errors to `connector_name_error`
 */
#define LIBGAMMA_CRTC_INFO_CONNECTOR_NAME (1 << 10)

	/**
	 * The name of the connector as designated by the display
	 * server or as give by this library in case the display
	 * server lacks this feature.
	 */
	char *connector_name;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int connector_name_error;


/**
 * For a `libgamma_crtc_information_t` fill in the value for
 * `connector_type` and report errors to `connector_type_error`
 */
#define LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE (1 << 11)

	/**
	 * The type of the connector that is associated with the CRTC
	 */
	libgamma_connector_type_t connector_type;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int connector_type_error;


/**
 * For a `libgamma_crtc_information_t` fill in the
 * values for `gamma_red`, `gamma_green`, and `gamma_blue`
 * and report errors to `gamma_error`
 */
#define LIBGAMMA_CRTC_INFO_GAMMA (1 << 12)

	/**
	 * The gamma characteristics of the monitor as reported
	 * in its Extended Display Information Data. The value
	 * holds the value for the red channel.
	 * 
	 * If you do not have and more accurate measurement of the
	 * gamma for the monitor this could be used to give a rought
	 * gamma correction; simply divide the value with 2.2 and use
	 * the result for the red channel in the gamma correction
	 */
	float gamma_red;

	/**
	 * The gamma characteristics of the monitor as reported
	 * in its Extended Display Information Data. The value
	 * holds the value for the green channel.
	 * 
	 * If you do not have and more accurate measurement of the
	 * gamma for the monitor this could be used to give a rought
	 * gamma correction; simply divide the value with 2.2 and use
	 * the result for the green channel in the gamma correction
	 */
	float gamma_green;

	/**
	 * The gamma characteristics of the monitor as reported
	 * in its Extended Display Information Data. The value
	 * holds the value for the blue channel.
	 * 
	 * If you do not have and more accurate measurement of the
	 * gamma for the monitor this could be used to give a rought
	 * gamma correction; simply divide the value with 2.2 and use
	 * the result for the blue channel in the gamma correction
	 */
	float gamma_blue;

	/**
	 * Zero on success, positive it holds the value `errno` had
	 * when the reading failed, otherwise (negative) the value
	 * of an error identifier provided by this library
	 */
	int gamma_error;

	/* DEVELOPERS: Remember to update LIBGAMMA_CRTC_INFO_COUNT below and maybe
	 *             also some of the list of LIBGAMMA_CRTC_INFO_* macros below */

} libgamma_crtc_information_t;

/**
 * The number of `LIBGAMMA_CRTC_INFO_*` values defined in
 * the version of the library the program is compiled against
 * 
 * This exclude the combining macros defined below this macro
 */
#define LIBGAMMA_CRTC_INFO_COUNT 13

/**
 * The number of `LIBGAMMA_CRTC_INFO_*` values defined in
 * the version of the library the program is linked against
 */
extern const int libgamma_crtc_info_count;

/**
 * Macro for both `libgamma_crtc_information_t` fields
 * that can specify the size of the monitor's viewport
 * as specified in the monitor's Extended Display
 * Information Data
 */
#define LIBGAMMA_CRTC_INFO_MACRO_EDID_VIEWPORT (LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID | LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID)

/**
 * Macro for all `libgamma_crtc_information_t` fields
 * that can be filled if the adjustment method have
 * support for reading the monitors' Extended Display
 * Information Data
 */
#define LIBGAMMA_CRTC_INFO_MACRO_EDID (LIBGAMMA_CRTC_INFO_EDID | LIBGAMMA_CRTC_INFO_MACRO_EDID_VIEWPORT | LIBGAMMA_CRTC_INFO_GAMMA)

/**
 * Macro for both `libgamma_crtc_information_t` fields
 * that can specify the size of the monitor's viewport
 * as provided by the adjustment method without this
 * library having to parse the monitor's Extended Display
 * Information Data
 */
#define LIBGAMMA_CRTC_INFO_MACRO_VIEWPORT (LIBGAMMA_CRTC_INFO_WIDTH_MM | LIBGAMMA_CRTC_INFO_HEIGHT_MM)

/**
 * Macro for the `libgamma_crtc_information_t` fields
 * that specifies the CRTC's gamma ramp sizes and gamma
 * ramp depth
 */
#define LIBGAMMA_CRTC_INFO_MACRO_RAMP (LIBGAMMA_CRTC_INFO_GAMMA_SIZE | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH)

/**
 * Macro for the `libgamma_crtc_information_t` fields
 * that specifies the CRTC's connector type and the
 * partition unique name of the connector
 */
#define LIBGAMMA_CRTC_INFO_MACRO_CONNECTOR (LIBGAMMA_CRTC_INFO_CONNECTOR_NAME | LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE)

/**
 * Macro for the `libgamma_crtc_information_t` fields
 * that required there is a monitor attached to the
 * connector, and that status itself
 */
#define LIBGAMMA_CRTC_INFO_MACRO_ACTIVE (LIBGAMMA_CRTC_INFO_MACRO_EDID | LIBGAMMA_CRTC_INFO_MACRO_VIEWPORT |\
                                         LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER | LIBGAMMA_CRTC_INFO_ACTIVE)



/**
 * Gamma ramp structure for 8-bit gamma ramps
 */
typedef struct libgamma_gamma_ramps8 {
	/**
	 * The size of `red`
	 */
	size_t red_size;

	/**
	 * The size of `green`
	 */
	size_t green_size;

	/**
	 * The size of `blue`
	 */
	size_t blue_size;

	/**
	 * The gamma ramp for the red channel
	 */
	uint8_t *red;

	/**
	 * The gamma ramp for the green channel
	 */
	uint8_t *green;

	/**
	 * The gamma ramp for the blue channel
	 */
	uint8_t *blue;

} libgamma_gamma_ramps8_t;


/**
 * Gamma ramp structure for 16-bit gamma ramps
 */
typedef struct libgamma_gamma_ramps16
{
	/**
	 * The size of `red`
	 */
	size_t red_size;

	/**
	 * The size of `green`
	 */
	size_t green_size;

	/**
	 * The size of `blue`
	 */
	size_t blue_size;

	/**
	 * The gamma ramp for the red channel
	 */
	uint16_t *red;

	/**
	 * The gamma ramp for the green channel
	 */
	uint16_t *green;

	/**
	 * The gamma ramp for the blue channel
	 */
	uint16_t *blue;

} libgamma_gamma_ramps16_t;


/**
 * Gamma ramp structure for 32-bit gamma ramps
 */
typedef struct libgamma_gamma_ramps32
{
	/**
	 * The size of `red`
	 */
	size_t red_size;

	/**
	 * The size of `green`
	 */
	size_t green_size;

	/**
	 * The size of `blue`
	 */
	size_t blue_size;

	/**
	 * The gamma ramp for the red channel
	 */
	uint32_t *red;

	/**
	 * The gamma ramp for the green channel
	 */
	uint32_t *green;

	/**
	 * The gamma ramp for the blue channel
	 */
	uint32_t *blue;

} libgamma_gamma_ramps32_t;


/**
 * Gamma ramp structure for 64-bit gamma ramps
 */
typedef struct libgamma_gamma_ramps64
{
	/**
	 * The size of `red`
	 */
	size_t red_size;

	/**
	 * The size of `green`
	 */
	size_t green_size;

	/**
	 * The size of `blue`
	 */
	size_t blue_size;

	/**
	 * The gamma ramp for the red channel
	 */
	uint64_t *red;

	/**
	 * The gamma ramp for the green channel
	 */
	uint64_t *green;

	/**
	 * The gamma ramp for the blue channel
	 */
	uint64_t *blue;

} libgamma_gamma_ramps64_t;


/**
 * Gamma ramp structure for `float` gamma ramps
 */
typedef struct libgamma_gamma_rampsf
{
	/**
	 * The size of `red`
	 */
	size_t red_size;

	/**
	 * The size of `green`
	 */
	size_t green_size;

	/**
	 * The size of `blue`
	 */
	size_t blue_size;

	/**
	 * The gamma ramp for the red channel
	 */
	float *red;

	/**
	 * The gamma ramp for the green channel
	 */
	float *green;

	/**
	 * The gamma ramp for the blue channel
	 */
	float *blue;

} libgamma_gamma_rampsf_t;


/**
 * Gamma ramp structure for `double` gamma ramps
 */
typedef struct libgamma_gamma_rampsd
{
	/**
	 * The size of `red`
	 */
	size_t red_size;

	/**
	 * The size of `green`
	 */
	size_t green_size;

	/**
	 * The size of `blue`
	 */
	size_t blue_size;

	/**
	 * The gamma ramp for the red channel
	 */
	double *red;

	/**
	 * The gamma ramp for the green channel
	 */
	double *green;

	/**
	 * The gamma ramp for the blue channel
	 */
	double *blue;

} libgamma_gamma_rampsd_t;


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
 * Prints an error to stderr in a `perror` fashion
 * 
 * @param  name        The text to add at the beginning
 * @param  error_code  The error code, may be an `errno` value
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__access__(__read_only__, 1))))
void libgamma_perror(const char *, int);

/**
 * Get a description of an error
 * 
 * @param   error_code  The error code, may be an `errno` value, if
 *                      `LIBGAMMA_ERRNO_SET`, the current value of `errno`
 *                      will be used
 * @return              The description associated with the error code,
 *                      `NULL` if the error code is not recognised
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__)))
const char *libgamma_strerror(int);

/**
 * Get a description of an error
 * 
 * @param   error_code  The error code, may be an `errno` value, if
 *                      `LIBGAMMA_ERRNO_SET`, the current value of `errno`
 *                      will be used
 * @param   buf         Buffer that shall be used if a description must be generated
 * @param   bufsize     The size of `buf`, 1024 is recommended
 * @return              The description associated with the error code;
 *                      can only be `NULL` if `buf` is `NULL`. If the buffer
 *                      is insufficient, a truncated but NUL-terminated
 *                      description is returned and `errno` is set to `ERANGE`;
 *                      `errno` is otherwise unmodified
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __access__(__write_only__, 2, 3))))
const char *libgamma_strerror_r(int, char[], size_t);

/**
 * Get the name of the definition associated with a `libgamma` error code
 * 
 * @param   value  The error code
 * @return         The name of the definition associated with the error code,
 *                 `NULL` if the error code does not exist
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __const__)))
const char *libgamma_name_of_error(int);

/**
 * Get the value of a `libgamma` error definition refered to by name
 * 
 * @param   name  The name of the definition associated with the error code
 * @return        The error code, zero if the name is `NULL`
 *                or does not refer to a `libgamma` error
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __access__(__read_only__, 1), __pure__)))
int libgamma_value_of_error(const char *);

/**
 * Get the group that the user needs to be a member
 * of if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 * 
 * @return  The group that the user needs to be a member of
 *          if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__)))
#ifndef __WIN32__
gid_t libgamma_group_gid_get(void);
#else
short libgamma_group_gid_get(void);
#endif

/**
 * Set the group that the user needs to be a member
 * of if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 * 
 * @param  value  The group that the user needs to be a member of
 *                if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 */
#ifndef __WIN32__
void libgamma_group_gid_set(gid_t);
#else
void libgamma_group_gid_set(short);
#endif

/**
 * Get the group that the user needs to be a member of
 * if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 * 
 * @return  The group that the user needs to be a member of if
 *          `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned, `NULL`
 *          if the name of the group `libgamma_group_gid` cannot
 *          be determined
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__)))
const char *libgamma_group_name_get(void);

/**
 * Set the group that the user needs to be a member of
 * if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 * 
 * @param  value  The group that the user needs to be a member of if
 *                `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned, may be `NULL`
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__access__(__read_only__, 1))))
void libgamma_group_name_set(const char *);




/**
 * Get the name of an adjustment method,
 * for example "randr" for `LIBGAMMA_METHOD_X_RANDR`
 * 
 * @param   method  The adjustment method
 * @return          The name adjustment method, `NULL` if not
 *                  recognised (errno is not changed)
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __const__)))
const char *libgamma_name_of_method(int);

/**
 * Get the name of the constant for an adjustment
 * method, for example "LIBGAMMA_METHOD_X_RANDR"
 * for `LIBGAMMA_METHOD_X_RANDR`
 * 
 * @param   method  The adjustment method
 * @return          The name of the constant for adjustment method,
 *                  `NULL` if not recognised (errno is not changed)
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __const__)))
const char *libgamma_const_of_method(int);

/**
 * Check whether an adjustment method is available,
 * non-existing (invalid) methods will be identified
 * as not available under the rationale that the
 * library may be out of date
 * 
 * @param   method  The adjustment method
 * @return          Whether the adjustment method is available
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __const__)))
int libgamma_is_method_available(int);

/**
 * Get the value of an adjustment method
 * 
 * @param   method  The name of the adjustment method, for example
 *                  "randr" or "LIBGAMMA_METHOD_X_RANDR"
 * @return          The adjustment method; for example `LIBGAMMA_METHOD_X_RANDR`
 *                  for "randr" and "LIBGAMMA_METHOD_X_RANDR"
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__, __access__(__read_only__, 1), __pure__)))
int libgamma_value_of_method(const char *);



/**
 * Get the name of a connector type,
 * for example "VGA" for `LIBGAMMA_CONNECTOR_TYPE_VGA`
 * 
 * "Unknown"   is returned for `LIBGAMMA_CONNECTOR_TYPE_Unknown`,
 * "TV"        is returned for `LIBGAMMA_CONNECTOR_TYPE_TV`,
 * "Virtual"   is returned for `LIBGAMMA_CONNECTOR_TYPE_VIRTUAL`,
 * "Writeback" is returned for `LIBGAMMA_CONNECTOR_TYPE_WRITEBACK`
 * 
 * @param   connector  The connector type
 * @return             The name connector type, `NULL` if not
 *                     recognised (errno is not changed)
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __const__)))
const char *libgamma_name_of_connector_type(int);

/**
 * Get the name of the constant for a connector
 * type, for example "LIBGAMMA_CONNECTOR_TYPE_VGA"
 * for `LIBGAMMA_CONNECTOR_TYPE_VGA`
 * 
 * @param   connector  The connector type
 * @return             The name of the constant for connector type,
 *                     `NULL` if not recognised (errno is not changed)
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __const__)))
const char *libgamma_const_of_connector_type(int);

/**
 * Get the value of a connector type
 * 
 * @param   connector  The name of the connector type, for example
 *                     "VGA" or "LIBGAMMA_CONNECTOR_TYPE_VGA"
 * @return             The connector type; for example `LIBGAMMA_CONNECTOR_TYPE_VGA`
 *                     for "VGA" and "LIBGAMMA_CONNECTOR_TYPE_VGA"
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__, __access__(__read_only__, 1), __pure__)))
int libgamma_value_of_connector_type(const char *);



/**
 * Get the name of a subpixel order, for example
 * "Horizontal RGB" for `LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB`
 * 
 * "Unknown" is returned for `LIBGAMMA_SUBPIXEL_ORDER_UNKNOWN`,
 * "None"    is returned for `LIBGAMMA_SUBPIXEL_ORDER_NONE`
 * 
 * @param   order  The subpixel order
 * @return         The name subpixel order, `NULL` if not
 *                 recognised (errno is not changed)
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __const__)))
const char *libgamma_name_of_subpixel_order(int);

/**
 * Get the name of the constant for a subpixel order,
 * for example "LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB"
 * for `LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB`
 * 
 * @param   order  The subpixel order
 * @return         The name of the constant for subpixel order,
 *                 `NULL` if not recognised (errno is not changed)
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __const__)))
const char *libgamma_const_of_subpixel_order(int);

/**
 * Get the value of a subpixel order
 * 
 * @param   order  The name of the subpixel order, for example
 *                 "Horizontal RGB" or "LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB"
 * @return         The subpixel order; for example `LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB`
 *                 for "Horizontal RGB" and "LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB"
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __warn_unused_result__, __access__(__read_only__, 1), __pure__)))
int libgamma_value_of_subpixel_order(const char *);



/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation
 * 
 * The input must have `red_size`, `green_size`, and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_gamma_ramps8_initialise(libgamma_gamma_ramps8_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps8_initialise` or otherwise
 * initialised in the proper manner
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_ramps8_destroy(libgamma_gamma_ramps8_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps8_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_ramps8_free(libgamma_gamma_ramps8_t *restrict);


/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation
 * 
 * The input must have `red_size`, `green_size`, and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_gamma_ramps16_initialise(libgamma_gamma_ramps16_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps16_initialise` or otherwise
 * initialised in the proper manner
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_ramps16_destroy(libgamma_gamma_ramps16_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps16_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_ramps16_free(libgamma_gamma_ramps16_t *restrict);


/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation
 * 
 * The input must have `red_size`, `green_size`, and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated.
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_gamma_ramps32_initialise(libgamma_gamma_ramps32_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps32_initialise` or otherwise
 * initialised in the proper manner
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_ramps32_destroy(libgamma_gamma_ramps32_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps32_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_ramps32_free(libgamma_gamma_ramps32_t *restrict);


/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation.
 * 
 * The input must have `red_size`, `green_size`, and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated.
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_gamma_ramps64_initialise(libgamma_gamma_ramps64_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps64_initialise` or otherwise
 * initialised in the proper manner
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_ramps64_destroy(libgamma_gamma_ramps64_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps64_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_ramps64_free(libgamma_gamma_ramps64_t *restrict);


/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation
 * 
 * The input must have `red_size`, `green_size`, and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_gamma_rampsf_initialise(libgamma_gamma_rampsf_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsf_initialise` or otherwise
 * initialised in the proper manner
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_rampsf_destroy(libgamma_gamma_rampsf_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsf_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_rampsf_free(libgamma_gamma_rampsf_t *restrict);


/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation
 * 
 * The input must have `red_size`, `green_size`, and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_gamma_rampsd_initialise(libgamma_gamma_rampsd_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsd_initialise` or otherwise
 * initialised in the proper manner
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_rampsd_destroy(libgamma_gamma_rampsd_t *restrict);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsd_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_gamma_rampsd_free(libgamma_gamma_rampsd_t *restrict);



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
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __access__(__write_only__, 1, 2))))
size_t libgamma_list_methods(int *restrict, size_t, int);

/**
 * Return the capabilities of an adjustment method
 * 
 * @param   this    The data structure to fill with the method's capabilities
 * @param   method  The adjustment method (display server and protocol)
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__access__(__write_only__, 1))))
int libgamma_method_capabilities(libgamma_method_capabilities_t *restrict, int);

/**
 * Return the default site for an adjustment method
 * 
 * @param   method  The adjustment method (display server and protocol)
 * @return          The default site, `NULL` if it cannot be determined or
 *                  if multiple sites are not supported by the adjustment
 *                  method
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__)))
const char *libgamma_method_default_site(int);

/**
 * Return the default variable that determines
 * the default site for an adjustment method
 * 
 * @param   method  The adjustment method (display server and protocol)
 * @return          The environ variables that is used to determine the
 *                  default site, `NULL` if there is none, that is, if
 *                  the method does not support multiple sites
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __const__)))
const char *libgamma_method_default_site_variable(int);



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
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__(1))))
int libgamma_site_initialise(libgamma_site_state_t *restrict, int, char *restrict);

/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_site_destroy(libgamma_site_state_t *restrict);

/**
 * Release all resources held by a site state
 * and free the site state pointer
 * 
 * @param  this  The site state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
inline void
libgamma_site_free(libgamma_site_state_t *restrict this__)
{
	libgamma_site_destroy(this__);
	free(this__);
}

/**
 * Restore the gamma ramps all CRTC:s within a site to the system settings
 * 
 * @param   this  The site state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
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
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_partition_initialise(libgamma_partition_state_t *restrict, libgamma_site_state_t *restrict, size_t);

/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_partition_destroy(libgamma_partition_state_t *restrict);

/**
 * Release all resources held by a partition state
 * and free the partition state pointer
 * 
 * @param  this  The partition state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
inline void
libgamma_partition_free(libgamma_partition_state_t *restrict this__)
{
	libgamma_partition_destroy(this__);
	free(this__);
}

/**
 * Restore the gamma ramps all CRTC:s within a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
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
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_crtc_initialise(libgamma_crtc_state_t *restrict, libgamma_partition_state_t *restrict, size_t);

/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_crtc_destroy(libgamma_crtc_state_t *restrict);

/**
 * Release all resources held by a CRTC state
 * and free the CRTC state pointer
 * 
 * @param  this  The CRTC state
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
inline void
libgamma_crtc_free(libgamma_crtc_state_t *restrict this__)
{
	libgamma_crtc_destroy(this__);
	free(this__);
}

/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_crtc_restore(libgamma_crtc_state_t *restrict);


/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error; on error refer to the error reports in `this`
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __access__(__write_only__, 1))))
int libgamma_get_crtc_information(libgamma_crtc_information_t *restrict, libgamma_crtc_state_t *restrict, int32_t);

/**
 * Release all resources in an information data structure for a CRTC
 * 
 * @param  this  The CRTC information
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
void libgamma_crtc_information_destroy(libgamma_crtc_information_t *restrict);

/**
 * Release all resources in an information data structure for a CRTC
 * and free the data structure pointer
 * 
 * @param  this  The CRTC information
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
inline void
libgamma_crtc_information_free(libgamma_crtc_information_t *restrict this__)
{
	libgamma_crtc_information_destroy(this__);
	free(this__);
}


/**
 * Convert a raw representation of an EDID to a lowercase hexadecimal representation
 * 
 * @param   edid    The EDID in raw representation
 * @param   length  The length of `edid`
 * @return          The EDID in lowercase hexadecimal representation
 *                  `NULL` on allocation error, `errno` will be set accordingly
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __access__(__read_only__, 1, 2))))
char *libgamma_behex_edid_lowercase(const unsigned char *restrict, size_t);

/**
 * Convert a raw representation of an EDID to an uppercase hexadecimal representation
 * 
 * @param   edid    The EDID in raw representation
 * @param   length  The length of `edid`
 * @return          The EDID in uppercase hexadecimal representation,
 *                  NULL` on allocation error, `errno` will be set accordingly
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __access__(__read_only__, 1, 2))))
char *libgamma_behex_edid_uppercase(const unsigned char *restrict, size_t);

/**
 * Convert a raw representation of an EDID to a lowercase hexadecimal representation
 * 
 * @param   edid:const unsigned char*  The EDID in raw representation
 * @param   length:size_t              The length of `edid`
 * @return  :char*                     The EDID in lowercase hexadecimal representation,
 *                                     `NULL` on allocation error, `errno` will be set accordingly
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __access__(__read_only__, 1, 2))))
inline char *
libgamma_behex_edid(const unsigned char *restrict edid__, size_t length__)
{
	return libgamma_behex_edid_lowercase(edid__, length__);
}

/**
 * Convert an hexadecimal representation of an EDID to a raw representation
 * 
 * @param   edid  The EDID in hexadecimal representation
 * @return        The EDID in raw representation, it will be half the length
 *                of `edid` (the input value); `NULL` on allocation error or
 *                if the EDID is malformated, `errno` will be set accordingly
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__warn_unused_result__, __nonnull__, __access__(__read_only__, 1))))
unsigned char *libgamma_unhex_edid(const char *restrict);



/**
 * Get the current gamma ramps for a CRTC, 8-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __access__(__read_only__, 2))))
int libgamma_crtc_get_gamma_ramps8(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps8_t *restrict);

/**
 * Set the gamma ramps for a CRTC, 8-bit gamma-depth version.
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __access__(__read_only__, 2))))
int libgamma_crtc_set_gamma_ramps8__new(libgamma_crtc_state_t *restrict, const libgamma_gamma_ramps8_t *restrict);
#define libgamma_crtc_set_gamma_ramps8 libgamma_crtc_set_gamma_ramps8__new


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
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_crtc_set_gamma_ramps8_f(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps8_fun *,
                                     libgamma_gamma_ramps8_fun *, libgamma_gamma_ramps8_fun *);


/**
 * Get the current gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __hot__, __access__(__read_only__, 2))))
int libgamma_crtc_get_gamma_ramps16(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps16_t *restrict);

/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __hot__, __access__(__read_only__, 2))))
int libgamma_crtc_set_gamma_ramps16__new(libgamma_crtc_state_t *restrict, const libgamma_gamma_ramps16_t *restrict);
#define libgamma_crtc_set_gamma_ramps16 libgamma_crtc_set_gamma_ramps16__new

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
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __hot__)))
int libgamma_crtc_set_gamma_ramps16_f(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps16_fun *,
                                      libgamma_gamma_ramps16_fun *, libgamma_gamma_ramps16_fun *);


/**
 * Get the current gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __access__(__read_only__, 2))))
int libgamma_crtc_get_gamma_ramps32(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps32_t *restrict);

/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __access__(__read_only__, 2))))
int libgamma_crtc_set_gamma_ramps32__new(libgamma_crtc_state_t *restrict, const libgamma_gamma_ramps32_t *restrict);
#define libgamma_crtc_set_gamma_ramps32 libgamma_crtc_set_gamma_ramps32__new

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
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_crtc_set_gamma_ramps32_f(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps32_fun *,
                                      libgamma_gamma_ramps32_fun *, libgamma_gamma_ramps32_fun *);


/**
 * Get the current gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __access__(__read_only__, 2))))
int libgamma_crtc_get_gamma_ramps64(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps64_t *restrict);

/**
 * Set the gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __access__(__read_only__, 2))))
int libgamma_crtc_set_gamma_ramps64__new(libgamma_crtc_state_t *restrict, const libgamma_gamma_ramps64_t *restrict);
#define libgamma_crtc_set_gamma_ramps64 libgamma_crtc_set_gamma_ramps64__new

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
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_crtc_set_gamma_ramps64_f(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps64_fun *,
                                      libgamma_gamma_ramps64_fun *, libgamma_gamma_ramps64_fun *);


/**
 * Get the current gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __access__(__read_only__, 2))))
int libgamma_crtc_get_gamma_rampsf(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsf_t *restrict);

/**
 * Set the gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __access__(__read_only__, 2))))
int libgamma_crtc_set_gamma_rampsf__new(libgamma_crtc_state_t *restrict, const libgamma_gamma_rampsf_t *restrict);
#define libgamma_crtc_set_gamma_rampsf libgamma_crtc_set_gamma_rampsf__new

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
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_crtc_set_gamma_rampsf_f(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsf_fun *,
                                     libgamma_gamma_rampsf_fun *, libgamma_gamma_rampsf_fun *);


/**
 * Get the current gamma ramps for a CRTC, `double` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __access__(__read_only__, 2))))
int libgamma_crtc_get_gamma_rampsd(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsd_t *restrict);

/**
 * Set the gamma ramps for a CRTC, `double` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__, __access__(__read_only__, 2))))
int libgamma_crtc_set_gamma_rampsd__new(libgamma_crtc_state_t *restrict, const libgamma_gamma_rampsd_t *restrict);
#define libgamma_crtc_set_gamma_rampsd libgamma_crtc_set_gamma_rampsd__new

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
LIBGAMMA_GCC_ONLY__(__attribute__((__nonnull__)))
int libgamma_crtc_set_gamma_rampsd_f(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsd_fun *,
                                     libgamma_gamma_rampsd_fun *, libgamma_gamma_rampsd_fun *);


#endif
