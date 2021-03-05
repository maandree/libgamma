/* See LICENSE file for copyright and license details. */
#include "libgamma.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#if LIBGAMMA_CRTC_INFO_COUNT != 13
# warning CRTC information fields have been updated
#endif



/**
 * X macro of all integer gamma ramps
 * 
 * @param  _:macro(identifer)  Macro to expand
 */
#define LIST_INTEGER_RAMPS(_) _(ramps8) _(ramps32) _(ramps64) _(ramps16)

/**
 * X macro of all floating-point gamma ramps
 * 
 * @param  _:macro(identifer)  Macro to expand
 */
#define LIST_FLOAT_RAMPS(_) _(rampsf) _(rampsd)

/**
 * X macro of all gamma ramps
 * 
 * @param  _:macro(identifer)  Macro to expand
 */
#define LIST_RAMPS(_) LIST_FLOAT_RAMPS(_) LIST_INTEGER_RAMPS(_)

/* ramps16 is last because we want to make sure that the gamma ramps are
   preserved exactly on exit, and we assume X RandR is used. */



/**
 * Test mapping function from [0, 1] float encoding value to [0, 2⁸ − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2⁸ − 1] integer output value
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__const__)))
static uint8_t
dim_ramps8(float encoding)
{
	double i_encoding = (double)(encoding / 2.f);
	double f_output = (double)UINT8_MAX * i_encoding;
	uint8_t output = (uint8_t)f_output;
	if (i_encoding < (double)0.25f && output > UINT8_MAX / 2)
		output = 0;
	if (i_encoding > (double)0.75f && output < UINT8_MAX / 2)
		output = UINT8_MAX;
	return output;
}

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2¹⁶ − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2¹⁶ − 1] integer output value
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__const__)))
static uint16_t
dim_ramps16(float encoding)
{
	double i_encoding = (double)(encoding / 2.f);
	double f_output = (double)UINT16_MAX * i_encoding;
	uint16_t output = (uint16_t)f_output;
	if (i_encoding < (double)0.25f && output > UINT16_MAX / 2)
		output = 0;
	if (i_encoding > (double)0.75f && output < UINT16_MAX / 2)
		output = UINT16_MAX;
	return output;
}

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2³² − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2³² − 1] integer output value
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__const__)))
static uint32_t
dim_ramps32(float encoding)
{
	double i_encoding = (double)(encoding / 2.f);
	double f_output = (double)UINT32_MAX * i_encoding;
	uint32_t output = (uint32_t)f_output;
	if (i_encoding < (double)0.25f && output > UINT32_MAX / 2)
		output = 0;
	if (i_encoding > (double)0.75f && output < UINT32_MAX / 2)
		output = UINT32_MAX;
	return output;
}

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2⁶⁴ − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2⁶⁴ − 1] integer output value
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__const__)))
static uint64_t
dim_ramps64(float encoding)
{
	double i_encoding = (double)(encoding / 2.f);
	double f_output = (double)UINT64_MAX * i_encoding;
	uint64_t output = (uint64_t)f_output;
	if (i_encoding < (double)0.25f && output > UINT64_MAX / 2)
		output = 0;
	if (i_encoding > (double)0.75f && output < UINT64_MAX / 2)
		output = UINT64_MAX;
	return output;
}

/**
 * Test mapping function from [0, 1] float encoding value to [0, 1] float output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 1] float output value
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__const__)))
static float
dim_rampsf(float encoding)
{
	return encoding / 2.f;
}

/**
 * Test mapping function from [0, 1] double precision float encoding
 * value to [0, 1] double precision float output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 1] float output value
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__const__)))
static double
dim_rampsd(double encoding)
{
	return encoding / (double)2.f;
}


/**
 * Get the name representation of an
 * adjustment method by its identifier
 * 
 * @param   method  The identifier of the adjustment method
 * @return          The name of the adjustment
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__const__)))
static const char *
method_name(int method)
{
	const char *r = libgamma_name_of_method(method);
	return r ? r : "(unknown)";
}


/**
 * Print a list of adjustment methods
 * 
 * @param  description  Precursory text for the list
 * @param  methods      An array allocated to fit all adjustment methods
 * @param  operation    See the `operation` parameter for `libgamma_list_methods`
 */
