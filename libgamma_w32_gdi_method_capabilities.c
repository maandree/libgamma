/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_W32_GDI
#include "common.h"


/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
void
libgamma_w32_gdi_method_capabilities(struct libgamma_method_capabilities *restrict this)
{
	/* Gamma ramps size, depth and support can be queried */
	this->crtc_information = LIBGAMMA_CRTC_INFO_GAMMA_SIZE | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH;
	/* Windows GDI does not support sites or partitions */
	this->default_site_known = 1;
	this->multiple_sites = 0;
	this->multiple_partitions = 0;
	/* Windows GDI does support CRTC:s */
	this->multiple_crtcs = 1;
	/* Partitions are not supported... */
	this->partitions_are_graphics_cards = 0;
	/* Windows GDI does not have system restore capabilities */
	this->site_restore = 0;
	this->partition_restore = 0;
	this->crtc_restore = 0;
	/* Ramps sizes are fixed and identical and ramp depth is too */
	this->identical_gamma_sizes = 1;
	this->fixed_gamma_size = 1;
	this->fixed_gamma_depth = 1;
	/* Windows GDI is a real adjustment method that can be faked */
#ifdef FAKE_LIBGAMMA_METHOD_W32_GDI
	/* It is considered real but fake if it is translated to X RandR */
	this->fake = 1;
# ifdef HAVE_LIBGAMMA_METHOD_X_RANDR
	this->real = 1;
# else
	this->real = 0;
# endif
#else
	/* It is real and not fake if we are running on Windows */
	this->fake = 0;
	this->real = 1;
#endif
	/* Gamma ramp adjustments are persistent */
	this->auto_restore = 0;
}
