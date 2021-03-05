/* See LICENSE file for copyright and license details. */
#ifdef IN_LIBGAMMA_QUARTZ_CG


/* This header file contains some capabilities of
 * <CoreGraphics/CGDirectDisplay.h> and <CoreGraphics/CGError.h>,
 * and can be used modify gamma ramps without Mac OS X and Quartz
 * but with its API.
 * 
 * The content of this file is based on the documentation found on:
 * 
 * https://developer.apple.com/library/mac/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/Reference/reference.html
 * 
 * https://developer.apple.com/library/mac/documentation/CoreGraphics/Reference/CoreGraphicsConstantsRef/Reference/reference.html#//apple_ref/c/tdef/CGError */


/**
 * Numerical `typedef` for errors that occur in CoreGraphics calls
 */
typedef int32_t CGError;

/**
 * The call was successful
 */
#define kCGErrorSuccess 0

/**
 * The data type that is used for the values in the gamma ramps
 */
typedef float CGGammaValue;

/**
 * The data type for display ID:s
 */
typedef uint32_t CGDirectDisplayID;



/**
 * Get a list of all online displays on the system
 * 
 * @param   max_size      The number of elements allocated for `displays_out`
 * @param   displays_out  List ot fill with the ID for each online display on the system
 * @param   count_out     Output parameter for the number of elements stored in `displays_out`
 *                        if `displays_out` is too small to fit all display ID:s,
 *                        `*count_out` will be `max_size`
 * @return                `kCGErrorSuccess` on success, and error number of failure
 */
CGError CGGetOnlineDisplayList(uint32_t, CGDirectDisplayID *restrict, uint32_t *restrict);

/**
 * Set the gamma ramps for a display
 * 
 * @param   display     The ID of the display
 * @param   gamma_size  The number of stops in gamma ramps
 * @param   red         The gamma ramp for the red channel
 * @param   green       The gamma ramp for the green channel
 * @param   blue        The gamma ramp for the blue channel
 * @return              `kCGErrorSuccess` on success, and error number of failure
 */
CGError CGSetDisplayTransferByTable(CGDirectDisplayID, uint32_t, const CGGammaValue *, const CGGammaValue *, const CGGammaValue *);

/**
 * Get the current gamma ramps for a display
 * 
 * @param   display         The ID of the display
 * @param   gamma_size      The number of stops you have allocated for the gamma ramps
 * @param   red             Table allocated for the gamma ramp for the red channel
 * @param   green           Table allocated for the gamma ramp for the green channel
 * @param   blue            Table allocated for the gamma ramp for the blue channel
 * @param   gamma_size_out  Output parameter for the actual number of stops in the gamma ramps
 * @return                  `kCGErrorSuccess` on success, and error number of failure
 */
CGError CGGetDisplayTransferByTable(CGDirectDisplayID, uint32_t, CGGammaValue *restrict,
                                    CGGammaValue *restrict, CGGammaValue *restrict, uint32_t *restrict);

/**
 * Restore each display's gamma ramps to the settings in ColorSync
 */
void CGDisplayRestoreColorSyncSettings(void);

/**
 * Get the number of stops in the gamma ramps for a display
 * 
 * @param   display  The ID of the display
 * @return           The number of stops in the gamma ramps
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__const__)))
uint32_t CGDisplayGammaTableCapacity(CGDirectDisplayID);


/* The follow part most only be used when this module is used,
 * it cannot be used when the real CoreGraphics is used.
 * CoreGraphics does not have this function, it is added so
 * that there is a way to cleanly close the X connection
 * and free resources needed by this module. */

/**
 * Release resources used by the backend
 */
void close_fake_quartz_cg(void);


#endif