static void
list_methods(const char *description, int *methods, int operation)
{
	/* Get adjustment method list */
	size_t i, n = libgamma_list_methods(methods, LIBGAMMA_METHOD_COUNT, operation);

	/* Print adjustment method list */
	printf("%s:\n", description);
	for (i = 0; i < n; i++)
		printf("  %s\n", method_name(methods[i]));
	printf("\n");
}


/**
 * Print all lists, of adjustments methods, that
 * are made available by `libgamma_list_methods`
 */
static void
list_methods_lists(void)
{
	/* Allocate a list for adjustment methods that is large
	 * enough if the program is up to date with the library */
	int methods[LIBGAMMA_METHOD_COUNT];
	/* Get a list of all adjustment methods */
	size_t n = libgamma_list_methods(methods, LIBGAMMA_METHOD_COUNT, 4);

	/* Print adjustment method lists. */
	list_methods("Available adjustment methods",               methods, 4);
	list_methods("Available real adjustment methods",          methods, 3);
	list_methods("Available real non-fake adjustment methods", methods, 2);
	list_methods("Recommended adjustment methods",             methods, 1);
	list_methods("Recommended non-fake adjustment methods",    methods, 0);
}


/**
 * Test the availability (determined at
 * compile-time) of all adjustment
 * methods and one that does not exist
 */
static void
method_availability(void)
{
	int method;
	/* The availability of an adjustmen method whose identifier
	 * is invalid. It should say it is not available. */
	printf("Testing the availability of a non-existing adjustment method: ");
	printf("%s\n", libgamma_is_method_available(-1) ? "available" : "not available");

	/* Test the availability of the adjustment methods that does exist */
	for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++) {
		printf("Testing the availability of %s: ", method_name(method));
		printf("%s\n", libgamma_is_method_available(method) ? "available" : "not available");
	}
	printf("\n");
}


/**
 * List the default site and the environment
 * variable, if any, that determines the
 * default site, for all availiable adjustment
 * methods
 */
static void
list_default_sites(void)
{
	int method;
	for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++) {
		/* Include only available adjustment methods.
		 * If an adjustment method is not available
		 * it should print "(null)", but as that can
		 * be misguiding we exclude those methods. */
		if (libgamma_is_method_available(method)) {
			printf("Default site for %s:\n", method_name(method));
			printf("  Variable: %s\n", libgamma_method_default_site_variable(method));
			printf("  Site name: %s\n", libgamma_method_default_site(method));
			printf("\n");
		}
	}
}


/**
 * Print the capabilities of all availiable
 * adjustment methods.
 */
static void
method_capabilities(void)
{
	libgamma_method_capabilities_t caps;
	int method;
	for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++) {
		if (libgamma_is_method_available(method)) {
			/* Print adjustment method name and get the
			 * adjustment method's capabilities. */
			printf("Capabilities of %s:\n", method_name(method));
			libgamma_method_capabilities(&caps, method);

			/* Print capabilities. The CRTC information
			 * capabilities is printed hexadecimal. See
			 * the `LIBGAMMA_CRTC_INFO_*` definitions in
			 * `libgamma.h` for what each bit represents. */
			printf("  %s: %X\n", "CRTC information",      caps.crtc_information);
			printf("  %s: %s\n", "Default site known",    caps.default_site_known            ? "yes" : "no");
			printf("  %s: %s\n", "Multiple sites",        caps.multiple_sites                ? "yes" : "no");
			printf("  %s: %s\n", "Multiple partitions",   caps.multiple_partitions           ? "yes" : "no");
			printf("  %s: %s\n", "Multiple crtcs",        caps.multiple_crtcs                ? "yes" : "no");
			printf("  %s: %s\n", "Graphics cards",        caps.partitions_are_graphics_cards ? "yes" : "no");
			printf("  %s: %s\n", "Site restore",          caps.site_restore                  ? "yes" : "no");
			printf("  %s: %s\n", "Partition restore",     caps.partition_restore             ? "yes" : "no");
			printf("  %s: %s\n", "CRTC restore",          caps.crtc_restore                  ? "yes" : "no");
			printf("  %s: %s\n", "Identical gamma sizes", caps.identical_gamma_sizes         ? "yes" : "no");
			printf("  %s: %s\n", "Fixed gamma size",      caps.fixed_gamma_size              ? "yes" : "no");
			printf("  %s: %s\n", "Fixed gamma depth",     caps.fixed_gamma_depth             ? "yes" : "no");
			printf("  %s: %s\n", "Real method",           caps.real                          ? "yes" : "no");
			printf("  %s: %s\n", "Fake method",           caps.fake                          ? "yes" : "no");
			printf("  %s: %s\n", "Auto restore",          caps.auto_restore                  ? "yes" : "no");
			printf("\n");
		}
	}
}



