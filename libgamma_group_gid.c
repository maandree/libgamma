/* See LICENSE file for copyright and license details. */
#include "common.h"

#ifdef __WIN32__
# define gid_t short
#endif


static _Thread_local gid_t libgamma_group_gid;


/**
 * Get the group that the user needs to be a member
 * of if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 * 
 * @return  The group that the user needs to be a member of
 *          if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 */
gid_t
libgamma_group_gid_get(void)
{
	return libgamma_group_gid;
}


/**
 * Set the group that the user needs to be a member
 * of if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 * 
 * @param  value  The group that the user needs to be a member of
 *                if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 */
void
libgamma_group_gid_set(gid_t value)
{
	libgamma_group_gid = value;
}
