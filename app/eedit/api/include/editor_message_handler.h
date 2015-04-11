#pragma once

#include "editor_export.h"
#include "editor_message.h"


extern "C" {

    // move to editor_status.h
    typedef enum {
        EDITOR_STATUS_OK     = 0,
        EDITOR_STATUS_ERROR,
        EDITOR_STATUS_FATAL_ERROR,
    } editor_module_status_t;

    typedef int (*editor_message_handler_t)(struct editor_message_s * ev); // int -> editor_module_status_t

    EDITOR_EXPORT
    int editor_register_message_handler(const char * name, editor_message_handler_t);

    EDITOR_EXPORT
    editor_message_handler_t editor_get_message_handler(const char * name);

}