/**
 * Let the user select adjustment method, site, partition and CRTC
 * 
 * @param   site_state  Output slot for the site
 * @param   part_state  Output slot for the partition
 * @param   crtc_state  Output slot for the CRTC
 * @return              Zero on and only on success
 */
static int
select_monitor(libgamma_site_state_t *site_state, libgamma_partition_state_t *part_state, libgamma_crtc_state_t *crtc_state)
{
	int method;
	char *site;
	char *tmp;
	char buf[256];
	int r;


	/* -- Adjustment method -- */

	/* Let the user select adjustment method */
	printf("Select adjustment method:\n");
	for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++)
		printf("    %i:  %s\n", method, method_name(method));
	printf("> ");
	fflush(stdout);
	fgets(buf, sizeof(buf) / sizeof(char), stdin);
	method = atoi(buf);


	/* -- Site -- */

	/* Let the user select site */
	printf("Select site: ");
	fflush(stdout);
	fgets(buf, sizeof(buf) / sizeof(char), stdin);
	tmp = strchr(buf, '\n');
	if (tmp)
		*tmp = '\0';
	if (!buf[0]) {
		site = NULL;
	} else {
		site = malloc((strlen(buf) + 1) * sizeof(char));
		memcpy(site, buf, strlen(buf) + 1);
	}

	/* Initialise site state */
	if ((r = libgamma_site_initialise(site_state, method, site))) {
		free(site);
		libgamma_perror("error", r);
		return 1;
	}


	/* -- Partition -- */

	/* Check that the site has at least one partition */
	if (!site_state->partitions_available) {
		libgamma_site_free(site_state);
		printf("No partitions found\n");
		return 1;
	}

	/* Let the user select partition */
	printf("Select partition [0, %lu]: ", site_state->partitions_available - 1);
	fflush(stdout);
	fgets(buf, sizeof(buf) / sizeof(char), stdin);

	/* Initialise partition state */
	if ((r = libgamma_partition_initialise(part_state, site_state, (size_t)atoll(buf)))) {
		libgamma_site_free(site_state);
		libgamma_perror("error", r);
		return 1;
	}


	/* -- CRTC -- */

	/* Check that the partition has at least one CRTC */
	if (!part_state->crtcs_available) {
		libgamma_partition_free(part_state);
		libgamma_site_free(site_state);
		printf("No CRTC:s found\n");
		return 1;
	}

	/* Let the user select CRTC */
	printf("Select CRTC [0, %lu]: ", part_state->crtcs_available - 1);
	fflush(stdout);
	fgets(buf, sizeof(buf) / sizeof(char), stdin);

	/* Initialise CRTC state. */
	if ((r = libgamma_crtc_initialise(crtc_state, part_state, (size_t)atoll(buf)))) {
		libgamma_partition_free(part_state);
		libgamma_site_free(site_state);
		libgamma_perror("error", r);
		return 1;
	}

	printf("\n");
	return 0;
}



/**
 * Conditionally print a CRTC information field
 * 
 * @param  type:data-type           Data type for the value of the information field
 * @param  notation:string-literal  %-pattern for `printf` (excluding the %) that is used for `type`
 * @param  do_print                 Whether the information should be included in the process's output
 * @param  description              A description of the information field
 * @param  error                    The error of the information field
 * @param  value                    The value of the information field
 */
