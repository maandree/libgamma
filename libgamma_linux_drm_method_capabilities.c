/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_LINUX_DRM
#include "common.h"


/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
void
libgamma_linux_drm_method_capabilities(struct libgamma_method_capabilities *restrict this)
{
	/* Support for all information except gamma ramp support */
	this->crtc_information = LIBGAMMA_CRTC_INFO_MACRO_EDID
	                       | LIBGAMMA_CRTC_INFO_MACRO_VIEWPORT
	                       | LIBGAMMA_CRTC_INFO_MACRO_RAMP
	                       | LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER
	                       | LIBGAMMA_CRTC_INFO_ACTIVE
	                       | LIBGAMMA_CRTC_INFO_MACRO_CONNECTOR;
	/* DRM supports multiple partitions and CRTC:s but not sites */
	this->default_site_known = 1;
	this->multiple_sites = 0;
	this->multiple_partitions = 1;
	this->multiple_crtcs = 1;
	/* Partitions are graphics cards in DRM */
	this->partitions_are_graphics_cards = 1;
	/* Linux does not have system restore capabilities */
	this->site_restore = 0;
	this->partition_restore = 0;
	this->crtc_restore = 0;
	/* Gamma ramp sizes are identical but not fixed */
	this->identical_gamma_sizes = 1;
	this->fixed_gamma_size = 0;
	/* Gamma ramp depths are fixed */
	this->fixed_gamma_depth = 1;
	/* DRM is a real non-faked adjustment method */
	this->real = 1;
	this->fake = 0;
	/* Gamma ramp adjustments are persistent */
	this->auto_restore = 0;
}
