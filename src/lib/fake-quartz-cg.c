/* See LICENSE file for copyright and license details. */
#ifndef FAKE_LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS
# error Compiling fake-quartz-cg.c without FAKE_LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS
#endif

/* This file very sloppily translates Mac OS X Quartz calls to X RandR calls.
 * It should by no means be used, without additional modification, as a
 * part of a compatibility layer. The purpose of this file is only to make
 * it possible to test for logical errors in Max OS X specific code on
 * a GNU/Linux system under X. */


#include "fake-quartz-cg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifndef HAVE_LIBGAMMA_METHOD_X_RANDR
/* Use dummy translation */


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
CGError
CGGetOnlineDisplayList(uint32_t max_size, CGDirectDisplayID *restrict displays_out, uint32_t *restrict count_out)
{
	/* Pretend that we have 2 CRTC:s */
	uint32_t i;
	for (i = 0; i < max_size && i < 2; i++)
		displays_out[i] = (CGDirectDisplayID)i;
	*count_out = i;
	return kCGErrorSuccess;
}


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
CGError
CGSetDisplayTransferByTable(CGDirectDisplayID display, uint32_t gamma_size, const CGGammaValue *red,
                            const CGGammaValue *green, const CGGammaValue *blue)
{
	(void) display;
	(void) red;
	(void) green;
	(void) blue;

	/* We pretend that our gamma ramps are of size 256 */
	if (gamma_size != 256) {
		fprintf(stderr, "Gamma size should be 256\n");
		abort();
	}
	return kCGErrorSuccess;
}


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
CGError
CGGetDisplayTransferByTable(CGDirectDisplayID display, uint32_t gamma_size, CGGammaValue *restrict red,
                            CGGammaValue *restrict green, CGGammaValue *restrict blue, uint32_t *restrict gamma_size_out)
{
	long i;

	(void) display;

	/* We pretend that our gamma ramps are of size 256 */
	if (gamma_size != 256) {
		fprintf(stderr, "Gamma size should be 256\n");
		abort();
	}

	/* We pretend that our gamma ramps are of size 256 */
	*gamma_size_out = 256;

	/* Pretend that our gamma ramps are identity mappings */
	for (i = 0; i < 256; i++)
		red[i] = green[i] = blue[i] = (CGGammaValue)i / 255;

	return kCGErrorSuccess;
}


/**
 * Restore each display's gamma ramps to the settings in ColorSync
 */
void
CGDisplayRestoreColorSyncSettings(void)
{
	/* Do nothing */
}


/**
 * Get the number of stops in the gamma ramps for a display
 * 
 * @param   display  The ID of the display
 * @return           The number of stops in the gamma ramps
 */
uint32_t
CGDisplayGammaTableCapacity(CGDirectDisplayID display)
{
	/* We pretend that our gamma ramps are of size 256 */
	(void) display;
	return 256;
}


/**
 * Release resources used by the backend
 */
void
close_fake_quartz(void)
{
	/* Do nothing */
}


#else
/* Use translation to X RandR */


#include <xcb/xcb.h>
#include <xcb/randr.h>



/**
 * Connection to the X RandR display
 */
static xcb_connection_t *restrict connection = NULL;

/**
 * Resouces for the screen
 * 
 * We only have one screen, again this
 * is a very sloppy compatibility layer
 */
static xcb_randr_get_screen_resources_current_reply_t *restrict res_reply = NULL;

/**
 * The number of available CRTC:s
 */
static uint32_t crtc_count = 0;

/**
 * List of X RandR CRTC:s
 */
static xcb_randr_crtc_t *restrict crtcs = NULL;

/**
 * The original gamma ramps, used to emulate gamma ramp restoration to system settings
 */
static uint16_t *restrict original_ramps = NULL;



/* xcb violates the rule to never return struct:s */
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Waggregate-return"
#endif


/**
 * Get a list of all online displays on the system
 * 
 * @param   max_size      The number of elements allocated for `displays_out`
 * @param   displays_out  List ot fill with the ID for each online display on the system
 * @param   count_out     Output parameter for the number of elements stored in `displays_out`,
 *                        if `displays_out` is too small to fit all display ID:s,
 *                        `*count_out` will be `max_size`
 * @return                `kCGErrorSuccess` on success, and error number of failure
 */