#define print_crtc_information_(type, notation)\
	static void\
	print_crtc_information_##type(int do_print, const char *description, int error, type value)\
	{\
		char buf[256];\
		if (do_print) {\
			if (error) {\
				snprintf(buf, sizeof(buf) / sizeof(char), "  (error) %s", description);\
				libgamma_perror(buf, error);\
			} else {\
				printf("  %s: %" notation "\n", description, value);\
			}\
		}\
	}


/**
 * A single [a-z] word alternative to `const char *`, we need it for the
 * function name of the string variant of `print_crtc_information_*`
 */
typedef const char *str;
/* Create `print_crtc_information_*` variants */
print_crtc_information_(size_t, "lu")
print_crtc_information_(signed, "i")
print_crtc_information_(int, "i")
#ifdef __GCC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdouble-promotion"
#endif
print_crtc_information_(float, "f")
#ifdef __GCC__
# pragma GCC diagnostic pop
#endif
print_crtc_information_(str, "s")
#undef print_crtc_information_


/**
 * Get a string representation of a subpixel order
 * 
 * @param   value  The subpixel order
 * @return         String representation
 */
static const char *
subpixel_order_str(libgamma_subpixel_order_t value)
{
	const char *r = libgamma_name_of_subpixel_order(value);
	return r ? r : "(unknown)";
}


/**
 * Get a string representation of a connector type
 * 
 * @param   value  The connector type
 * @return         String representation
 */
static const char *
connector_type_str(libgamma_connector_type_t value)
{
	const char *r = libgamma_name_of_connector_type(value);
	return r ? r : "(unknown)";
}


/**
 * Get a string representation of a decision
 * 
 * @param   value  The decision
 * @return         String representation
 */
static const char *
decision_str(libgamma_decision_t value)
{
	switch (value) {
	case LIBGAMMA_NO:    return "LIBGAMMA_NO"; 
	case LIBGAMMA_MAYBE: return "LIBGAMMA_MAYBE"; 
	case LIBGAMMA_YES:   return "LIBGAMMA_YES"; 
	default:
		return "(unknown)";
	}
}


/**
 * The CRTC information for a CRTC
 * 
 * @param  crtc  The CRTC
 */
