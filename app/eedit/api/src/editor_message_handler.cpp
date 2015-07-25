#include <iostream>
#include <map>
#include <string>

#include "editor_message_handler.h"

#include "core/log/log.hpp"

namespace eedit
{

std::map<std::string, ::editor_message_handler_t> modfunc_table;

int register_module_function(const char * name, editor_message_handler_t fn)
{
    auto ret = modfunc_table.insert(std::pair<std::string, editor_message_handler_t>(std::string(name), fn));
    if (ret.second == false) {
        app_logln(-1, "function '%s' already exists with a value of %p", name, ret.first->second);
        return -1;
    }

    return 0;
}

::editor_message_handler_t get_module_function(const char * name)
{
    auto  ret = modfunc_table.find(std::string(name));
    if (ret != modfunc_table.end()) {
        return *ret->second;
    }
    
    app_logln(-1, "function '%s' not found ", name);

    return nullptr;
}
}


extern "C" {

    int editor_register_message_handler(const char * name, editor_message_handler_t fn)
    {
        return eedit::register_module_function(name, fn);
    }

    editor_message_handler_t editor_get_message_handler(const char * name)
    {
        return eedit::get_module_function(name);
    }

}
