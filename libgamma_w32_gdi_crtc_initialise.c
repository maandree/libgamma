/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_W32_GDI
#include "common.h"



/**
 * Initialise an allocated CRTC state
 * 
 * @param   this       The CRTC state to initialise
 * @param   partition  The partition state for the partition that the CRTC belongs to
 * @param   crtc       The the index of the CRTC within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int
libgamma_w32_gdi_crtc_initialise(libgamma_crtc_state_t *restrict this,
                                 libgamma_partition_state_t *restrict partition, size_t crtc)
{
	DISPLAY_DEVICE display;
	HDC context;

	(void) partition;

	this->data = NULL;

	/* Windows's API mandates this... */
	display.cb = sizeof(DISPLAY_DEVICE);
	/* Get identifier for selected CRTC */
	if (!EnumDisplayDevices(NULL, (DWORD)crtc, &display, 0))
		return LIBGAMMA_NO_SUCH_CRTC;
	/* Check that the connector is enabled,
	 * newer versions of Windows will always pass.
	 * (According to w32's documentation, but that
	 * that is a load of crap) */
	if (!(display.StateFlags & DISPLAY_DEVICE_ACTIVE))
		return LIBGAMMA_CONNECTOR_DISABLED;
	/* Acquire CRTC connection. */
	context = CreateDC(TEXT("DISPLAY"), display.DeviceName, NULL, NULL);
	if (!context)
		return LIBGAMMA_OPEN_CRTC_FAILED;
	this->data = context;
	return 0;
}
