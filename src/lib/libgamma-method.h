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
#ifndef LIBGAMMA_METHOD_H
#define LIBGAMMA_METHOD_H

#if !defined(LIBGAMMA_CONFIG_H) && !defined(DEBUG)
# error libgamma-method.h should not be included directly, include libgamma.h instead
#endif


#ifndef __GCC__
# define __attribute__(x)
#endif

#include <stddef.h>
#include <stdint.h>



/**
 * The identifier for the dummy adjustment method.
 * This method can be configured and is useful for
 * testing your program's ability to handle errors.
 */
#define LIBGAMMA_METHOD_DUMMY  0

/**
 * The identifier for the adjustment method with
 * uses the RandR protocol under the X display server.
 */
#define LIBGAMMA_METHOD_X_RANDR  1

/**
 * The identifier for the adjustment method with
 * uses the VidMode protocol under the X display server.
 * This is an older alternative to RandR that can
 * work on some drivers that are not supported by RandR,
 * however it can only control the primary CRTC per
 * screen (partition).
 */
#define LIBGAMMA_METHOD_X_VIDMODE  2

/**
 * The identifier for the Direct Rendering Manager
 * adjustment method that is available in Linux
 * (built in to the Linux kernel with a userland
 * library for access) and is a part of the
 * Direct Rendering Infrastructure. This adjustment
 * method all work when you are in non-graphical
 * mode; however a display server cannnot be
 * started while this is running, but it can be
 * started while a display server is running.
 */
#define LIBGAMMA_METHOD_LINUX_DRM  3

/**
 * The identifier for the Graphics Device Interface
 * adjustment method that is available in Windows.
 * This method is not well tested; it can be compiled
 * to be available under X.org using a translation layer.
 */
#define LIBGAMMA_METHOD_W32_GDI  4

/**
 * The identifier for the CoreGraphics adjustment
 * method that is available in Mac OS X that can
 * adjust gamma ramps under the Quartz display server.
 * This method is not well tested; it can be compiled
 * to be available under X.org using a translation layer.
 */
#define LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS  5


/**
 * The index of the last gamma method, neither it
 * nor any index before it may actually be supported
 * as it could have been disabled at compile-time.
 */
#define LIBGAMMA_METHOD_MAX  5

/**
 * The number adjustment methods provided by this library.
 * Note however that this includes adjstment methods that
 * have been removed at compile-time.
 */
#define LIBGAMMA_METHOD_COUNT  (LIBGAMMA_METHOD_MAX + 1)



/**
 * Capabilities of adjustment methods.
 */
typedef struct libgamma_method_capabilities
{
  /**
   * OR of the CRTC information fields in `libgamma_crtc_information_t`
   * that may (but can fail) be read successfully.
   */
  int32_t crtc_information;
  
  /**
   * Whether the default site is known, if true the site is integrated
   * to the system or can be determined using environment variables.
   */
  unsigned default_site_known : 1;
  
  /**
   * Whether the adjustment method supports multiple sites rather
   * than just the default site.
   */
  unsigned multiple_sites : 1;
  
  /**
   * Whether the adjustment method supports multiple partitions
   * per site.
   */
  unsigned multiple_partitions : 1;
  
  /**
   * Whether the adjustment method supports multiple CRTC:s
   * per partition per site.
   */
  unsigned multiple_crtcs : 1;
  
  /**
   * Whether the partition to graphics card is a bijection.
   */
  unsigned partitions_are_graphics_cards : 1;
  
  /**
   * Whether the adjustment method supports `libgamma_site_restore`.
   */
  unsigned site_restore : 1;
  
  /**
   * Whether the adjustment method supports `libgamma_partition_restore`.
   */
  unsigned partition_restore : 1;
  
  /**
   * Whether the adjustment method supports `libgamma_crtc_restore`.
   */
  unsigned crtc_restore : 1;
    
  /**
   * Whether the `red_gamma_size`, `green_gamma_size` and `blue_gamma_size`
   * fields in `libgamma_crtc_information_t` will always have the same
   * values as each other for the adjustment method.
   */
  unsigned identical_gamma_sizes : 1;
  
  /**
   * Whether the `red_gamma_size`, `green_gamma_size` and `blue_gamma_size`
   * fields in `libgamma_crtc_information_t` will always be filled with the
   * same value for the adjustment method.
   */
  unsigned fixed_gamma_size : 1;
  
  /**
   * Whether the `gamma_depth` field in `libgamma_crtc_information_t`
   * will always be filled with the same value for the adjustment method.
   */
  unsigned fixed_gamma_depth : 1;
  
  /**
   * Whether the adjustment method will actually perform adjustments.
   */
  unsigned real : 1;
  
  /**
   * Whether the adjustment method is implement using a translation layer.
   */
  unsigned fake : 1;
  
} libgamma_method_capabilities_t;


