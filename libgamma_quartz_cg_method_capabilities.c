/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_QUARTZ_CG
#include "common.h"


/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
void
libgamma_quartz_cg_method_capabilities(libgamma_method_capabilities_t* restrict this)
{
	/* Gamma ramps size and depth can be queried */
	this->crtc_information = LIBGAMMA_CRTC_INFO_GAMMA_SIZE | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH;
	/* Quartz/CoreGraphics does not support sites or partitions */
	this->default_site_known = 1;
	this->multiple_sites = 0;
	this->multiple_partitions = 0;
	/* Quartz/CoreGraphics does support CRTC:s */
	this->multiple_crtcs = 1;
	/* Partitions are not supported... */
	this->partitions_are_graphics_cards = 0;
	/* CoreGraphics have support for system restore */
	this->site_restore = 1;
	this->partition_restore = 1;
	/* But not for individual CRTC:s */
	this->crtc_restore = 0;
	/* Gamma ramp sizes are identifical but not fixed */
	this->identical_gamma_sizes = 1;
	this->fixed_gamma_size = 0;
	/* Gamma ramp depths are fixed */
	this->fixed_gamma_depth = 1;
	/* Quartz/CoreGraphics is a real adjustment method that can be faked */
#ifdef FAKE_LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS
	/* It is considered real but fake if it is translated to X RandR */
	this->fake = 1;
# ifdef HAVE_LIBGAMMA_METHOD_X_RANDR
	this->real = 1;
# else
	this->real = 0;
# endif
#else
	/* It is real and not fake if we are running on Mac OS X */
	this->fake = 0;
	this->real = 1;
#endif
	/* Gamma ramp adjustments are non-persistent */
	this->auto_restore = 1;
}
