/* See LICENSE file for copyright and license details. */
#include "common.h"


static _Thread_local char group_name[64]; /* Group names can only be up to 16 bytes logn, 63 is a very safe number */


/**
 * Get the group that the user needs to be a member of
 * if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 * 
 * @return  The group that the user needs to be a member of if
 *          `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned, `NULL`
 *          if the name of the group `libgamma_group_gid` cannot
 *          be determined
 */
const char *
libgamma_group_name_get(void)
{
	return *group_name ? group_name : NULL;
}


/**
 * Set the group that the user needs to be a member of
 * if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 * 
 * @param  value  The group that the user needs to be a member of if
 *                `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned, may be `NULL`
 */
void
libgamma_group_name_set(const char *value)
{
	size_t n;
	if (!value) {
		*group_name = 0;
	} else {
		n = strlen(value);
		if (n >= sizeof(group_name))
			*group_name = 0;
		else
			memcpy(group_name, value, n + 1);
	}
}
