/* See LICENSE file for copyright and license details. */
#ifndef LIBGAMMA_TEST_USER_H
#define LIBGAMMA_TEST_USER_H


#include "methods.h"

#include <libgamma.h>

#include <stdio.h>
#include <stdlib.h>



/**
 * Let the user select adjustment method, site, partition and CRTC
 * 
 * @param   site_state  Output slot for the site
 * @param   part_state  Output slot for the partition
 * @param   crtc_state  Output slot for the CRTC
 * @return              Zero on and only on success
 */
int select_monitor(libgamma_site_state_t *restrict site_state,
                   libgamma_partition_state_t *restrict part_state,
                   libgamma_crtc_state_t *restrict crtc_state);


#endif
