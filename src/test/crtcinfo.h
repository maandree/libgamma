/* See LICENSE file for copyright and license details. */
#ifndef LIBGAMMA_TEST_CRTCINFO_H
#define LIBGAMMA_TEST_CRTCINFO_H


#include <libgamma.h>

#include <stdio.h>
#include <stdlib.h>


/**
 * The CRTC information for a CRTC
 * 
 * @param  crtc  The CRTC
 */
void crtc_information(libgamma_crtc_state_t *restrict crtc);


#endif
