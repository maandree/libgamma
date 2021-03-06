/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
void
libgamma_dummy_method_capabilities(struct libgamma_method_capabilities *restrict this)
{
	int real_method = libgamma_dummy_internal_configurations.real_method;
	*this = libgamma_dummy_internal_configurations.capabilities;
	this->real = real_method != LIBGAMMA_METHOD_DUMMY;
	this->fake = this->real;
	this->auto_restore = real_method == LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS;
}
