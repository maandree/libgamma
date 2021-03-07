/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Test whether a file descriptor refers to a VT
 * 
 * @param   fd  The file descriptor
 * @return      Whether the file descriptor refers to a VT
 */
static int
is_vt_proper(int fd)
{
	char buf[32], digit0;

	/* Get TTY */
	if (ttyname_r(fd, buf, sizeof(buf) / sizeof(char)))
		return 0;

	/* Validate TTY path */
	if (!strcmp(buf, "/dev/console"))
		return 1;
	if (strncmp(buf, "/dev/tty", sizeof("/dev/tty") - 1))
		return 0;

	/* Validate TTY name */
	digit0 = buf[sizeof("/dev/tty") - 1];
	return '1' <= digit0 && digit0 <= '9';
}


/**
 * Test the availability of an adjustment method
 * 
 * @param  method     The adjustment method
 * @param  operation  Allowed values:
 *                      0: Pass if the environment suggests it will work but is not fake
 *                      1: Pass if the environment suggests it will work
 *                      2: Pass if real and not fake
 *                      3: Pass if real
 *                      4: Always pass
 *                    Other values invoke undefined behaviour
 * @return            Whether the test passed
 */
static int
list_method_test(int method, int operation)
{
	struct libgamma_method_capabilities caps;
	int fd, r, saved_errno;

	libgamma_method_capabilities(&caps, sizeof(caps), method);

	switch (operation) {
	case 0:
		/* Methods that the environment suggests will work, excluding fake */
		if (caps.fake)
			return 0;
		/* fall through */

	case 1:
		/* Methods that the environment suggests will work, including fake */
		if (!caps.real)
			return 0;
#ifdef HAVE_LIBGAMMA_METHOD_LINUX_DRM
		if (method == LIBGAMMA_METHOD_LINUX_DRM) {
			saved_errno = errno;
			if (is_vt_proper(STDIN_FILENO)) {
				r = 1;
			} else if (is_vt_proper(STDOUT_FILENO)) {
				r = 1;
			} else if (is_vt_proper(STDERR_FILENO)) {
				r = 1;
			} else {
				r = 0;
				fd = open("/dev/tty", O_RDONLY);
				if (fd >= 0) {
					r = is_vt_proper(fd);
					close(fd);
				}
			}
			errno = saved_errno;
			return r;
		}
#endif
#ifdef HAVE_LIBGAMMA_METHOD_DUMMY
		if (method == LIBGAMMA_METHOD_DUMMY)
			return 0;
#endif
		return caps.default_site_known;

	case 2:
		/* All real non-fake methods */
		return caps.real && !caps.fake;

	case 3:
		/* All real methods */
		return caps.real;

	default:
		/* All methods */
		return 1;
	}
}


/**
 * List available adjustment methods by their order of preference based on the environment
 * 
 * @param  methods    Output array of methods, should be able to hold `LIBGAMMA_METHOD_COUNT` elements
 * @param  buf_size   The number of elements that fits in `methods`, it should be `LIBGAMMA_METHOD_COUNT`,
 *                    This is used to avoid writing outside the output buffer if this library adds new
 *                    adjustment methods without the users of the library recompiling
 * @param  operation  Allowed values:
 *                      0: Methods that the environment suggests will work, excluding fake
 *                      1: Methods that the environment suggests will work, including fake
 *                      2: All real non-fake methods
 *                      3: All real methods
 *                      4: All methods
 *                    Other values invoke undefined behaviour
 * @return            The number of element that have been stored in `methods`, or should
 *                    have been stored if the buffer was large enough
 */
size_t
libgamma_list_methods(int *restrict methods, size_t buf_size, int operation)
{
	size_t n = 0;

#define X(CONST, ...)\
	if (list_method_test(CONST, operation) && n++ < buf_size)\
		methods[n - 1] = CONST;
	LIST_AVAILABLE_METHODS(X)
#undef X

	return n;
}
