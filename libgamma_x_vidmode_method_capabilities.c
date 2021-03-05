/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
void
libgamma_x_vidmode_method_capabilities(libgamma_method_capabilities_t *restrict this)
{
	char *restrict display = getenv("DISPLAY");
	/* Gamma ramps size and depth can be queried */
	this->crtc_information = LIBGAMMA_CRTC_INFO_GAMMA_SIZE | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH;
	/* X VidMode supports multiple sites and partitions but not CRTC:s */
	this->default_site_known = (display && *display) ? 1 : 0;
	this->multiple_sites = 1;
	this->multiple_partitions = 1;
	this->multiple_crtcs = 0;
	/* Partitions are screens and not graphics cards in X */
	this->partitions_are_graphics_cards = 0;
	/* X does not have system restore capabilities */
	this->site_restore = 0;
	this->partition_restore = 0;
	this->crtc_restore = 0;
	/* Gamma ramp sizes are identical but not fixed */
	this->identical_gamma_sizes = 1;
	this->fixed_gamma_size = 0;
	/* Gamma ramp depths are fixed */
	this->fixed_gamma_depth = 1;
	/* X VidMode is a real non-faked adjustment method */
	this->real = 1;
	this->fake = 0;
	/* Gamma ramp adjustments are persistent */
	this->auto_restore = 0;
}