static void
crtc_information(libgamma_crtc_state_t *restrict crtc)
{
	libgamma_method_capabilities_t caps;
	libgamma_crtc_information_t info;
	int fields, field;
	char *edid_lc, *edid_uc;
	unsigned char *edid_raw;

	/* Get supported CRTC informations fields */
	libgamma_method_capabilities(&caps, crtc->partition->site->method);

	/* List unsupport information fields by testing them one by one */
	for (fields = caps.crtc_information; field = fields & -fields, fields; fields ^= field) {
		if (libgamma_get_crtc_information(&info, crtc, field))
			printf("Could not read CRTC information field %i\n", field);
		free(info.edid);
		free(info.connector_name);
	}

	/* Get CRTC information, that is supported */
	fields = caps.crtc_information;
	if (libgamma_get_crtc_information(&info, crtc, fields))
		printf("An error occurred while reading CRTC information\n");

	/* Macros for printing CRTC information */
#define print2(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR, ERROR_VAR)\
	print_crtc_information_##TYPE(fields & FIELD_ID, DESCRIPTION, info.ERROR_VAR, info.FIELD_VAR);
#define print(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR)\
	print2(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR, FIELD_VAR##_error);
  
  
	/* Print CRTC information */

	printf("CRTC information:\n");

	/* Print the EDID field */
	if ((fields & LIBGAMMA_CRTC_INFO_EDID)) {
		if (info.edid_error) {
			libgamma_perror("  (error) EDID", info.edid_error);
		} else {
			edid_lc  = libgamma_behex_edid(info.edid, info.edid_length);
			edid_raw = libgamma_unhex_edid(edid_lc);
			edid_uc  = libgamma_behex_edid_uppercase(edid_raw, info.edid_length);
			printf("  EDID: %s\n", edid_lc);
			printf("  EDID (uppercase): %s\n", edid_uc);
			printf("  EDID (length): %lu\n", info.edid_length);
			free(edid_lc);
			free(edid_raw);
			free(edid_uc);
		}
	}
	/* Print physical dimensions of the monitor */
	print(size_t, LIBGAMMA_CRTC_INFO_WIDTH_MM, "width", width_mm);
	print(size_t, LIBGAMMA_CRTC_INFO_HEIGHT_MM, "height", height_mm);
	print(size_t, LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID, "width per EDID", width_mm_edid);
	print(size_t, LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID, "height per EDID", height_mm_edid);
	/* Print gamma ramp information */
	print2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "red gamma ramp size", red_gamma_size, gamma_size_error);
	print2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "green gamma ramp size", green_gamma_size, gamma_size_error);
	print2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "blue gamma ramp size", blue_gamma_size, gamma_size_error);
	print(signed, LIBGAMMA_CRTC_INFO_GAMMA_DEPTH, "gamma ramp depth", gamma_depth);
	/* Print gamma ramp support */
	if (fields & LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT) {
		if (info.gamma_support_error)
			libgamma_perror("  (error) gamma support", info.gamma_support_error);
		else
			printf("  gamma support: %s\n", decision_str(info.gamma_support));
	}
	/* Print subpixel order for the monitor */
	if (fields & LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER) {
		if (info.subpixel_order_error)
			libgamma_perror("  (error) subpixel order", info.subpixel_order_error);
		else
			printf("  subpixel order: %s\n", subpixel_order_str(info.subpixel_order));
	}
	/* Print connector information */
	print(int, LIBGAMMA_CRTC_INFO_ACTIVE, "active", active);
	print(str, LIBGAMMA_CRTC_INFO_CONNECTOR_NAME, "connector name", connector_name);
	if (fields & LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE) {
		if (info.connector_type_error)
			libgamma_perror("  (error) subpixel order", info.connector_type_error);
		else
			printf("  subpixel order: %s\n", connector_type_str(info.connector_type));
	}
	/* Print the gamma characteristics of the monitor */
	print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "red gamma characteristics", gamma_red, gamma_error);
	print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "green gamma characteristics", gamma_green, gamma_error);
	print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "blue gamma characteristics", gamma_blue, gamma_error);
	printf("\n");


#undef print
#undef print2

	/* Release resouces */
	free(info.edid);
	free(info.connector_name);
}


/**
 * The error API
 */
void
error_test(void)
{
	int i;
  
	/* Test that naming and name dereferencing
	   of errors work. Because the mappings in
	   these [`libgamma_value_of_error` and
	   `libgamma_name_of_error`] functions are
	   generated, it should work if and only if
	   one test passes, assumming the errors are
	   unique whihc is tested in the end of this
	   function. */
	printf("Testing error API using LIBGAMMA_STATE_UNKNOWN:\n");
	printf("  Expecting %i: %i\n", LIBGAMMA_STATE_UNKNOWN, libgamma_value_of_error("LIBGAMMA_STATE_UNKNOWN"));
	printf("  Expecting %s: %s\n", "LIBGAMMA_STATE_UNKNOWN", libgamma_name_of_error(LIBGAMMA_STATE_UNKNOWN));
	printf("\n");
  
	/* Test that `libgamma_perror` can print
	   libgamma errors and system errors, and
	   handle success in the same with as `perror`. */
	printf("Testing libgamma_perror:\n");
	libgamma_perror("  Expecting LIBGAMMA_STATE_UNKNOWN", LIBGAMMA_STATE_UNKNOWN);
	libgamma_perror("  Expecting a description for ENOMEM", ENOMEM);
	libgamma_perror("  Expecting a description for successfulness", 0);
	/* Test that `libgamma_perror` handles
	   `LIBGAMMA_ERRNO_SET` correctly. */
	libgamma_perror("  Expecting a description for ENOMEM", (errno = ENOMEM, LIBGAMMA_ERRNO_SET));
	/* That that `libgamma_perror` handles
	   `LIBGAMMA_DEVICE_REQUIRE_GROUP`
	   correctly both when the required
	   group's name is known and when it
	   is unknown. */
	libgamma_group_gid_set(10);
	libgamma_group_name_set("test");
	libgamma_perror("  Expecting 'LIBGAMMA_DEVICE_REQUIRE_GROUP: test (10)'", LIBGAMMA_DEVICE_REQUIRE_GROUP);
	libgamma_group_name_set(NULL);
	libgamma_perror("  Expecting 'LIBGAMMA_DEVICE_REQUIRE_GROUP: 10'", LIBGAMMA_DEVICE_REQUIRE_GROUP);
	printf("\n");
  
	/* That all libgamma error codes
	   are unique. This is done by
	   getting the name associated
	   with an error code and the getting
	   the error code associated that
	   name and test that the same
	   error code is returned as put in,
	   for each libgamma error code. */
	printf("Testing error code uniqueness: ");
	for (i = -1; i >= LIBGAMMA_ERROR_MIN; i--) {
		if (libgamma_value_of_error(libgamma_name_of_error(i)) != i) {
			printf("failed\n");
			goto not_unique;
		}
	}
	printf("passed\n");
not_unique:
	printf("\n");
}