CGError
CGGetOnlineDisplayList(uint32_t max_size, CGDirectDisplayID *restrict displays_out, uint32_t *restrict count_out)
{
	xcb_generic_error_t *error;
	xcb_screen_iterator_t iter;
	xcb_randr_get_screen_resources_current_cookie_t res_cookie;
	xcb_randr_get_crtc_gamma_cookie_t gamma_cookie;
	xcb_randr_get_crtc_gamma_reply_t *restrict gamma_reply;
	uint32_t i;

	/* Connect to the display and get screen data if not already done so */
	if (!connection) {
		/* Connect to the display */
		connection = xcb_connect(NULL, NULL);
		/* Get the first screen */
		iter = xcb_setup_roots_iterator(xcb_get_setup(connection));
		/* Get the resources of the screen */
		res_cookie = xcb_randr_get_screen_resources_current(connection, iter.data->root);
		res_reply = xcb_randr_get_screen_resources_current_reply(connection, res_cookie, &error);
		if (error) {
			fprintf(stderr, "Failed to open X connection\n");
			xcb_disconnect(connection);
			crtc_count = 0;
			return ~kCGErrorSuccess;
		}

		/* Get the number of CRTC:s */
		crtc_count = (uint32_t)(res_reply->num_crtcs);
		/* Get the CRTC ID:s */
		crtcs = xcb_randr_get_screen_resources_current_crtcs(res_reply);

		/* Allocate memory where we store the gamma ramps as
		 * they looked when this adjustment method was first
		 * used. This is used to emulate the functionality of
		 * `CGDisplayRestoreColorSyncSettings` which restore the
		 * all gamma ramps on the system to the system settnigs.
		 */
		original_ramps = malloc(crtc_count * 3 * 256 * sizeof(uint16_t));
		if (!original_ramps) {
			perror("malloc");
			xcb_disconnect(connection);
			crtc_count = 0;
			return ~kCGErrorSuccess;
		}

		/* Fill the gamma ramps we just allocated */
		for (i = 0; i < crtc_count; i++) {
			/* Read current gamma ramps */
			gamma_cookie = xcb_randr_get_crtc_gamma(connection, crtcs[i]);
			gamma_reply = xcb_randr_get_crtc_gamma_reply(connection, gamma_cookie, &error);
			if (error) {
				fprintf(stderr, "Failed to read gamma ramps.\n");
				xcb_disconnect(connection);
				crtc_count = 0;
				return ~kCGErrorSuccess;
			}

			/* Copy over the gamma ramps to the memory area we have allocated */
#define __DEST(C)  original_ramps + (C + 3 * i) * 256
#define __SRC(C)  xcb_randr_get_crtc_gamma_##C(gamma_reply)
			memcpy(__DEST(0), __SRC(red),   256 * sizeof(uint16_t));
			memcpy(__DEST(1), __SRC(green), 256 * sizeof(uint16_t));
			memcpy(__DEST(2), __SRC(blue),  256 * sizeof(uint16_t));
#undef __SRC
#undef __DEST

			/* Release resouces */
			free(gamma_reply);
		}
	}

	/* Return CRTC ID:s */
	for (i = 0; i < max_size && i < crtc_count; i++)
		displays_out[i] = (CGDirectDisplayID)i;

	/* Return the number of CRTC ID:s we returned */
	*count_out = i;
	return kCGErrorSuccess;
}


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
CGError
CGSetDisplayTransferByTable(CGDirectDisplayID display, uint32_t gamma_size, const CGGammaValue *red,
                            const CGGammaValue *green, const CGGammaValue *blue)
{
	xcb_void_cookie_t gamma_cookie;
	uint16_t r_int[256], g_int[256], b_int[256];
	int32_t v;
	long i;

	/* This is a sloppy compatibility layer that assumes the gamma ramp size is 256 */
	if (gamma_size != 256) {
		fprintf(stderr, "Gamma size should be 256.\n");
		abort();
	}

	/* Translate the gamma ramps from float (CoreGraphics) to 16-bit unsigned integer (X RandR) */
	for (i = 0; i < 256; i++) {
		/* Red channel */
		v = (int32_t)(red[i] * UINT16_MAX);
		r_int[i] = (uint16_t)(v < 0 ? 0 : v > UINT16_MAX ? UINT16_MAX : v);

		/* Green channel */
		v = (int32_t)(green[i] * UINT16_MAX);
		g_int[i] = (uint16_t)(v < 0 ? 0 : v > UINT16_MAX ? UINT16_MAX : v);

		/* Blue channel */
		v = (int32_t)(blue[i] * UINT16_MAX);
		b_int[i] = (uint16_t)(v < 0 ? 0 : v > UINT16_MAX ? UINT16_MAX : v);
	}

	/* Apply gamma ramps */
	gamma_cookie = xcb_randr_set_crtc_gamma_checked(connection, crtcs[display], (uint16_t)gamma_size, r_int, g_int, b_int);
	/* Check for errors */
	return !xcb_request_check(connection, gamma_cookie) ? kCGErrorSuccess : ~kCGErrorSuccess;
}


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
CGError
CGGetDisplayTransferByTable(CGDirectDisplayID display, uint32_t gamma_size, CGGammaValue *restrict red,
                            CGGammaValue *restrict green, CGGammaValue *restrict blue, uint32_t *restrict gamma_size_out)
{
	xcb_randr_get_crtc_gamma_cookie_t gamma_cookie;
	xcb_randr_get_crtc_gamma_reply_t *restrict gamma_reply;
	xcb_generic_error_t *error;
	uint16_t *restrict r_int;
	uint16_t *restrict g_int;
	uint16_t *restrict b_int;
	long i;

	/* This is a sloppy compatibility layer that assumes the gamma ramp size is 256 */
	if (gamma_size != 256) {
		fprintf(stderr, "Gamma size should be 256\n");
		abort();
	}

	/* The gamma ramp size should be returned to the caller */
	*gamma_size_out = 256;

	/* Read current gamma ramps */
	gamma_cookie = xcb_randr_get_crtc_gamma(connection, crtcs[display]);
	gamma_reply = xcb_randr_get_crtc_gamma_reply(connection, gamma_cookie, &error);
	if (error) {
		fprintf(stderr, "Failed to write gamma ramps\n");
		return ~kCGErrorSuccess;
	}

	/* Get gamma ramp values */
	r_int = xcb_randr_get_crtc_gamma_red(gamma_reply);
	g_int = xcb_randr_get_crtc_gamma_green(gamma_reply);
	b_int = xcb_randr_get_crtc_gamma_blue(gamma_reply);

	/* Translate gamma ramps to float format, that is what CoreGraphics uses */
	for (i = 0; i < 256; i++) {
		red[i]   = (CGGammaValue)r_int[i] / UINT16_MAX;
		green[i] = (CGGammaValue)g_int[i] / UINT16_MAX;
		blue[i]  = (CGGammaValue)b_int[i] / UINT16_MAX;
	}

	free(gamma_reply);
	return kCGErrorSuccess;
}


