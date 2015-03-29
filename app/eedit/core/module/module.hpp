#pragma once

#include <ew/ew_config.hpp>

/*
  provide a way to publish module apis
*/


// TODO: export event to C-api
#ifdef __DUMMY__

extern "C"
struct editor_event_s {
    uint64_t buffer_id;
    uint64_t byte_buffer_id; // use internally internally 0
    uint64_t view_id;

    screen_dimension  screem_dim;

// with screen_id, buffer_id --> editor_buffer_view { codec, input_map },

    enum event_type {
        key,
        mouse,
        paste,
        layout,
        drag_and_drop,
        system_quit,
        etc..
    };

    union event_data {

    };

};

#endif


// FIXME: MOVE TO APIS

extern "C" {

    typedef int (*module_fn)(void * event); // FIXME: move to editor_event_s * event

    SHOW_SYMBOL
    int       editor_register_module_function(const char * name, module_fn);

    SHOW_SYMBOL
    module_fn editor_get_module_function(const char * name);

}
