#include <iostream>
#include <map>
#include <string>
#include "event_function.h"

#include "core/log/log.hpp"

namespace eedit
{

std::map<std::string, ::editor_module_function_t> modfunc_table;

int register_module_function(const char * name, editor_module_function_t fn)
{
    auto ret = modfunc_table.insert(std::pair<std::string, editor_module_function_t>(std::string(name), fn));
    if (ret.second == false) {
        app_log << "function '" << name << "' already existed\n";
        app_log << " with a value of " << ret.first->second << '\n';
        return -1;
    }

    return 0;
}

::editor_module_function_t get_module_function(const char * name)
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

    int editor_register_module_function(const char * name, editor_module_function_t fn)
    {
        return eedit::register_module_function(name, fn);
    }

    editor_module_function_t editor_get_module_function(const char * name)
    {
        return eedit::get_module_function(name);
    }

}
