/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Prints an error to stderr in a `perror` fashion
 * 
 * @param  name   The text to add at the beginning
 * @param  value  The error code, may be an `errno` value
 */
void
libgamma_perror(const char *name, int error_code)
{
	const char *desc, *gname;
	char buf[1024];
	long int gid;

	desc = libgamma_strerror_r(error_code, buf, sizeof(buf));

	if (error_code == LIBGAMMA_DEVICE_REQUIRE_GROUP) {
		gid = (intmax_t)libgamma_group_gid_get();
		if (!gid)
			goto fallback;
		gname = libgamma_group_name_get();
		if (!gname) {
			if (name && *name)
				fprintf(stderr, "%s: %s in group %ji\n", name, desc, gid);
			else
				fprintf(stderr, "%s in group %ji\n", desc, gid);
		} else {
			if (name && *name)
				fprintf(stderr, "%s: %s in the %s group (%ji)\n", name, desc, gname, gid);
			else
				fprintf(stderr, "%s in the %s group (%ji)\n", desc, gname, gid);
		}
	} else {
	fallback:
		if (name && *name)
			fprintf(stderr, "%s: %s\n", name, desc);
		else
			fprintf(stderr, "%s\n", desc);
	}
}
