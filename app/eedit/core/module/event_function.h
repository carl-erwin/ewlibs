#pragma once

#include "editor_export.h"
#include "editor_event.h"


extern "C" {

    typedef enum {
        EDITOR_STATUS_OK     = 0,
        EDITOR_STATUS_ERROR,
        EDITOR_STATUS_FATAL_ERROR,
    } editor_module_status_t;

    typedef int (*editor_module_function_t)(struct editor_event_s * ev); // int -> editor_module_status_t

    EDITOR_EXPORT
    int editor_register_module_function(const char * name, editor_module_function_t);

    EDITOR_EXPORT
    editor_module_function_t editor_get_module_function(const char * name);

}
