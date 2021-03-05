/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Configurations for the dummy adjustment method
 */
libgamma_dummy_configurations_t libgamma_dummy_internal_configurations = {
	.capabilities = {
		.crtc_information = (1 << LIBGAMMA_CRTC_INFO_COUNT) - 1,
		.default_site_known = 1,
		.multiple_sites = 1,
		.multiple_partitions = 1,
		.multiple_crtcs = 1,
		.partitions_are_graphics_cards = 1,
		.site_restore = 1,
		.partition_restore = 1,
		.crtc_restore = 1,
		.identical_gamma_sizes = 0,
		.fixed_gamma_size = 0,
		.fixed_gamma_depth = 0
	},
	.crtc_info_template = {
		.edid = NULL,
		.edid_length = 0,
		.edid_error = LIBGAMMA_EDID_NOT_FOUND,
		.width_mm = 400,
		.width_mm_error = 0,
		.height_mm = 300,
		.height_mm_error = 0,
		.red_gamma_size = 1024,
		.green_gamma_size = 2048,
		.blue_gamma_size = 512,
		.gamma_size_error = 0,
		.gamma_depth = 64,
		.gamma_depth_error = 0,
		.gamma_support = 1,
		.gamma_support_error = 0,
		.subpixel_order = LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB,
		.subpixel_order_error = 0,
		.active = 1,
		.active_error = 0,
		.connector_name = NULL,
		.connector_name_error = LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED,
		.connector_type = LIBGAMMA_CONNECTOR_TYPE_Unknown,
		.connector_type_error = 0
	},
	.real_method = LIBGAMMA_METHOD_DUMMY,
	.site_count = 2,
	.default_partition_count = 2,
	.default_crtc_count = 2,
	.inherit_sites = 1,
	.inherit_partition_count = 1,
	.inherit_crtc_count = 1,
	.stall_for_partition_count = 0,
	.stall_for_crtc_count = 0,
	.stalled_start = 1,
	.verbose = 0
};