/**
 * Site state.
 * 
 * On operating systems that integrate a graphical environment
 * there is usually just one site. However, one systems with
 * pluggable graphics, like Unix-like systems such as GNU/Linux
 * and the BSD:s, there can usually be any (feasible) number of
 * sites. In X.org parlance they are called displays.
 */
typedef struct libgamma_site_state
{
  /**
   * Adjustment method implementation specific data.
   * You as a user of this library should not touch this.
   */
  void* data;
  
  /**
   * This field specifies, for the methods if this library,
   * which adjustment method (display server and protocol)
   * is used to adjust the gamma ramps.
   */
  int method;
  
  /**
   * The site identifier. It can either be `NULL` or a string.
   * `NULL` indicates the default site. On systems like the
   * Unix-like systems, where the graphics are pluggable, this
   * is usually resolved by an environment variable, such as
   * "DISPLAY" for X.org.
   */
  char* site;
  
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
 * Partition state.
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
typedef struct libgamma_partition_state
{
  /**
   * Adjustment method implementation specific data.
   * You as a user of this library should not touch this.
   */
  void* data;
  
  /**
   * The site this partition belongs to.
   */
  libgamma_site_state_t* site;
  
  /**
   * The index of the partition.
   */
  size_t partition;
  
  /**
   * The number of CRTC:s that are available under this
   * partition. Note that the CRTC:s are not necessarily
   * online.
   */
  size_t crtcs_available;
  
} libgamma_partition_state_t;


/**
 * Cathode ray tube controller state.
 * 
 * The CRTC controls the gamma ramps for the
 * monitor that is plugged in to the connector
 * that the CRTC belongs to.
 */
typedef struct libgamma_crtc_state
{
  /**
   * Adjustment method implementation specific data.
   * You as a user of this library should not touch this.
   */
  void* data;
  
  /**
   * The partition this CRTC belongs to.
   */
  libgamma_partition_state_t* partition;
  
  /**
   * The index of the CRTC within its partition.
   */
  size_t crtc;
  
} libgamma_crtc_state_t;


/**
 * Types for connectors.
 */
typedef enum libgamma_connector_type
  {
    /**
     * The adjustment method does not know the connector's type
     * (This could be considered an error).
     */
    LIBGAMMA_CONNECTOR_TYPE_Unknown = 0,
    
    /**
     * Video Graphics Array (VGA).
     */
    LIBGAMMA_CONNECTOR_TYPE_VGA,
    
    /**
     * Digital Visual Interface, unknown type.
     */
    LIBGAMMA_CONNECTOR_TYPE_DVI,
    
    /**
     * Digital Visual Interface, integrated (DVI-I).
     */
    LIBGAMMA_CONNECTOR_TYPE_DVII,
    
    /**
     * Digital Visual Interface, digital only (DVI-D).
     */
    LIBGAMMA_CONNECTOR_TYPE_DVID,
    
    /**
     * Digital Visual Interface, analogue only (DVI-A).
     */
    LIBGAMMA_CONNECTOR_TYPE_DVIA,
    
    /**
     * Composite video.
     */
    LIBGAMMA_CONNECTOR_TYPE_Composite,
    
    /**
     * Separate Video (S-video).
     */
    LIBGAMMA_CONNECTOR_TYPE_SVIDEO,
    
    /**
     * Low-voltage differential signaling (LVDS).
     */
    LIBGAMMA_CONNECTOR_TYPE_LVDS,
    
    /**
     * Component video, usually separate cables for each channel.
     */
    LIBGAMMA_CONNECTOR_TYPE_Component,
    
    /**
     * 9 pin DIN (Deutsches Institut für Normung) connector.
     */
    LIBGAMMA_CONNECTOR_TYPE_9PinDIN,
    
    /**
     * DisplayPort.
     */
    LIBGAMMA_CONNECTOR_TYPE_DisplayPort,
    
    /**
     * High-Definition Multimedia Interface (HDMI), unknown type.
     */
    LIBGAMMA_CONNECTOR_TYPE_HDMI,
    
    /**
     * High-Definition Multimedia Interface, type A (HDMI-A).
     */
    LIBGAMMA_CONNECTOR_TYPE_HDMIA,
    
    /**
     * High-Definition Multimedia Interface, type B (HDMI-B).
     */
    LIBGAMMA_CONNECTOR_TYPE_HDMIB,
    
    /**
     * Television, unknown connector.
     */
    LIBGAMMA_CONNECTOR_TYPE_TV,
    
    /**
     * Embedded DisplayPort (eDP).
     */
    LIBGAMMA_CONNECTOR_TYPE_eDP,
    
    /**
     * A virtual connector.
     */
    LIBGAMMA_CONNECTOR_TYPE_VIRTUAL,
    
    /**
     * Display Serial Interface (DSI).
     */
    LIBGAMMA_CONNECTOR_TYPE_DSI,
    
    /**
     * LFP connector.
     * (If you know what this is add it to Wikipedia.)
     */
    LIBGAMMA_CONNECTOR_TYPE_LFP
    
  } libgamma_connector_type_t;

/**
 * The number of values defined in `libgamma_connector_type_t`.
 */
#define LIBGAMMA_CONNECTOR_TYPE_COUNT  20

/**
 * Orders for subpixels. Currently the possible values are
 * very biased to LCD, Plasma and monochrome monitors.
 */
typedef enum libgamma_subpixel_order
  {
    /**
     * The adjustment method does not know the order of the subpixels.
     * (This could be considered an error.)
     */
    LIBGAMMA_SUBPIXEL_ORDER_UNKNOWN = 0,
    
    /**
     * There are no subpixels in the monitor.
     */
    LIBGAMMA_SUBPIXEL_ORDER_NONE,
    
    /**
     * The subpixels are ordered red, green and then blue, from left to right.
     */
    LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB,
    
    /**
     * The subpixels are ordered blue, green and then red, from left to right.
     */
    LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_BGR,
    
    /**
     * The subpixels are ordered red, green and then blue, from the top down.
     */
    LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_RGB,
    
    /**
     * The subpixels are ordered blue, green and then red, from the top down.
     */
    LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_BGR
    
  } libgamma_subpixel_order_t;

/**
 * The number of values defined in `libgamma_subpixel_order_t`.
 */
#define LIBGAMMA_SUBPIXEL_ORDER_COUNT  6


/**
 * For a `libgamma_crtc_information_t` fill in the
 * values for `edid` and `edid_length` and report errors to `edid_error`.
 */
#define LIBGAMMA_CRTC_INFO_EDID  (1 << 0)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * value for `width_mm` and report errors to `width_mm_error`.
 */
#define LIBGAMMA_CRTC_INFO_WIDTH_MM  (1 << 1)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * value for `height_mm` and report errors to `height_mm_error`.
 */
#define LIBGAMMA_CRTC_INFO_HEIGHT_MM  (1 << 2)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * value for `width_mm_edid` and report errors to `width_mm_edid_error`.
 */
#define LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID  (1 << 3)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * value for `height_mm_edid` and report errors to `height_mm_edid_error`.
 */
#define LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID  (1 << 4)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * values for `red_gamma_size`, `green_gamma_size` and `blue_gamma_size`.
 * and report errors to `gamma_size_error`.
 */
#define LIBGAMMA_CRTC_INFO_GAMMA_SIZE  (1 << 5)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * value for `gamma_depth` and report errors to `gamma_depth_error`.
 */
#define LIBGAMMA_CRTC_INFO_GAMMA_DEPTH  (1 << 6)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * value for `gamma_support` and report errors to `gamma_support_error`.
 */
#define LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT  (1 << 7)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * value for `subpixel_order` and report errors to `subpixel_order_error`.
 */
#define LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER  (1 << 8)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * value for `active` and report errors to `active_error`.
 */
#define LIBGAMMA_CRTC_INFO_ACTIVE  (1 << 9)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * value for `connector_name` and report errors to `connector_name_error`.
 */
#define LIBGAMMA_CRTC_INFO_CONNECTOR_NAME  (1 << 10)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * value for `connector_type` and report errors to `connector_type_error`.
 */
#define LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE  (1 << 11)

/**
 * For a `libgamma_crtc_information_t` fill in the
 * values for `gamma_red`, `gamma_green` and `gamma_blue`
 * and report errors to `gamma_error`.
 */
#define LIBGAMMA_CRTC_INFO_GAMMA  (1 << 12)

/**
 * The number of `LIBGAMMA_CRTC_INFO_*` values defined.
 */
#define LIBGAMMA_CRTC_INFO_COUNT  13

/**
 * Macro for both `libgamma_crtc_information_t` fields
 * that can specify the size of the monitor's viewport
 * as specified in the monitor's Extended Display
 * Information Data.
 */
#define LIBGAMMA_CRTC_INFO_MACRO_EDID_VIEWPORT  ( LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID   \
						| LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID  )

