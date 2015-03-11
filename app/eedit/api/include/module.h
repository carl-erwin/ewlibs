#pragma once

// skeleton
extern "C" {


	typedef enum eedit_module_type_e {
		MODULE_TYPE_INVALID = 0,
		MODULE_TYPE_CODEC,        // see codec.h
		MODULE_TYPE_FILTER,       // see filter.h
		MODULE_TYPE_EDITOR_MODE,  // see mode.h ??
	} eedit_module_type_e;

	typedef enum eedit_module_init_status_e {
		MODULE_INIT_OK    = 0,
		MODULE_INIT_ERROR = 1,
	} eedit_module_init_status_e;


	const char * module_name();

	const char * module_version();

	const char * module_depends();

	eedit_module_type_e  module_type();

	eedit_module_init_status_e  module_init();  // module register there function here   by symbol ex: multicursor:clone-cursor-down/up/left/right multicuror:enable/disable

	int  module_quit();

} // extern "C"
/////
