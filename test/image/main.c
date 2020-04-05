/* main.c  -  Image library  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform image loading library in C11 for projects
 * based on our foundation library.
 *
 * The latest source code maintained by Mattias Jansson is always available at
 *
 * https://github.com/mjansson/image_lib
 *
 * This library is built on top of the foundation library available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any
 * restrictions.
 *
 */

#include <image/image.h>

#include <foundation/foundation.h>
#include <test/test.h>

static application_t
test_image_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Image tests"));
	app.short_name = string_const(STRING_CONST("test_image"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_image_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_image_foundation_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_image_initialize(void) {
	image_config_t config;
	memset(&config, 0, sizeof(config));
	log_set_suppress(HASH_IMAGE, ERRORLEVEL_INFO);
	return image_module_initialize(config);
}

static void
test_image_finalize(void) {
	image_module_finalize();
}

DECLARE_TEST(image, create) {
	return 0;
}

static void
test_image_declare(void) {
	ADD_TEST(image, create);
}

static test_suite_t test_image_suite = {test_image_application,
                                        test_image_memory_system,
                                        test_image_foundation_config,
                                        test_image_declare,
                                        test_image_initialize,
                                        test_image_finalize,
                                        0};

#if BUILD_MONOLITHIC

int
test_umage_run(void);

int
test_image_run(void) {
	test_suite = test_image_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_image_suite;
}

#endif
