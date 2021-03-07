/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_RANDR
#include "common.h"


/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
void
libgamma_x_randr_method_capabilities(struct libgamma_method_capabilities *restrict this)
{
	char *display = getenv("DISPLAY");
	/* Support for all information except active status and gamma ramp support.
	 * Active status can be queried but it is not guaranteed produces an up to date result. */
	this->crtc_information = LIBGAMMA_CRTC_INFO_MACRO_EDID
	                       | LIBGAMMA_CRTC_INFO_MACRO_VIEWPORT
	                       | LIBGAMMA_CRTC_INFO_MACRO_RAMP
	                       | LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER
	                       | LIBGAMMA_CRTC_INFO_MACRO_CONNECTOR;
	/* X RandR supports multiple sites, partitions and CRTC:s */
	this->default_site_known = display && *display;
	this->multiple_sites = 1;
	this->multiple_partitions = 1;
	this->multiple_crtcs = 1;
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
	/* X RandR is a real non-faked adjustment method */
	this->real = 1;
	this->fake = 0;
	/* Gamma ramp adjustments are persistent */
	this->auto_restore = 0;
}