/**
 * Test libgamma
 * 
 * @return  Non-zero on machine detectable error, this library
 *          may still be faulty if zero is returned
 */
int
main(void)
{
	libgamma_site_state_t *site_state = malloc(sizeof(libgamma_site_state_t));
	libgamma_partition_state_t *part_state = malloc(sizeof(libgamma_partition_state_t));
	libgamma_crtc_state_t *crtc_state = malloc(sizeof(libgamma_crtc_state_t));
	libgamma_crtc_information_t info;
#define X(RAMPS)\
	libgamma_gamma_##RAMPS##_t old_##RAMPS, RAMPS;\
	libgamma_gamma_##RAMPS##_fun* f_##RAMPS = dim_##RAMPS;
	LIST_RAMPS(X)
#undef X
	size_t i, n;
	int r, rr = 0;

	/* Test miscellaneous parts of the library */
	list_methods_lists();
	method_availability();
	list_default_sites();
	method_capabilities();
	error_test();

	/* Select monitor for tests over CRTC:s, partitions and sites */
	if (select_monitor(site_state, part_state, crtc_state))
		return 1;

	/* Test CRTC information functions */
	crtc_information(crtc_state);

	/* Get the sizes of the gamma ramps for the selected CRTC */
	libgamma_get_crtc_information(&info, crtc_state, LIBGAMMA_CRTC_INFO_GAMMA_SIZE);

	/* Create gamma ramps for each depth */
#define X(R)\
	old_##R.red_size = info.red_gamma_size;\
	old_##R.green_size = info.green_gamma_size;\
	old_##R.blue_size = info.blue_gamma_size;\
	R = old_##R;\
	libgamma_gamma_##R##_initialise(&old_##R);\
	libgamma_gamma_##R##_initialise(&R);
	LIST_RAMPS(X)
#undef X

	/* Fill gamma ramps, for each depth, with the CRTC:s current ramps */
#define X(R)\
	if ((rr |= r = libgamma_crtc_get_gamma_##R(crtc_state, &old_##R))) {\
		libgamma_perror("libgamma_crtc_get_gamma_" #R, r);\
		goto done;\
	}\
	if ((rr |= r = libgamma_crtc_get_gamma_##R(crtc_state, &R))) {\
		libgamma_perror("libgamma_crtc_get_gamma_" #R, r);\
		goto done;\
	}
	LIST_RAMPS(X)
#undef X
  
	/* Test function assisted gamma ramps setting */
#define X(R)\
	/* Dim the monitor for one second and the restore it. */\
	printf("Dimming monitor output for 1 second... (" #R ")\n");\
	if ((rr |= r = libgamma_crtc_set_gamma_##R##_f(crtc_state, f_##R, f_##R, f_##R)))\
		libgamma_perror("libgamma_crtc_set_gamma_" #R "_f", r);\
	sleep(1);\
	if ((rr |= r = libgamma_crtc_set_gamma_##R(crtc_state, &old_##R)))\
		libgamma_perror("libgamma_crtc_set_gamma_" #R, r);\
	printf("Done!\n");\
	/* Sleep for one second, we have more depths to test */\
	printf("Sleeping for 1 second...\n");\
	sleep(1);
	LIST_RAMPS(X)
#undef X

	/* Test getting and setting gamma ramps */
#define X(R)\
	/* Get the grand size of the gamma ramps */\
	n = R.red_size;\
	n = n > R.green_size ? n : R.green_size;\
	n = n > R.blue_size ? n : R.blue_size;\
	/* Print the current gamma ramps */\
	printf("Current gamma ramps (" #R "):\n");\
	for (i = 0; i < n; i++) {\
		if (i < R.red_size)    Y(R.red,   "1");  else  printf("      ");\
		if (i < R.green_size)  Y(R.green, "2");  else  printf("      ");\
		if (i < R.blue_size)   Y(R.blue,  "4");  else  printf("      ");\
		printf("\n");\
	}\
	printf("\n");\
	/* Adjust the gamma ramps for dimming the monitor */\
	for (i = 0; i < R.red_size + R.green_size + R.blue_size; i++)\
		R.red[i] /= 2;\
	/* Dim the monitor for one second and the restore it */\
	printf("Dimming monitor for 1 second...\n");\
	if ((rr |= r = libgamma_crtc_set_gamma_##R(crtc_state, &R)))\
		libgamma_perror("libgamma_crtc_set_gamma_" #R, r);\
	sleep(1);\
	if ((rr |= r = libgamma_crtc_set_gamma_##R(crtc_state, &old_##R)))\
		libgamma_perror("libgamma_crtc_set_gamma_" #R, r);\
	printf("Done!\n");\
	/* Sleep for one second, we have more depths to test */\
	printf("Sleeping for 1 second...\n");\
	sleep(1);
#define Y(R, C)  printf("  \033[3" C "m%1.8lf\033[00m", (double)(R[i]))
	LIST_FLOAT_RAMPS(X)
#undef Y
#define Y(R, C)  printf("  \033[3" C "m%16llX\033[00m", (uint64_t)(R[i]))
	LIST_INTEGER_RAMPS(X)
#undef Y
#undef X

	/* Test order of gamma ramps */
	memcpy(ramps16.red, old_ramps16.red, ramps16.red_size * sizeof(uint16_t));
	memset(ramps16.green, 0, ramps16.green_size * sizeof(uint16_t));
	memset(ramps16.blue, 0, ramps16.blue_size * sizeof(uint16_t));
	printf("Making the monitor red-only for 1 second...\n");
	if ((rr |= r = libgamma_crtc_set_gamma_ramps16(crtc_state, &ramps16)))
		libgamma_perror("libgamma_crtc_set_gamma_ramps16", r);
	sleep(1);
	memset(ramps16.red, 0, ramps16.red_size * sizeof(uint16_t));
	memcpy(ramps16.green, old_ramps16.green, ramps16.green_size * sizeof(uint16_t));
	printf("Making the monitor green-only for 1 second...\n");
	if ((rr |= r = libgamma_crtc_set_gamma_ramps16(crtc_state, &ramps16)))
		libgamma_perror("libgamma_crtc_set_gamma_ramps16", r);
	sleep(1);
	memset(ramps16.green, 0, ramps16.green_size * sizeof(uint16_t));
	memcpy(ramps16.blue, old_ramps16.blue, ramps16.blue_size * sizeof(uint16_t));
	printf("Making the monitor green-only for 1 second...\n");
	if ((rr |= r = libgamma_crtc_set_gamma_ramps16(crtc_state, &ramps16)))
		libgamma_perror("libgamma_crtc_set_gamma_ramps16", r);
	sleep(1);
	if ((rr |= r = libgamma_crtc_set_gamma_ramps64(crtc_state, &old_ramps64)))
		libgamma_perror("libgamma_crtc_set_gamma_ramps64", r);
	printf("Done!\n");

	/* TODO Test gamma ramp restore functions */
  
done:
	/* Release resouces */
#define X(R)\
	libgamma_gamma_##R##_destroy(&R);\
	libgamma_gamma_##R##_destroy(&old_##R);
	LIST_RAMPS(X)
#undef X
	libgamma_crtc_free(crtc_state);
	libgamma_partition_free(part_state);
	libgamma_site_free(site_state);
	return rr;
}
