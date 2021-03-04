/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * The number of `LIBGAMMA_CRTC_INFO_*` values defined in
 * the version of the library the program is linked against
 * 
 * This exclude the combining macros defined below the
 * `LIBGAMMA_CRTC_INFO_COUNT` macro
 */
const int libgamma_crtc_info_count = LIBGAMMA_CRTC_INFO_COUNT;