/**
 * Macro for all `libgamma_crtc_information_t` fields
 * that can be filled if the adjustment method have
 * support for reading the monitors' Extended Display
 * Information Data.
 */
#define LIBGAMMA_CRTC_INFO_MACRO_EDID  ( LIBGAMMA_CRTC_INFO_EDID                 \
				       | LIBGAMMA_CRTC_INFO_MACRO_EDID_VIEWPORT  \
				       | LIBGAMMA_CRTC_INFO_GAMMA                )

/**
 * Macro for both `libgamma_crtc_information_t` fields
 * that can specify the size of the monitor's viewport
 * as provided by the adjustment method without this
 * library having to parse the monitor's Extended Display
 * Information Data.
 */
#define LIBGAMMA_CRTC_INFO_MACRO_VIEWPORT  (LIBGAMMA_CRTC_INFO_WIDTH_MM | LIBGAMMA_CRTC_INFO_HEIGHT_MM)

/**
 * Macro for the `libgamma_crtc_information_t` fields
 * that specifies the CRTC's gamma ramp sizes and gamma
 * ramp depth.
 */
#define LIBGAMMA_CRTC_INFO_MACRO_RAMP  (LIBGAMMA_CRTC_INFO_GAMMA_SIZE | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH)

/**
 * Macro for the `libgamma_crtc_information_t` fields
 * that specifies the CRTC's connector type and the
 * partition unique name of the connector.
 */
