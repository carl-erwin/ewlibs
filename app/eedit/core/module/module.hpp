#pragma once

/*
  provide a way to publish module apis
*/


// TODO: export event to C-api
#ifdef __DUMMY__

extern "C"
struct editor_event_s {
	uint64_t buffer_id;
	uint64_t byte_buffer_id; // filled internally 0
	uint64_t view_id;

	screen_dimension  screem_dim;

// with screen_id, buffer_id --> editor_buffer_view { codec, input_map },

	enum event_type {
		key,
		mouse,
		paste,
		layout,
		drag-drop,system-quit,etc..
	};
	union event_data {

	};

};

#endif


extern "C" {

	typedef int (*module_fn)(void * event); // FIXME: move to json-rpc | ac/av ?, TODO: use const editor_event_s * event

	int       editor_register_module_function(const char * name, module_fn);
	module_fn editor_get_module_function(const char * name);

}
