/* See LICENSE file for copyright and license details. */

/*
 * This file is intended to be included from
 * libgamma_crtc_set_gamma_ramps{8,16,32,64,f,d}_f
 */


libgamma_crtc_information_t info;
struct APPEND_RAMPS(libgamma_gamma_) ramps;
size_t i, n;
int e;

/* Get the size of the gamma ramps */
if (libgamma_get_crtc_information(&info, this, LIBGAMMA_CRTC_INFO_GAMMA_SIZE)) {
	e = info.gamma_size_error;
	if (e < 0)
		return e;
	errno = e;
	return LIBGAMMA_ERRNO_SET;
 }

/* Copy the size of the gamma ramps and calculte the grand size */
n  = ramps.  red_size = info.  red_gamma_size;
n += ramps.green_size = info.green_gamma_size;
n += ramps. blue_size = info. blue_gamma_size;

/* Allocate gamma ramps */
ramps.  red = malloc(n * sizeof(TYPE));
ramps.green = &ramps.  red[ramps.  red_size];
ramps. blue = &ramps.green[ramps.green_size];
if (!ramps.red)
	return LIBGAMMA_ERRNO_SET;

/* Generate the gamma ramp for the red channel */
for (i = 0, n = ramps.red_size; i < n; i++)
	ramps.red[i] = red_function((float)i / (float)(n - 1));

/* Generate the gamma ramp for the green channel */
for (i = 0, n = ramps.green_size; i < n; i++)
	ramps.green[i] = green_function((float)i / (float)(n - 1));

/* Generate the gamma ramp for the blue channel */
for (i = 0, n = ramps.blue_size; i < n; i++)
	ramps.blue[i] = blue_function((float)i / (float)(n - 1));

/* Apply the gamma ramps */
e = APPEND_RAMPS(libgamma_crtc_set_gamma_)(this, &ramps);
free(ramps.red);
return e;