#define LIBGAMMA_CRTC_INFO_MACRO_CONNECTOR  ( LIBGAMMA_CRTC_INFO_CONNECTOR_NAME  \
					    | LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE  )

/**
 * Macro for the `libgamma_crtc_information_t` fields
 * that required there is a monitor attached to the connector,
 * and that status itself.
 */
#define LIBGAMMA_CRTC_INFO_MACRO_ACTIVE  ( LIBGAMMA_CRTC_INFO_MACRO_EDID      \
					 | LIBGAMMA_CRTC_INFO_MACRO_VIEWPORT  \
					 | LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER  \
					 | LIBGAMMA_CRTC_INFO_ACTIVE          )



/**
 * Cathode ray tube controller information data structure.
 */
typedef struct libgamma_crtc_information
{
  /**
   * The Extended Display Identification Data associated with
   * the attached monitor. This is raw byte array that is usually
   * 128 bytes long. It is not NUL-terminate, rather its length
   * is stored in `edid_length`.
   */
  unsigned char* edid;
  
  /**
   * The length of `edid`.
   */
  size_t edid_length;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int edid_error;
  
  
  /**
   * The phyical width, in millimetres, of the viewport of the
   * attached monitor, as reported by the adjustment method. This
   * value may be incorrect, which is a known issue with the X
   * server where it is the result of the X server attempting
   * the estimate the size on its own.
   * Zero means that its is not applicable, which is the case
   * for projectors.
   */
  size_t width_mm;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int width_mm_error;
  
  
  /**
   * The phyical height, in millimetres, of the viewport of the
   * attached monitor, as reported by the adjustment method. This
   * value may be incorrect, which is a known issue with the X
   * server where it is the result of the X server attempting
   * the estimate the size on its own.
   * Zero means that its is not applicable, which is the case
   * for projectors.
   */
  size_t height_mm;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int height_mm_error;
  
  
  /**
   * The phyical width, in millimetres, of the viewport of the
   * attached monitor, as reported by it the monitor's Extended
   * Display Information Data. This value can only contain whole
   * centimetres, which means that the result is always zero
   * modulus ten. However, this could change with revisions of
   * the EDID structure.
   * Zero means that its is not applicable, which is the case
   * for projectors.
   */
  size_t width_mm_edid;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int width_mm_edid_error;
  
  
  /**
   * The phyical height, in millimetres, of the viewport of the
   * attached monitor, as reported by it the monitor's Extended
   * Display Information Data. This value can only contain whole
   * centimetres, which means that the result is always zero
   * modulus ten. However, this could change with revisions of
   * the EDID structure.
   * Zero means that its is not applicable, which is the case
   * for projectors.
   */
  size_t height_mm_edid;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int height_mm_edid_error;
  
  
  /**
   * The size of the encoding axis of the red gamma ramp.
   */
  size_t red_gamma_size;
  
  /**
   * The size of the encoding axis of the green gamma ramp.
   */
  size_t green_gamma_size;
  
  /**
   * The size of the encoding axis of the blue gamma ramp.
   */
  size_t blue_gamma_size;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int gamma_size_error;
  
  
  /**
   * The bit-depth of the value axes of gamma ramps,
   * -1 for single precision floating point, and -2 for
   * double precision floating point.
   */
  signed gamma_depth;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int gamma_depth_error;
  
  
  /**
   * Non-zero gamma ramp adjustments are supported.
   */
  int gamma_support;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int gamma_support_error;
  
  
  /**
   * The layout of the subpixels.
   * You cannot count on this value --- especially for CRT:s ---
   * but it is provided anyway as a means of distinguishing monitors.
   */
  libgamma_subpixel_order_t subpixel_order;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int subpixel_order_error;
  
  
  /**
   * Whether there is a monitors connected to the CRTC.
   */
  int active;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int active_error;
  
  
  /**
   * The name of the connector as designated by the display
   * server or as give by this library in case the display
   * server lacks this feature.
   */
  char* connector_name;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int connector_name_error;
  
  
  /**
   * The type of the connector that is associated with the CRTC.
   */
  libgamma_connector_type_t connector_type;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int connector_type_error;
  
  
  /**
   * The gamma characteristics of the monitor as reported
   * in its Extended Display Information Data. The value
   * holds the value for the red channel. If you do not have
   * and more accurate measurement of the gamma for the
   * monitor this could be used to give a rought gamma
   * correction; simply divide the value with 2.2 and use
   * the result for the red channel in the gamma correction.
   */
  float gamma_red;
  
  /**
   * The gamma characteristics of the monitor as reported
   * in its Extended Display Information Data. The value
   * holds the value for the green channel. If you do not have
   * and more accurate measurement of the gamma for the
   * monitor this could be used to give a rought gamma
   * correction; simply divide the value with 2.2 and use
   * the result for the green channel in the gamma correction.
   */
  float gamma_green;
  
  /**
   * The gamma characteristics of the monitor as reported
   * in its Extended Display Information Data. The value
   * holds the value for the blue channel. If you do not have
   * and more accurate measurement of the gamma for the
   * monitor this could be used to give a rought gamma
   * correction; simply divide the value with 2.2 and use
   * the result for the blue channel in the gamma correction.
   */
  float gamma_blue;
  
  /**
   * Zero on success, positive it holds the value `errno` had
   * when the reading failed, otherwise (negative) the value
   * of an error identifier provided by this library.
   */
  int gamma_error;
  
} libgamma_crtc_information_t;



