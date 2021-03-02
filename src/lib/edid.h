/* See LICENSE file for copyright and license details. */
#ifndef LIBGAMMA_EDID_H
#define LIBGAMMA_EDID_H

#include "libgamma-method.h"

#include <stdint.h>


/**
 * Parse the EDID of a monitor
 * 
 * @param   this    Instance of a data structure to fill with the information about the EDID;
 *                  it must contain the EDID and its length
 * @param   fields  OR:ed identifiers for the information about the EDID that should be parsed;
 *                  fields that do not have to do with EDID are ignored
 * @return          Non-zero on error
 */
int libgamma_parse_edid(libgamma_crtc_information_t *restrict, int32_t);


#endif
