/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_LINUX_DRM
#include "common.h"


/**
 * Figure out why `open` failed for a graphics card
 * 
 * @param   pathname  The pathname of the error card
 * @return            The error code to report
 */
static int
figure_out_card_open_error(const char *pathname)
{
	char buf[1024]; /* My output of `sysconf(_SC_GETGR_R_SIZE_MAX)`. */
	gid_t supplemental_groups[NGROUPS_MAX];
	struct group *group, _grp;
	struct stat attr;
	int i, n;


	/* Check which the device exists */
	if (errno == ENXIO || errno == ENODEV)
		return LIBGAMMA_NO_SUCH_PARTITION;


	/* If we did not get access permission, figure out why */

	if (errno != EACCES) {
		/* If we could not figure out what went
		 * wrong, just return the error we got */
		return LIBGAMMA_ERRNO_SET;
	}

#define __test(R, W) ((attr.st_mode & ((R) | (W))) == ((R) | (W)))

	/* Get permission requirement for the file */
	if (stat(pathname, &attr))
		return errno == EACCES ? LIBGAMMA_NO_SUCH_PARTITION : LIBGAMMA_ERRNO_SET;

	/* Test owner's, group's and others' permissions */
	if ((attr.st_uid == geteuid() && __test(S_IRUSR, S_IWUSR)) ||
	    (attr.st_gid == getegid() && __test(S_IRGRP, S_IWGRP)) || __test(S_IROTH, S_IWOTH))
		return LIBGAMMA_DEVICE_ACCESS_FAILED;

	/* The group should be "video", but perhaps
	   it is "root" to restrict users */
	if (!attr.st_gid /* root group */ || __test(S_IRGRP, S_IWGRP))
		return LIBGAMMA_DEVICE_RESTRICTED;


	/* Get the user's supplemental group membership list */
	n = getgroups(NGROUPS_MAX, supplemental_groups);
	if (n < 0)
		return LIBGAMMA_ERRNO_SET;

	/* Test whether any of the supplemental
	   group should be satisfactory */
	for (i = 0; i < n; i++)
		if (supplemental_groups[i] == attr.st_gid)
			break;

	/* If one of the supplemental groups should be satisfactory,
	   then we do not know anything more than that access failed */
	if (i != n)
		return LIBGAMMA_DEVICE_ACCESS_FAILED;

	/* Otherwise, try to get the name of the group that is
	   required and report the missing group membership */ 
	errno = getgrgid_r(attr.st_gid, &_grp, buf, sizeof(buf) / sizeof(char), &group);
	if (errno == ERANGE) {
		/* The lenght of the group's name is absurdly long, degrade to thread-unsafe. */
		errno = 0;
		group = getgrgid(attr.st_gid);
	} else if (errno) {
		return LIBGAMMA_ERRNO_SET;
	}

	libgamma_group_gid_set(attr.st_gid);
	libgamma_group_name_set(group ? group->gr_name : NULL);
	return LIBGAMMA_DEVICE_REQUIRE_GROUP;
#undef __test
}


/**
 * Initialise an allocated partition state
 * 
 * @param   this       The partition state to initialise
 * @param   site       The site state for the site that the partition belongs to
 * @param   partition  The the index of the partition within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int
libgamma_linux_drm_partition_initialise(libgamma_partition_state_t *restrict this,
                                        libgamma_site_state_t *restrict site, size_t partition)
{
	int rc = 0;
	libgamma_drm_card_data_t *restrict data;
	char pathname[PATH_MAX];

	(void) site;

	/* Check for partition index overflow */
	if (partition > INT_MAX)
		return LIBGAMMA_NO_SUCH_PARTITION;

	/* Allocate and initialise graphics card data */
	this->data = NULL;
	data = malloc(sizeof(libgamma_drm_card_data_t));
	if (!data)
		return LIBGAMMA_ERRNO_SET;
	data->fd = -1;
	data->res = NULL;
	data->encoders = NULL;
	data->connectors = NULL;
  
	/* Get the pathname for the graphics card */
	snprintf(pathname, sizeof(pathname) / sizeof(char), DRM_DEV_NAME, DRM_DIR_NAME, (int)partition);

	/* Acquire access to the graphics card */
	data->fd = open(pathname, O_RDWR | O_CLOEXEC);
	if (data->fd < 0) {
		rc = figure_out_card_open_error(pathname);
		goto fail_data;
	}

	/* Acquire mode resources */
	data->res = drmModeGetResources(data->fd);
	if (!data->res) {
		rc = LIBGAMMA_ACQUIRING_MODE_RESOURCES_FAILED;
		goto fail_fd;
	}

	/* Get the number of CRTC:s that are available in the partition */
	if (data->res->count_crtcs < 0) {
		rc = LIBGAMMA_NEGATIVE_CRTC_COUNT;
		goto fail_res;
	}
	this->crtcs_available = (size_t)data->res->count_crtcs;
	this->data = data;
	return 0;

fail_res:
	drmModeFreeResources(data->res);
fail_fd:
	close(data->fd);
fail_data:
	free(data);
	return rc;
}