/**
 * Gamma ramp structure for 8-bit gamma ramps.
 */
typedef struct libgamma_gamma_ramps8
{
  /**
   * The size of `red`.
   */
  size_t red_size;
  
  /**
   * The size of `green`.
   */
  size_t green_size;
  
  /**
   * The size of `blue`.
   */
  size_t blue_size;
  
  /**
   * The gamma ramp for the red channel.
   */
  uint8_t* red;
  
  /**
   * The gamma ramp for the green channel.
   */
  uint8_t* green;
  
  /**
   * The gamma ramp for the blue channel.
   */
  uint8_t* blue;
  
} libgamma_gamma_ramps8_t;


/**
 * Gamma ramp structure for 16-bit gamma ramps.
 */
typedef struct libgamma_gamma_ramps16
{
  /**
   * The size of `red`.
   */
  size_t red_size;
  
  /**
   * The size of `green`.
   */
  size_t green_size;
  
  /**
   * The size of `blue`.
   */
  size_t blue_size;
  
  /**
   * The gamma ramp for the red channel.
   */
  uint16_t* red;
  
  /**
   * The gamma ramp for the green channel.
   */
  uint16_t* green;
  
  /**
   * The gamma ramp for the blue channel.
   */
  uint16_t* blue;
  
} libgamma_gamma_ramps16_t;


