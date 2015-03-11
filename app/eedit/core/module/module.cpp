#include <iostream>
#include <map>
#include <string>
#include "module.hpp"

#include "core/log.hpp"

namespace eedit
{

std::map<std::string, ::module_fn> modfunc_table;

int register_module_function(const char * name, module_fn fn)
{
	auto ret = modfunc_table.insert(std::pair<std::string, module_fn>(std::string(name), fn));
	if (ret.second == false) {
		app_log << "function '" << name << "' already existed\n";
		app_log << " with a value of " << ret.first->second << '\n';
		return -1;
	}

	return 0;
}

::module_fn get_module_function(const char * name)
{
	auto  ret = modfunc_table.find(std::string(name));
	if (ret != modfunc_table.end()) {
		return *ret->second;
	}
	app_log << __PRETTY_FUNCTION__ << " function '" << name << "' not found\n";
	return nullptr;
}
}


extern "C" {

	int eedit_register_module_function(const char * name, module_fn fn)
	{
		return eedit::register_module_function(name, fn);
	}

	module_fn eedit_get_module_function(const char * name)
	{
		return eedit::get_module_function(name);
	}

}
