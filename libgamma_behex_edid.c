/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Convert a raw representation of an EDID to a lowercase hexadecimal representation
 * 
 * @param   edid    The EDID in raw representation
 * @param   length  The length of `edid`
 * @return          The EDID in lowercase hexadecimal representation,
 *                  `NULL` on allocation error, `errno` will be set accordingly
 */
extern inline char *libgamma_behex_edid(const unsigned char *restrict, size_t);
