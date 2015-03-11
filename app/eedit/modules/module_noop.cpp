#include "ew/ew_config.hpp"
#include "./api/include/module.h"


extern "C"
SHOW_SYMBOL const char * module_name()
{
	return "dummy";
}


extern "C"
SHOW_SYMBOL const char * module_version()
{
	return "1.0.0";
}


extern "C"
SHOW_SYMBOL const char * module_depends()
{
	return "";
}

extern "C"
SHOW_SYMBOL eedit_module_type_e  module_type()
{
	return MODULE_TYPE_FILTER;
}

extern "C"
SHOW_SYMBOL eedit_module_init_status_e  module_init()
{
	return MODULE_INIT_OK;
}

extern "C"
SHOW_SYMBOL int  module_quit()
{
	return 0;
}