/**
 * Restore each display's gamma ramps to the settings in ColorSync
 */
void
CGDisplayRestoreColorSyncSettings(void)
{
	xcb_generic_error_t *error;
	xcb_void_cookie_t gamma_cookie;
	uint32_t i;

	/* Restore the gamma ramps on each monitor to
	 * the ramps that we used when we started */
	for (i = 0; i < crtc_count; i++) {
		/* We assume that our gamma ramps are of the size
		 * 256 (this is a sloppy compatibility layer) */
		gamma_cookie = xcb_randr_set_crtc_gamma_checked(connection, crtcs[i], 256,
		                                                &original_ramps[(0 + 3 * i) * 256],
		                                                &original_ramps[(1 + 3 * i) * 256],
		                                                &original_ramps[(2 + 3 * i) * 256]);
		if ((error = xcb_request_check(connection, gamma_cookie)))
			fprintf(stderr, "Quartz gamma reset emulation with RandR returned %i\n", error->error_code);
	}
}


/**
 * Get the number of stops in the gamma ramps for a display
 * 
 * @param   display  The ID of the display
 * @return           The number of stops in the gamma ramps
 */
uint32_t
CGDisplayGammaTableCapacity(CGDirectDisplayID display)
{
	/* We assume that our gamma ramps are of the size
	 * 256 (this is a sloppy compatibility layer) */
	(void) display;
	return 256;
}


#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif


/**
 * Release resources used by the backend
 */
void close_fake_quartz_cg(void)
{
	free(res_reply);
	res_reply = NULL;
	if (connection) {
		xcb_disconnect(connection);
		connection = NULL;
	}
	free(original_ramps);
	original_ramps = NULL;
}

#endif
