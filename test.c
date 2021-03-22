/* See LICENSE file for copyright and license details. */
#include "libgamma.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#ifdef __WIN32__
# define gid_t short
#endif


#if LIBGAMMA_CRTC_INFO_COUNT != 15
# error CRTC information fields have been updated
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
 * Invertion mapping function from [0, 1] float encoding value to [0, 2¹⁶ − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2¹⁶ − 1] integer output value
 */
LIBGAMMA_GCC_ONLY__(__attribute__((__const__)))
static uint16_t
inv_ramps16(float encoding)
{
	double i_encoding = (double)(1.f - encoding);
	double f_output = (double)UINT16_MAX * i_encoding;
	uint16_t output = (uint16_t)f_output;
	if (i_encoding < (double)0.25f && output > UINT16_MAX / 2)
		output = 0;
	if (i_encoding > (double)0.75f && output < UINT16_MAX / 2)
		output = UINT16_MAX;
	return output;
}


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
	printf("%s:", description);
	for (i = 0; i < n; i++)
		printf(" %s", method_name(methods[i]));
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

	/* Print adjustment method lists. */
	list_methods("Available adjustment methods",               methods, 4);
	list_methods("Available real adjustment methods",          methods, 3);
	list_methods("Available real non-fake adjustment methods", methods, 2);
	list_methods("Recommended adjustment methods",             methods, 1);
	list_methods("Recommended non-fake adjustment methods",    methods, 0);
	printf("\n");
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
	struct libgamma_method_capabilities caps;
	int method;
	for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++) {
		if (libgamma_is_method_available(method)) {
			/* Print adjustment method name and get the
			 * adjustment method's capabilities. */
			printf("Capabilities of %s:\n", method_name(method));
			libgamma_method_capabilities(&caps, sizeof(caps), method);
			if (caps.struct_version != LIBGAMMA_METHOD_CAPABILITIES_STRUCT_VERSION) {
				fprintf(stderr, "LIBGAMMA_METHOD_CAPABILITIES_STRUCT_VERSION must be updated\n");
				exit(1);
			}

			/* Print capabilities. The CRTC information
			 * capabilities is printed hexadecimal. See
			 * the `LIBGAMMA_CRTC_INFO_*` definitions in
			 * `libgamma.h` for what each bit represents. */
			printf("  %s: %llx\n", "CRTC information",      caps.crtc_information);
			printf("  %s: %s\n",   "Default site known",    caps.default_site_known            ? "yes" : "no");
			printf("  %s: %s\n",   "Multiple sites",        caps.multiple_sites                ? "yes" : "no");
			printf("  %s: %s\n",   "Multiple partitions",   caps.multiple_partitions           ? "yes" : "no");
			printf("  %s: %s\n",   "Multiple crtcs",        caps.multiple_crtcs                ? "yes" : "no");
			printf("  %s: %s\n",   "Graphics cards",        caps.partitions_are_graphics_cards ? "yes" : "no");
			printf("  %s: %s\n",   "Site restore",          caps.site_restore                  ? "yes" : "no");
			printf("  %s: %s\n",   "Partition restore",     caps.partition_restore             ? "yes" : "no");
			printf("  %s: %s\n",   "CRTC restore",          caps.crtc_restore                  ? "yes" : "no");
			printf("  %s: %s\n",   "Identical gamma sizes", caps.identical_gamma_sizes         ? "yes" : "no");
			printf("  %s: %s\n",   "Fixed gamma size",      caps.fixed_gamma_size              ? "yes" : "no");
			printf("  %s: %s\n",   "Fixed gamma depth",     caps.fixed_gamma_depth             ? "yes" : "no");
			printf("  %s: %s\n",   "Real method",           caps.real                          ? "yes" : "no");
			printf("  %s: %s\n",   "Fake method",           caps.fake                          ? "yes" : "no");
			printf("  %s: %s\n",   "Auto restore",          caps.auto_restore                  ? "yes" : "no");
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
select_monitor(struct libgamma_site_state *site_state, struct libgamma_partition_state *part_state,
               struct libgamma_crtc_state *crtc_state)
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
	fgets(buf, sizeof(buf), stdin);
	method = atoi(buf);


	/* -- Site -- */

	/* Let the user select site */
	printf("Select site: ");
	fflush(stdout);
	fgets(buf, sizeof(buf), stdin);
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
	fgets(buf, sizeof(buf), stdin);

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
	fgets(buf, sizeof(buf), stdin);

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
#define PRINT_CRTC_INFORMATION(type, notation)\
	static void\
	print_crtc_information_##type(int do_print, const char *description, int error, type value)\
	{\
		char buf[256];\
		if (do_print) {\
			if (error) {\
				snprintf(buf, sizeof(buf), "  (error) %s", description);\
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
PRINT_CRTC_INFORMATION(size_t, "lu")
PRINT_CRTC_INFORMATION(signed, "i")
PRINT_CRTC_INFORMATION(int, "i")
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdouble-promotion"
#endif
PRINT_CRTC_INFORMATION(float, "f")
#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
PRINT_CRTC_INFORMATION(str, "s")
#undef PRINT_CRTC_INFORMATION


/**
 * Get a string representation of a subpixel order
 * 
 * @param   value  The subpixel order
 * @return         String representation
 */
static const char *
subpixel_order_str(enum libgamma_subpixel_order value)
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
connector_type_str(enum libgamma_connector_type value)
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
decision_str(enum libgamma_decision value)
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
crtc_information(struct libgamma_crtc_state *restrict crtc)
{
	struct libgamma_method_capabilities caps;
	struct libgamma_crtc_information info;
	unsigned long long fields, field;
	char *edid_lc, *edid_uc;
	unsigned char *edid_raw;

	/* Get supported CRTC informations fields */
	libgamma_method_capabilities(&caps, sizeof(caps), crtc->partition->site->method);
	if (caps.struct_version != LIBGAMMA_METHOD_CAPABILITIES_STRUCT_VERSION) {
		fprintf(stderr, "LIBGAMMA_METHOD_CAPABILITIES_STRUCT_VERSION must be updated\n");
		exit(1);
	}

	/* List unsupport information fields by testing them one by one */
	for (fields = caps.crtc_information; field = fields & -fields, fields; fields ^= field) {
		if (libgamma_get_crtc_information(&info, sizeof(info), crtc, field))
			printf("Could not read CRTC information field %llu\n", field);
		free(info.edid);
		free(info.connector_name);
	}

	/* Get CRTC information, that is supported */
	fields = caps.crtc_information;
	if (libgamma_get_crtc_information(&info, sizeof(info), crtc, fields))
		printf("An error occurred while reading CRTC information\n");
	if (info.struct_version != LIBGAMMA_CRTC_INFORMATION_STRUCT_VERSION) {
		fprintf(stderr, "LIBGAMMA_CRTC_INFORMATION_STRUCT_VERSION must be updated\n");
		exit(1);
	}

	/* Macros for printing CRTC information */
#define PRINT2(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR, ERROR_VAR)\
	print_crtc_information_##TYPE(fields & FIELD_ID, DESCRIPTION, info.ERROR_VAR, info.FIELD_VAR);
#define PRINT(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR)\
	PRINT2(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR, FIELD_VAR##_error);
  
  
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
	PRINT(size_t, LIBGAMMA_CRTC_INFO_WIDTH_MM, "width", width_mm);
	PRINT(size_t, LIBGAMMA_CRTC_INFO_HEIGHT_MM, "height", height_mm);
	PRINT(size_t, LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID, "width per EDID", width_mm_edid);
	PRINT(size_t, LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID, "height per EDID", height_mm_edid);
	/* Print gamma ramp information */
	PRINT2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "red gamma ramp size", red_gamma_size, gamma_size_error);
	PRINT2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "green gamma ramp size", green_gamma_size, gamma_size_error);
	PRINT2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "blue gamma ramp size", blue_gamma_size, gamma_size_error);
	PRINT(signed, LIBGAMMA_CRTC_INFO_GAMMA_DEPTH, "gamma ramp depth", gamma_depth);
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
	PRINT(int, LIBGAMMA_CRTC_INFO_ACTIVE, "active", active);
	PRINT(str, LIBGAMMA_CRTC_INFO_CONNECTOR_NAME, "connector name", connector_name);
	if (fields & LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE) {
		if (info.connector_type_error)
			libgamma_perror("  (error) subpixel order", info.connector_type_error);
		else
			printf("  subpixel order: %s\n", connector_type_str(info.connector_type));
	}
	/* Print the gamma characteristics of the monitor */
	PRINT2(float, LIBGAMMA_CRTC_INFO_GAMMA, "red gamma characteristics", gamma_red, gamma_error);
	PRINT2(float, LIBGAMMA_CRTC_INFO_GAMMA, "green gamma characteristics", gamma_green, gamma_error);
	PRINT2(float, LIBGAMMA_CRTC_INFO_GAMMA, "blue gamma characteristics", gamma_blue, gamma_error);
	/* Print the colour space of the monitor */
	PRINT2(float, LIBGAMMA_CRTC_INFO_CHROMA, "red chroma x", red_chroma_x, chroma_error);
	PRINT2(float, LIBGAMMA_CRTC_INFO_CHROMA, "red chroma y", red_chroma_y, chroma_error);
	PRINT2(float, LIBGAMMA_CRTC_INFO_CHROMA, "green chroma x", green_chroma_x, chroma_error);
	PRINT2(float, LIBGAMMA_CRTC_INFO_CHROMA, "green chroma y", green_chroma_y, chroma_error);
	PRINT2(float, LIBGAMMA_CRTC_INFO_CHROMA, "blue chroma x", blue_chroma_x, chroma_error);
	PRINT2(float, LIBGAMMA_CRTC_INFO_CHROMA, "blue chroma y", blue_chroma_y, chroma_error);
	PRINT2(float, LIBGAMMA_CRTC_INFO_WHITE_POINT, "white point x", white_point_x, white_point_error);
	PRINT2(float, LIBGAMMA_CRTC_INFO_WHITE_POINT, "white point y", white_point_y, white_point_error);

	printf("\n");


#undef PRINT
#undef PRINT2

	/* Release resouces */
	free(info.edid);
	free(info.connector_name);
}


/**
 * Test that count macros are set to the same values as the count variables
 */
static void
test_count_consts(void)
{
	if (LIBGAMMA_ERROR_MIN != libgamma_error_min) {
		fprintf(stderr, "LIBGAMMA_ERROR_MIN != libgamma_error_min\n");
		exit(1);
	}
	if (LIBGAMMA_METHOD_COUNT != libgamma_method_count) {
		fprintf(stderr, "LIBGAMMA_METHOD_COUNT != libgamma_method_count\n");
		exit(1);
	}
	if (LIBGAMMA_CONNECTOR_TYPE_COUNT != libgamma_connector_type_count) {
		fprintf(stderr, "LIBGAMMA_CONNECTOR_TYPE_COUNT != libgamma_connector_type_count\n");
		exit(1);
	}
	if (LIBGAMMA_SUBPIXEL_ORDER_COUNT != libgamma_subpixel_order_count) {
		fprintf(stderr, "LIBGAMMA_SUBPIXEL_ORDER_COUNT != libgamma_subpixel_order_count\n");
		exit(1);
	}
}


/**
 * Test functions for connector types
 */
static void
test_connector_types(void)
{
	size_t n = 0;
	char buf[128], *w;
	const char *r;
	enum libgamma_connector_type type;

#define X(CONST)\
	do {\
		if ((unsigned int)CONST >= (unsigned int)LIBGAMMA_CONNECTOR_TYPE_COUNT) {\
			fprintf(stderr, "%s >= LIBGAMMA_CONNECTOR_TYPE_COUNT\n", #CONST);\
			exit(1);\
		}\
		if (!libgamma_const_of_connector_type(CONST)) {\
			fprintf(stderr, "libgamma_const_of_connector_type(%s) == NULL\n", #CONST);\
			exit(1);\
		}\
		if (!libgamma_name_of_connector_type(CONST)) {\
			fprintf(stderr, "libgamma_name_of_connector_type(%s) == NULL\n", #CONST);\
			exit(1);\
		}\
		if (strcmp(libgamma_const_of_connector_type(CONST), #CONST)) {\
			fprintf(stderr, "libgamma_const_of_connector_type(%s) != \"%s\"\n", #CONST, #CONST);\
			exit(1);\
		}\
		type = (enum libgamma_connector_type)-1;\
		if (libgamma_value_of_connector_type(#CONST, &type) || type != CONST) {\
			fprintf(stderr, "libgamma_value_of_connector_type(\"%s\") != %s\n", #CONST, #CONST);\
			exit(1);\
		}\
		type = (enum libgamma_connector_type)-1;\
		if (libgamma_value_of_connector_type(libgamma_name_of_connector_type(CONST), &type) || type != CONST) {\
			fprintf(stderr, "libgamma_value_of_connector_type(libgamma_name_of_connector_type(%s)) != %s\n",\
			        #CONST, #CONST); \
			exit(1);\
		}\
		type = (enum libgamma_connector_type)-1;\
		if (libgamma_value_of_connector_type(#CONST, &type) || type != CONST) { \
			fprintf(stderr, "libgamma_value_of_connector_type(\"%s\") != %s\n", #CONST, #CONST);\
			exit(1);\
		}\
		if (strlen(libgamma_name_of_connector_type(CONST)) >= sizeof(buf)) {\
			fprintf(stderr, "strlen(libgamma_name_of_connector_type(%s) >= %zu\n", #CONST, sizeof(buf));\
			exit(1);\
		}\
		r = libgamma_name_of_connector_type(CONST);\
		for (w = buf; *r; r++)\
			if (*r != ' ' && *r != '-')\
				*w++ = *r;\
		*w = '\0';\
		if (strcasecmp(buf, &#CONST[sizeof("LIBGAMMA_CONNECTOR_TYPE_") - 1])) {\
			fprintf(stderr, "libgamma_name_of_connector_type(%s) without [ -] != %s, ignoreing case\n",\
			        #CONST, &#CONST[sizeof("LIBGAMMA_CONNECTOR_TYPE_") - 1]);\
			exit(1);\
		}\
		n += 1;\
	} while (0)

	X(LIBGAMMA_CONNECTOR_TYPE_Unknown);
	X(LIBGAMMA_CONNECTOR_TYPE_VGA);
	X(LIBGAMMA_CONNECTOR_TYPE_DVI);
	X(LIBGAMMA_CONNECTOR_TYPE_DVII);
	X(LIBGAMMA_CONNECTOR_TYPE_DVID);
	X(LIBGAMMA_CONNECTOR_TYPE_DVIA);
	X(LIBGAMMA_CONNECTOR_TYPE_Composite);
	X(LIBGAMMA_CONNECTOR_TYPE_SVIDEO);
	X(LIBGAMMA_CONNECTOR_TYPE_LVDS);
	X(LIBGAMMA_CONNECTOR_TYPE_Component);
	X(LIBGAMMA_CONNECTOR_TYPE_9PinDIN);
	X(LIBGAMMA_CONNECTOR_TYPE_DisplayPort);
	X(LIBGAMMA_CONNECTOR_TYPE_HDMI);
	X(LIBGAMMA_CONNECTOR_TYPE_HDMIA);
	X(LIBGAMMA_CONNECTOR_TYPE_HDMIB);
	X(LIBGAMMA_CONNECTOR_TYPE_TV);
	X(LIBGAMMA_CONNECTOR_TYPE_eDP);
	X(LIBGAMMA_CONNECTOR_TYPE_VIRTUAL);
	X(LIBGAMMA_CONNECTOR_TYPE_DSI);
	X(LIBGAMMA_CONNECTOR_TYPE_LFP);
	X(LIBGAMMA_CONNECTOR_TYPE_DPI);
	X(LIBGAMMA_CONNECTOR_TYPE_WRITEBACK);
	X(LIBGAMMA_CONNECTOR_TYPE_SPI);

#undef X

	if (n != LIBGAMMA_CONNECTOR_TYPE_COUNT) {
		fprintf(stderr, "List of connector types in `test_connector_types` must be updated");
		exit(1);
	}

	if (libgamma_name_of_connector_type(-1)) {
		fprintf(stderr, "libgamma_name_of_connector_type(<invalid>) != NULL\n");
		exit(1);
	}
	if (libgamma_const_of_connector_type(-1)) {
		fprintf(stderr, "libgamma_const_of_connector_type(<invalid>) != NULL\n");
		exit(1);
	}
	if (libgamma_value_of_connector_type("", &type) != LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED) {
		fprintf(stderr, "libgamma_value_of_connector_type(<invalid>) != LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED\n");
		exit(1);
	}
}


/**
 * Test functions for subpixel orders
 */
static void
test_subpixel_orders(void)
{
	size_t n = 0;
	char buf[128], *w;
	const char *r;
	enum libgamma_subpixel_order order;

#define X(CONST)\
	do {\
		if ((unsigned int)CONST >= (unsigned int)LIBGAMMA_SUBPIXEL_ORDER_COUNT) {\
			fprintf(stderr, "%s >= LIBGAMMA_SUBPIXEL_ORDER_COUNT\n", #CONST);\
			exit(1);\
		}\
		if (!libgamma_const_of_subpixel_order(CONST)) {\
			fprintf(stderr, "libgamma_const_of_subpixel_order(%s) == NULL\n", #CONST);\
			exit(1);\
		}\
		if (!libgamma_name_of_subpixel_order(CONST)) {\
			fprintf(stderr, "libgamma_name_of_subpixel_order(%s) == NULL\n", #CONST);\
			exit(1);\
		}\
		if (strcmp(libgamma_const_of_subpixel_order(CONST), #CONST)) {\
			fprintf(stderr, "libgamma_const_of_subpixel_order(%s) != \"%s\"\n", #CONST, #CONST);\
			exit(1);\
		}\
		order = (enum libgamma_subpixel_order)-1;\
		if (libgamma_value_of_subpixel_order(#CONST, &order) || order != CONST) {\
			fprintf(stderr, "libgamma_value_of_subpixel_order(\"%s\") != %s\n", #CONST, #CONST);\
			exit(1);\
		}\
		order = (enum libgamma_subpixel_order)-1;\
		if (libgamma_value_of_subpixel_order(libgamma_name_of_subpixel_order(CONST), &order) || order != CONST) {\
			fprintf(stderr, "libgamma_value_of_subpixel_order(libgamma_name_of_subpixel_order(%s)) != %s\n",\
			        #CONST, #CONST);\
			exit(1);\
		}\
		order = (enum libgamma_subpixel_order)-1;\
		if (libgamma_value_of_subpixel_order(#CONST, &order) || order != CONST) {\
			fprintf(stderr, "libgamma_value_of_subpixel_order(\"%s\") != %s\n", #CONST, #CONST);\
			exit(1);\
		}\
		if (strlen(libgamma_name_of_subpixel_order(CONST)) >= sizeof(buf)) {\
			fprintf(stderr, "strlen(libgamma_name_of_subpixel_order(%s) >= %zu\n", #CONST, sizeof(buf));\
			exit(1);\
		}\
		r = libgamma_name_of_subpixel_order(CONST);\
		for (w = buf; *r; r++)\
			*w++ = *r == ' ' ? '_' : *r;\
		*w = '\0';\
		if (strcasecmp(buf, &#CONST[sizeof("LIBGAMMA_SUBPIXEL_ORDER_") - 1])) {\
			fprintf(stderr, "libgamma_name_of_subpixel_order(%s) with '_' for ' ' != %s, ignoreing case\n",\
			        #CONST, &#CONST[sizeof("LIBGAMMA_SUBPIXEL_ORDER_") - 1]);\
			exit(1);\
		}\
		n += 1;\
	} while (0)

	X(LIBGAMMA_SUBPIXEL_ORDER_UNKNOWN);
	X(LIBGAMMA_SUBPIXEL_ORDER_NONE);
	X(LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB);
	X(LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_BGR);
	X(LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_RGB);
	X(LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_BGR);

#undef X

	if (n != LIBGAMMA_SUBPIXEL_ORDER_COUNT) {
		fprintf(stderr, "List of subpixel orders in `test_subpixel_orders` must be updated");
		exit(1);
	}

	if (libgamma_name_of_subpixel_order(-1)) {
		fprintf(stderr, "libgamma_name_of_subpixel_order(<invalid>) != NULL\n");
		exit(1);
	}
	if (libgamma_const_of_subpixel_order(-1)) {
		fprintf(stderr, "libgamma_const_of_subpixel_order(<invalid>) != NULL\n");
		exit(1);
	}
	if (libgamma_value_of_subpixel_order("", &order) != LIBGAMMA_SUBPIXEL_ORDER_NOT_RECOGNISED) {
		fprintf(stderr, "libgamma_value_of_subpixel_order(<invalid>) != LIBGAMMA_SUBPIXEL_ORDER_NOT_RECOGNISED\n");
		exit(1);
	}
}


/**
 * Test functions for errors
 */
static void
test_errors(void)
{
	int n = 0, fds[2], flags, err;
	char buf[1024], buf2[1100];
	ssize_t r;

	alarm(2);
	pipe(fds);
	flags = fcntl(fds[0], F_GETFL);
	fcntl(fds[0], F_SETFL, flags | O_NONBLOCK);
	err = dup(STDERR_FILENO);

#define X(CONST)\
	do {\
		if (CONST >= 0) {\
			fprintf(stderr, "%s >= 0\n", #CONST);\
			exit(1);\
		}\
		if (CONST < LIBGAMMA_ERROR_MIN) {\
			fprintf(stderr, "%s < LIBGAMMA_ERROR_MIN\n", #CONST);\
			exit(1);\
		}\
		if (!libgamma_name_of_error(CONST)) {\
			fprintf(stderr, "libgamma_name_of_error(%s) == NULL\n", #CONST);\
			exit(1);\
		}\
		if (strcmp(libgamma_name_of_error(CONST), #CONST)) {\
			fprintf(stderr, "libgamma_name_of_error(%s) != \"%s\"\n", #CONST, #CONST);\
			exit(1);\
		}\
		if (libgamma_value_of_error(#CONST) != CONST) {\
			fprintf(stderr, "libgamma_value_of_error(\"%s\") != %s\n", #CONST, #CONST);\
			exit(1);\
		}\
		libgamma_group_gid_set(0);\
		libgamma_group_name_set(NULL);\
		if (CONST != LIBGAMMA_ERRNO_SET) {\
			if (!libgamma_strerror(CONST)) {\
				fprintf(stderr, "libgamma_strerror(%s) == NULL\n", #CONST);\
				exit(1);\
			}\
			if (!libgamma_strerror_r(CONST, buf, sizeof(buf))) {\
				fprintf(stderr, "libgamma_strerror_r(%s, buf, sizeof(buf)) == NULL\n", #CONST);\
				exit(1);\
			}\
			if (strcmp(libgamma_strerror_r(CONST, buf, sizeof(buf)), libgamma_strerror(CONST))) {\
				fprintf(stderr, "libgamma_strerror_r(%s, buf, sizeof(buf)) != libgamma_strerror(%s)\n",\
				        #CONST, #CONST);\
				exit(1);\
			}\
			dup2(fds[1], STDERR_FILENO);\
			libgamma_perror(NULL, CONST);\
			fflush(stderr);\
			dup2(err, STDERR_FILENO);\
			r = read(fds[0], buf, sizeof(buf));\
			if (r <= 0 || buf[r - 1] != '\n') {\
				fprintf(stderr, "libgamma_perror(NULL, %s) failed\n", #CONST);\
				exit(1);\
			}\
			buf[r - 1] = '\0';\
			if (strcmp(buf, libgamma_strerror(CONST))) {\
				fprintf(stderr, "libgamma_perror(NULL, %s) failed\n", #CONST);\
				exit(1);\
			}\
			dup2(fds[1], STDERR_FILENO);\
			libgamma_perror("", CONST);\
			fflush(stderr);\
			dup2(err, STDERR_FILENO);\
			r = read(fds[0], buf, sizeof(buf));\
			if (r <= 0 || buf[r - 1] != '\n') {\
				fprintf(stderr, "libgamma_perror(\"\", %s) failed\n", #CONST);\
				exit(1);\
			}\
			buf[r - 1] = '\0';\
			if (strcmp(buf, libgamma_strerror(CONST))) {\
				fprintf(stderr, "libgamma_perror(\"\", %s) failed\n", #CONST);\
				exit(1);\
			}\
			dup2(fds[1], STDERR_FILENO);\
			libgamma_perror("prefix", CONST);\
			fflush(stderr);\
			dup2(err, STDERR_FILENO);\
			r = read(fds[0], buf, sizeof(buf));\
			if (r <= 0 || buf[r - 1] != '\n') {\
				fprintf(stderr, "libgamma_perror(\"prefix\", %s) failed\n", #CONST);\
				exit(1);\
			}\
			buf[r - 1] = '\0';\
			if (strncmp(buf, "prefix: ", 8)) {\
				fprintf(stderr, "libgamma_perror(\"prefix\", %s) failed\n", #CONST);\
				exit(1);\
			}\
			if (strcmp(&buf[8], libgamma_strerror(CONST))) {\
				fprintf(stderr, "libgamma_perror(\"prefix\", %s) failed\n", #CONST);\
				exit(1);\
			}\
		}\
		libgamma_group_gid_set((gid_t)n);\
		if (libgamma_group_gid_get() != (gid_t)n) {\
			fprintf(stderr, "libgamma_group_gid_get() != (gid_t)n\n");\
			exit(1);\
		}\
		n += 1;\
	} while (0)

	X(LIBGAMMA_ERRNO_SET);
	X(LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD);
	X(LIBGAMMA_NO_SUCH_SITE);
	X(LIBGAMMA_NO_SUCH_PARTITION);
	X(LIBGAMMA_NO_SUCH_CRTC);
	X(LIBGAMMA_IMPOSSIBLE_AMOUNT);
	X(LIBGAMMA_CONNECTOR_DISABLED);
	X(LIBGAMMA_OPEN_CRTC_FAILED);
	X(LIBGAMMA_CRTC_INFO_NOT_SUPPORTED);
	X(LIBGAMMA_GAMMA_RAMP_READ_FAILED);
	X(LIBGAMMA_GAMMA_RAMP_WRITE_FAILED);
	X(LIBGAMMA_GAMMA_RAMP_SIZE_CHANGED);
	X(LIBGAMMA_MIXED_GAMMA_RAMP_SIZE);
	X(LIBGAMMA_WRONG_GAMMA_RAMP_SIZE);
	X(LIBGAMMA_SINGLETON_GAMMA_RAMP);
	X(LIBGAMMA_LIST_CRTCS_FAILED);
	X(LIBGAMMA_ACQUIRING_MODE_RESOURCES_FAILED);
	X(LIBGAMMA_NEGATIVE_PARTITION_COUNT);
	X(LIBGAMMA_NEGATIVE_CRTC_COUNT);
	X(LIBGAMMA_DEVICE_RESTRICTED);
	X(LIBGAMMA_DEVICE_ACCESS_FAILED);
	X(LIBGAMMA_DEVICE_REQUIRE_GROUP);
	X(LIBGAMMA_GRAPHICS_CARD_REMOVED);
	X(LIBGAMMA_STATE_UNKNOWN);
	X(LIBGAMMA_CONNECTOR_UNKNOWN);
	X(LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED);
	X(LIBGAMMA_SUBPIXEL_ORDER_NOT_RECOGNISED);
	X(LIBGAMMA_EDID_LENGTH_UNSUPPORTED);
	X(LIBGAMMA_EDID_WRONG_MAGIC_NUMBER);
	X(LIBGAMMA_EDID_REVISION_UNSUPPORTED);
	X(LIBGAMMA_GAMMA_NOT_SPECIFIED);
	X(LIBGAMMA_EDID_CHECKSUM_ERROR);
	X(LIBGAMMA_GAMMA_NOT_SPECIFIED_AND_EDID_CHECKSUM_ERROR);
	X(LIBGAMMA_GAMMA_RAMPS_SIZE_QUERY_FAILED);
	X(LIBGAMMA_OPEN_PARTITION_FAILED);
	X(LIBGAMMA_OPEN_SITE_FAILED);
	X(LIBGAMMA_PROTOCOL_VERSION_QUERY_FAILED);
	X(LIBGAMMA_PROTOCOL_VERSION_NOT_SUPPORTED);
	X(LIBGAMMA_LIST_PARTITIONS_FAILED);
	X(LIBGAMMA_NULL_PARTITION);
	X(LIBGAMMA_NOT_CONNECTED);
	X(LIBGAMMA_REPLY_VALUE_EXTRACTION_FAILED);
	X(LIBGAMMA_EDID_NOT_FOUND);
	X(LIBGAMMA_LIST_PROPERTIES_FAILED);
	X(LIBGAMMA_PROPERTY_VALUE_QUERY_FAILED);
	X(LIBGAMMA_OUTPUT_INFORMATION_QUERY_FAILED);

#undef X

	if (-n != LIBGAMMA_ERROR_MIN) {
		fprintf(stderr, "List of errors in `test_errors` must be updated");
		exit(1);
	}

	if (libgamma_value_of_error(NULL)) {
		fprintf(stderr, "libgamma_value_of_error(NULL) != 0\n");
		exit(1);
	}
	if (libgamma_value_of_error("")) {
		fprintf(stderr, "libgamma_value_of_error(<invalid>) != 0\n");
		exit(1);
	}
	if (libgamma_name_of_error(1000)) {
		fprintf(stderr, "libgamma_name_of_error(<invalid>) != NULL\n");
		exit(1);
	}
	if (libgamma_strerror(LIBGAMMA_ERROR_MIN - 1)) {
		fprintf(stderr, "libgamma_strerror(LIBGAMMA_ERROR_MIN - 1) != NULL\n");
		exit(1);
	}
	if (!libgamma_strerror_r(LIBGAMMA_ERROR_MIN - 1, buf, sizeof(buf))) {
		fprintf(stderr, "libgamma_strerror_r(LIBGAMMA_ERROR_MIN - 1, buf, sizeof(buf)) == NULL\n");
		exit(1);
	}
	if (!libgamma_strerror_r(INT_MAX, buf, sizeof(buf))) {
		fprintf(stderr, "libgamma_strerror_r(INT_MAX, buf, sizeof(buf)) == NULL\n");
		exit(1);
	}

	/* Just a few of the errors */
	if (strcmp(libgamma_strerror(LIBGAMMA_NO_SUCH_SITE), "No such site")) {
		fprintf(stderr, "libgamma_strerror(LIBGAMMA_NO_SUCH_SITE) != \"No such site\"\n");
		exit(1);
	}
	if (strcmp(libgamma_strerror(LIBGAMMA_NO_SUCH_PARTITION), "No such partition")) {
		fprintf(stderr, "libgamma_strerror(LIBGAMMA_NO_SUCH_PARTITION) != \"No such partition\"\n");
		exit(1);
	}
	if (strcmp(libgamma_strerror(LIBGAMMA_NO_SUCH_CRTC), "No such CRTC")) {
		fprintf(stderr, "libgamma_strerror(LIBGAMMA_NO_SUCH_CRTC) != \"No such CRTC\"\n");
		exit(1);
	}
	if (strcmp(libgamma_strerror(LIBGAMMA_SINGLETON_GAMMA_RAMP), "Single-stop gamma ramp")) {
		fprintf(stderr, "libgamma_strerror(LIBGAMMA_SINGLETON_GAMMA_RAMP) != \"Single-stop gamma ramp\"\n");
		exit(1);
	}
	if (strcmp(libgamma_strerror(LIBGAMMA_GRAPHICS_CARD_REMOVED), "Graphics card was removed")) {
		fprintf(stderr, "libgamma_strerror(LIBGAMMA_GRAPHICS_CARD_REMOVED) != \"Graphics card was removed\"\n");
		exit(1);
	}
	if (strcmp(libgamma_strerror(LIBGAMMA_DEVICE_RESTRICTED), "Device is restricted to root")) {
		fprintf(stderr, "libgamma_strerror(LIBGAMMA_DEVICE_RESTRICTED) != \"Device is restricted to root\"\n");
		exit(1);
	}

	errno = -1;
	if (!libgamma_strerror(0)) {
		fprintf(stderr, "libgamma_strerror(0) == NULL\n");
		exit(1);
	}
	if (!libgamma_strerror(ENOMEM)) {
		fprintf(stderr, "libgamma_strerror(ENOMEM) == NULL\n");
		exit(1);
	}
	if (!libgamma_strerror(ERANGE)) {
		fprintf(stderr, "libgamma_strerror(ERANGE) == NULL\n");
		exit(1);
	}
	if (!libgamma_strerror(EDOM)) {
		fprintf(stderr, "libgamma_strerror(EDOM) == NULL\n");
		exit(1);
	}
	errno = 0;
	if (!libgamma_strerror(LIBGAMMA_ERRNO_SET)) {
		fprintf(stderr, "libgamma_strerror(0 via LIBGAMMA_ERRNO_SET) == NULL\n");
		exit(1);
	}
	errno = ENOMEM;
	if (!libgamma_strerror(LIBGAMMA_ERRNO_SET)) {
		fprintf(stderr, "libgamma_strerror(ENOMEM via LIBGAMMA_ERRNO_SET) == NULL\n");
		exit(1);
	}
	errno = ERANGE;
	if (!libgamma_strerror(LIBGAMMA_ERRNO_SET)) {
		fprintf(stderr, "libgamma_strerror(ERANGE via LIBGAMMA_ERRNO_SET) == NULL\n");
		exit(1);
	}
	errno = EDOM;
	if (!libgamma_strerror(LIBGAMMA_ERRNO_SET)) {
		fprintf(stderr, "libgamma_strerror(EDOM via LIBGAMMA_ERRNO_SET) == NULL\n");
		exit(1);
	}

	errno = -1;
	if (strcmp(libgamma_strerror(0), strerror(0))) {
		fprintf(stderr, "libgamma_strerror(0) != strerror(0)\n");
		exit(1);
	}
	if (strcmp(libgamma_strerror(ENOMEM), strerror(ENOMEM))) {
		fprintf(stderr, "libgamma_strerror(ENOMEM) != strerror(ENOMEM)\n");
		exit(1);
	}
	if (strcmp(libgamma_strerror(ERANGE), strerror(ERANGE))) {
		fprintf(stderr, "libgamma_strerror(ERANGE) != strerror(ERANGE)\n");
		exit(1);
	}
	if (strcmp(libgamma_strerror(EDOM), strerror(EDOM))) {
		fprintf(stderr, "libgamma_strerror(EDOM) != strerror(EDOM)\n");
		exit(1);
	}
	errno = 0;
	if (strcmp(libgamma_strerror(LIBGAMMA_ERRNO_SET), strerror(0))) {
		fprintf(stderr, "libgamma_strerror(0 via LIBGAMMA_ERRNO_SET) != strerror(0)\n");
		exit(1);
	}
	errno = ENOMEM;
	if (strcmp(libgamma_strerror(LIBGAMMA_ERRNO_SET), strerror(ENOMEM))) {
		fprintf(stderr, "libgamma_strerror(ENOMEM via LIBGAMMA_ERRNO_SET) != strerror(ENOMEM)\n");
		exit(1);
	}
	errno = ERANGE;
	if (strcmp(libgamma_strerror(LIBGAMMA_ERRNO_SET), strerror(ERANGE))) {
		fprintf(stderr, "libgamma_strerror(ERANGE via LIBGAMMA_ERRNO_SET) != strerror(ERANGE)\n");
		exit(1);
	}
	errno = EDOM;
	if (strcmp(libgamma_strerror(LIBGAMMA_ERRNO_SET), strerror(EDOM))) {
		fprintf(stderr, "libgamma_strerror(EDOM via LIBGAMMA_ERRNO_SET) != strerror(EDOM)\n");
		exit(1);
	}

	errno = -1;
	if (!libgamma_strerror_r(0, buf, sizeof(buf))) {
		fprintf(stderr, "libgamma_strerror_r(0, buf, sizeof(buf)) == NULL\n");
		exit(1);
	}
	if (!libgamma_strerror_r(ENOMEM, buf, sizeof(buf))) {
		fprintf(stderr, "libgamma_strerror_r(ENOMEM, buf, sizeof(buf)) == NULL\n");
		exit(1);
	}
	if (!libgamma_strerror_r(ERANGE, buf, sizeof(buf))) {
		fprintf(stderr, "libgamma_strerror_r(ERANGE, buf, sizeof(buf)) == NULL\n");
		exit(1);
	}
	if (!libgamma_strerror_r(EDOM, buf, sizeof(buf))) {
		fprintf(stderr, "libgamma_strerror_r(EDOM, buf, sizeof(buf)) == NULL\n");
		exit(1);
	}
	errno = 0;
	if (!libgamma_strerror_r(LIBGAMMA_ERRNO_SET, buf, sizeof(buf))) {
		fprintf(stderr, "libgamma_strerror_r(0 via LIBGAMMA_ERRNO_SET, buf, sizeof(buf)) == NULL\n");
		exit(1);
	}
	errno = ENOMEM;
	if (!libgamma_strerror_r(LIBGAMMA_ERRNO_SET, buf, sizeof(buf))) {
		fprintf(stderr, "libgamma_strerror_r(ENOMEM via LIBGAMMA_ERRNO_SET, buf, sizeof(buf)) == NULL\n");
		exit(1);
	}
	errno = ERANGE;
	if (!libgamma_strerror_r(LIBGAMMA_ERRNO_SET, buf, sizeof(buf))) {
		fprintf(stderr, "libgamma_strerror_r(ERANGE via LIBGAMMA_ERRNO_SET, buf, sizeof(buf)) == NULL\n");
		exit(1);
	}
	errno = EDOM;
	if (!libgamma_strerror_r(LIBGAMMA_ERRNO_SET, buf, sizeof(buf))) {
		fprintf(stderr, "libgamma_strerror_r(EDOM via LIBGAMMA_ERRNO_SET, buf, sizeof(buf)) == NULL\n");
		exit(1);
	}

	errno = -1;
#ifdef ERRNO_0_HAS_MESSAGE
	if (strcmp(libgamma_strerror_r(0, buf, sizeof(buf)), strerror(0))) {
		fprintf(stderr, "libgamma_strerror_r(0, buf, sizeof(buf)) != strerror(0)\n");
		exit(1);
	}
#endif
	if (strcmp(libgamma_strerror_r(ENOMEM, buf, sizeof(buf)), strerror(ENOMEM))) {
		fprintf(stderr, "libgamma_strerror_r(ENOMEM, buf, sizeof(buf)) != strerror(ENOMEM)\n");
		exit(1);
	}
	if (strcmp(libgamma_strerror_r(ERANGE, buf, sizeof(buf)), strerror(ERANGE))) {
		fprintf(stderr, "libgamma_strerror_r(ERANGE, buf, sizeof(buf)) != strerror(ERANGE)\n");
		exit(1);
	}
	if (strcmp(libgamma_strerror_r(EDOM, buf, sizeof(buf)), strerror(EDOM))) {
		fprintf(stderr, "libgamma_strerror_r(EDOM, buf, sizeof(buf)) != strerror(EDOM)\n");
		exit(1);
	}
#ifdef ERRNO_0_HAS_MESSAGE
	errno = 0;
	if (strcmp(libgamma_strerror_r(LIBGAMMA_ERRNO_SET, buf, sizeof(buf)), strerror(0))) {
		fprintf(stderr, "libgamma_strerror_r(0 via LIBGAMMA_ERRNO_SET, buf, sizeof(buf)) != strerror(0)\n");
		exit(1);
	}
#endif
	errno = ENOMEM;
	if (strcmp(libgamma_strerror_r(LIBGAMMA_ERRNO_SET, buf, sizeof(buf)), strerror(ENOMEM))) {
		fprintf(stderr, "libgamma_strerror_r(ENOMEM via LIBGAMMA_ERRNO_SET, buf, sizeof(buf)) != strerror(ENOMEM)\n");
		exit(1);
	}
	errno = ERANGE;
	if (strcmp(libgamma_strerror_r(LIBGAMMA_ERRNO_SET, buf, sizeof(buf)), strerror(ERANGE))) {
		fprintf(stderr, "libgamma_strerror_r(ERANGE via LIBGAMMA_ERRNO_SET, buf, sizeof(buf)) != strerror(ERANGE)\n");
		exit(1);
	}
	errno = EDOM;
	if (strcmp(libgamma_strerror_r(LIBGAMMA_ERRNO_SET, buf, sizeof(buf)), strerror(EDOM))) {
		fprintf(stderr, "libgamma_strerror_r(EDOM via LIBGAMMA_ERRNO_SET, buf, sizeof(buf)) != strerror(EDOM)\n");
		exit(1);
	}

	libgamma_group_name_set(NULL);
	if (libgamma_group_name_get()) {
		fprintf(stderr, "libgamma_group_name_get() != NULL\n");
		exit(1);
	}
	libgamma_group_name_set("group");
	if (!libgamma_group_name_get()) {
		fprintf(stderr, "libgamma_group_name_get() == NULL\n");
		exit(1);
	}
	if (strcmp(libgamma_group_name_get(), "group")) {
		fprintf(stderr, "libgamma_group_name_get() != \"group\"\n");
		exit(1);
	}
	libgamma_group_name_set("");
	if (libgamma_group_name_get()) {
		fprintf(stderr, "libgamma_group_name_get() != NULL\n");
		exit(1);
	}
	libgamma_group_name_set("name");
	if (!libgamma_group_name_get()) {
		fprintf(stderr, "libgamma_group_name_get() == NULL\n");
		exit(1);
	}
	if (strcmp(libgamma_group_name_get(), "name")) {
		fprintf(stderr, "libgamma_group_name_get() != \"name\"\n");
		exit(1);
	}
	libgamma_group_name_set(NULL);
	if (libgamma_group_name_get()) {
		fprintf(stderr, "libgamma_group_name_get() != NULL\n");
		exit(1);
	}

	libgamma_group_gid_set(5);
	snprintf(buf2, sizeof(buf2), "%s in group 5", libgamma_strerror(LIBGAMMA_DEVICE_REQUIRE_GROUP));

	dup2(fds[1], STDERR_FILENO);
	libgamma_perror(NULL, LIBGAMMA_DEVICE_REQUIRE_GROUP);
	fflush(stderr);
	dup2(err, STDERR_FILENO);
	r = read(fds[0], buf, sizeof(buf));
	if (r <= 0 || buf[r - 1] != '\n') {
		fprintf(stderr, "libgamma_perror(NULL, LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n"); 
		exit(1);
	}
	buf[r - 1] = '\0';
	if (strcmp(buf, buf2)) {
		fprintf(stderr, "libgamma_perror(NULL, LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}

	dup2(fds[1], STDERR_FILENO);
	libgamma_perror("", LIBGAMMA_DEVICE_REQUIRE_GROUP);
	fflush(stderr);
	dup2(err, STDERR_FILENO);
	r = read(fds[0], buf, sizeof(buf));
	if (r <= 0 || buf[r - 1] != '\n') {
		fprintf(stderr, "libgamma_perror(\"\", LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}
	buf[r - 1] = '\0';
	if (strcmp(buf, buf2)) {
		fprintf(stderr, "libgamma_perror(\"\", LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}

	dup2(fds[1], STDERR_FILENO);
	libgamma_perror("prefix", LIBGAMMA_DEVICE_REQUIRE_GROUP);
	fflush(stderr);
	dup2(err, STDERR_FILENO);
	r = read(fds[0], buf, sizeof(buf));
	if (r <= 0 || buf[r - 1] != '\n') {
		fprintf(stderr, "libgamma_perror(\"prefix\", LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}
	buf[r - 1] = '\0';
	if (strncmp(buf, "prefix: ", 8)) {
		fprintf(stderr, "libgamma_perror(\"prefix\", LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}
	if (strcmp(&buf[8], buf2)) {
		fprintf(stderr, "libgamma_perror(\"prefix\", LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}

	libgamma_group_gid_set(5);
	libgamma_group_name_set("grp");
	snprintf(buf2, sizeof(buf2), "%s in the grp group (5)", libgamma_strerror(LIBGAMMA_DEVICE_REQUIRE_GROUP));

	dup2(fds[1], STDERR_FILENO);
	libgamma_perror(NULL, LIBGAMMA_DEVICE_REQUIRE_GROUP);
	fflush(stderr);
	dup2(err, STDERR_FILENO);
	r = read(fds[0], buf, sizeof(buf));
	if (r <= 0 || buf[r - 1] != '\n') {
		fprintf(stderr, "libgamma_perror(NULL, LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n"); 
		exit(1);
	}
	buf[r - 1] = '\0';
	if (strcmp(buf, buf2)) {
		fprintf(stderr, "libgamma_perror(NULL, LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}

	dup2(fds[1], STDERR_FILENO);
	libgamma_perror("", LIBGAMMA_DEVICE_REQUIRE_GROUP);
	fflush(stderr);
	dup2(err, STDERR_FILENO);
	r = read(fds[0], buf, sizeof(buf));
	if (r <= 0 || buf[r - 1] != '\n') {
		fprintf(stderr, "libgamma_perror(\"\", LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}
	buf[r - 1] = '\0';
	if (strcmp(buf, buf2)) {
		fprintf(stderr, "libgamma_perror(\"\", LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}

	dup2(fds[1], STDERR_FILENO);
	libgamma_perror("prefix", LIBGAMMA_DEVICE_REQUIRE_GROUP);
	fflush(stderr);
	dup2(err, STDERR_FILENO);
	r = read(fds[0], buf, sizeof(buf));
	if (r <= 0 || buf[r - 1] != '\n') {
		fprintf(stderr, "libgamma_perror(\"prefix\", LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}
	buf[r - 1] = '\0';
	if (strncmp(buf, "prefix: ", 8)) {
		fprintf(stderr, "libgamma_perror(\"prefix\", LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}
	if (strcmp(&buf[8], buf2)) {
		fprintf(stderr, "libgamma_perror(\"prefix\", LIBGAMMA_DEVICE_REQUIRE_GROUP) failed\n");
		exit(1);
	}

	libgamma_group_gid_set(0);
	libgamma_group_name_set(NULL);

	close(fds[0]);
	close(fds[1]);
	close(err);
	alarm(0);
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
	struct libgamma_site_state      *site_state = malloc(sizeof(*site_state));
	struct libgamma_partition_state *part_state = malloc(sizeof(*part_state));
	struct libgamma_crtc_state      *crtc_state = malloc(sizeof(*crtc_state));
	struct libgamma_crtc_information info;
#define X(RAMPS)\
	struct libgamma_gamma_##RAMPS old_##RAMPS, RAMPS;\
	libgamma_gamma_##RAMPS##_fun *f_##RAMPS = dim_##RAMPS;
	LIST_RAMPS(X)
#undef X
	size_t i, n;
	int r, rr = 0;

	/* Test miscellaneous parts of the library */
	test_count_consts();
	test_connector_types();
	test_subpixel_orders();
	test_errors();
	list_methods_lists();
	method_availability();
	list_default_sites();
	method_capabilities();

	/* Select monitor for tests over CRTC:s, partitions and sites */
	if (select_monitor(site_state, part_state, crtc_state))
		return 1;

	/* Test CRTC information functions */
	crtc_information(crtc_state);

	/* Get the sizes of the gamma ramps for the selected CRTC */
	libgamma_get_crtc_information(&info, sizeof(info), crtc_state, LIBGAMMA_CRTC_INFO_GAMMA_SIZE);

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
#define Y(R, C)  printf("  \033[3" C "m%16llX\033[00m", (unsigned long long int)(R[i]))
	LIST_INTEGER_RAMPS(X)
#undef Y
#undef X

	/* Test order of gamma ramps */
	memcpy(ramps16.red, old_ramps16.red, ramps16.red_size * sizeof(*ramps16.red));
	memset(ramps16.green, 0, ramps16.green_size * sizeof(*ramps16.green));
	memset(ramps16.blue, 0, ramps16.blue_size * sizeof(*ramps16.blue));
	printf("Making the monitor red-only for 1 second...\n");
	if ((rr |= r = libgamma_crtc_set_gamma_ramps16(crtc_state, &ramps16)))
		libgamma_perror("libgamma_crtc_set_gamma_ramps16", r);
	sleep(1);
	memset(ramps16.red, 0, ramps16.red_size * sizeof(*ramps16.red));
	memcpy(ramps16.green, old_ramps16.green, ramps16.green_size * sizeof(*ramps16.green));
	printf("Making the monitor green-only for 1 second...\n");
	if ((rr |= r = libgamma_crtc_set_gamma_ramps16(crtc_state, &ramps16)))
		libgamma_perror("libgamma_crtc_set_gamma_ramps16", r);
	sleep(1);
	memset(ramps16.green, 0, ramps16.green_size * sizeof(*ramps16.green));
	memcpy(ramps16.blue, old_ramps16.blue, ramps16.blue_size * sizeof(*ramps16.blue));
	printf("Making the monitor blue-only for 1 second...\n");
	if ((rr |= r = libgamma_crtc_set_gamma_ramps16(crtc_state, &ramps16)))
		libgamma_perror("libgamma_crtc_set_gamma_ramps16", r);
	sleep(1);
	if ((rr |= r = libgamma_crtc_set_gamma_ramps64(crtc_state, &old_ramps64)))
		libgamma_perror("libgamma_crtc_set_gamma_ramps64", r);
	printf("Done!\n");

	/* Test inversion (does not work properly on all devices) */
	printf("Inverting monitor output for 1 second... (does not work properly on all devices) (ramps16)\n");
	if ((rr |= r = libgamma_crtc_set_gamma_ramps16_f(crtc_state, inv_ramps16, inv_ramps16, inv_ramps16)))
		libgamma_perror("libgamma_crtc_set_gamma_ramps16_f", r);
	sleep(1);
	if ((rr |= r = libgamma_crtc_set_gamma_ramps16(crtc_state, &old_ramps16)))
		libgamma_perror("libgamma_crtc_set_gamma_ramps16", r);
	printf("Done!\n");
	sleep(1);

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