/**
 * Gamma ramp structure for 32-bit gamma ramps.
 */
typedef struct libgamma_gamma_ramps32
{
  /**
   * The size of `red`.
   */
  size_t red_size;
  
  /**
   * The size of `green`.
   */
  size_t green_size;
  
  /**
   * The size of `blue`.
   */
  size_t blue_size;
  
  /**
   * The gamma ramp for the red channel.
   */
  uint32_t* red;
  
  /**
   * The gamma ramp for the green channel.
   */
  uint32_t* green;
  
  /**
   * The gamma ramp for the blue channel.
   */
  uint32_t* blue;
  
} libgamma_gamma_ramps32_t;


/**
 * Gamma ramp structure for 64-bit gamma ramps.
 */
typedef struct libgamma_gamma_ramps64
{
  /**
   * The size of `red`.
   */
  size_t red_size;
  
  /**
   * The size of `green`.
   */
  size_t green_size;
  
  /**
   * The size of `blue`.
   */
  size_t blue_size;
  
  /**
   * The gamma ramp for the red channel.
   */
  uint64_t* red;
  
  /**
   * The gamma ramp for the green channel.
   */
  uint64_t* green;
  
  /**
   * The gamma ramp for the blue channel.
   */
  uint64_t* blue;
  
} libgamma_gamma_ramps64_t;


/**
 * Gamma ramp structure for `float` gamma ramps.
 */
typedef struct libgamma_gamma_rampsf
{
  /**
   * The size of `red`.
   */
  size_t red_size;
  
  /**
   * The size of `green`.
   */
  size_t green_size;
  
  /**
   * The size of `blue`.
   */
  size_t blue_size;
  
  /**
   * The gamma ramp for the red channel.
   */
  float* red;
  
  /**
   * The gamma ramp for the green channel.
   */
  float* green;
  
  /**
   * The gamma ramp for the blue channel.
   */
  float* blue;
  
} libgamma_gamma_rampsf_t;


/**
 * Gamma ramp structure for `double` gamma ramps.
 */
typedef struct libgamma_gamma_rampsd
{
  /**
   * The size of `red`.
   */
  size_t red_size;
  
  /**
   * The size of `green`.
   */
  size_t green_size;
  
  /**
   * The size of `blue`.
   */
  size_t blue_size;
  
  /**
   * The gamma ramp for the red channel.
   */
  double* red;
  
  /**
   * The gamma ramp for the green channel.
   */
  double* green;
  
  /**
   * The gamma ramp for the blue channel.
   */
  double* blue;
  
} libgamma_gamma_rampsd_t;



/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation.
 * 
 * The input must have `red_size`, `green_size` and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated.
 * 
 * @param   this  The gamma ramps.
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly.
 */
int libgamma_gamma_ramps8_initialise(libgamma_gamma_ramps8_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps8_initialise` or otherwise
 * initialised in the proper manner.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_ramps8_destroy(libgamma_gamma_ramps8_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps8_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_ramps8_free(libgamma_gamma_ramps8_t* restrict this);


/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation.
 * 
 * The input must have `red_size`, `green_size` and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated.
 * 
 * @param   this  The gamma ramps.
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly.
 */
int libgamma_gamma_ramps16_initialise(libgamma_gamma_ramps16_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps16_initialise` or otherwise
 * initialised in the proper manner.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_ramps16_destroy(libgamma_gamma_ramps16_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps16_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_ramps16_free(libgamma_gamma_ramps16_t* restrict this);


/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation.
 * 
 * The input must have `red_size`, `green_size` and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated.
 * 
 * @param   this  The gamma ramps.
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly.
 */
int libgamma_gamma_ramps32_initialise(libgamma_gamma_ramps32_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps32_initialise` or otherwise
 * initialised in the proper manner.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_ramps32_destroy(libgamma_gamma_ramps32_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps32_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_ramps32_free(libgamma_gamma_ramps32_t* restrict this);


/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation.
 * 
 * The input must have `red_size`, `green_size` and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated.
 * 
 * @param   this  The gamma ramps.
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly.
 */
int libgamma_gamma_ramps64_initialise(libgamma_gamma_ramps64_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps64_initialise` or otherwise
 * initialised in the proper manner.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_ramps64_destroy(libgamma_gamma_ramps64_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps64_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_ramps64_free(libgamma_gamma_ramps64_t* restrict this);


/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation.
 * 
 * The input must have `red_size`, `green_size` and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated.
 * 
 * @param   this  The gamma ramps.
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly.
 */
int libgamma_gamma_rampsf_initialise(libgamma_gamma_rampsf_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsf_initialise` or otherwise
 * initialised in the proper manner.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_rampsf_destroy(libgamma_gamma_rampsf_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsf_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_rampsf_free(libgamma_gamma_rampsf_t* restrict this);


/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation.
 * 
 * The input must have `red_size`, `green_size` and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated.
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly.
 */
int libgamma_gamma_rampsd_initialise(libgamma_gamma_rampsd_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsd_initialise` or otherwise
 * initialised in the proper manner.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_rampsd_destroy(libgamma_gamma_rampsd_t* restrict this);

/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsd_initialise` or otherwise
 * initialised in the proper manner, as well as release the pointer
 * to the structure.
 * 
 * @param  this  The gamma ramps.
 */
void libgamma_gamma_rampsd_free(libgamma_gamma_rampsd_t* restrict this);



#ifndef __GCC__
# undefine __attribute__
#endif

#endif

