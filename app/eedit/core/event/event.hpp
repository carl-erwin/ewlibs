#pragma once

/*
  the editor event api
*/

#include "./api/include/editor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum editor_event_type_e {

	EDITOR_EVENT_TYPE_FAMILY_MASK       = 0xFF00,

	INVALID_EDITOR_EVENT                = 0,

	// ui -> core
	EDITOR_RESIZE_EVENT                 = 0x0100, //
	EDITOR_KEYBOARD_EVENT               = 0x0200, // on press events

	EDITOR_POINTER_BUTTON_EVENT_FAMILY  = 0x0300,
	EDITOR_POINTER_BUTTON_PRESS_EVENT   = EDITOR_POINTER_BUTTON_EVENT_FAMILY  | 0x1,
	EDITOR_POINTER_BUTTON_RELEASE_EVENT = EDITOR_POINTER_BUTTON_EVENT_FAMILY  | 0x2,
	EDITOR_POINTER_MOTION_EVENT         = 0x0600,

	EDITOR_RPC_CALL_EVENT               = 0x0700,

	// ... -> core
	EDITOR_BUILD_LAYOUT_EVENT           = 0x0800,

	// core -> ui
	EDITOR_LAYOUT_NOTIFICATION_EVENT    = 0x0900,
	EDITOR_RPC_ANSWER_EVENT             = 0x0A00,

	// ui <-> core (bidirectionnal)
	EDITOR_APPLICATION_EVENT_FAMILY     = 0x0B00,
	EDITOR_QUIT_APPLICATION_DEFAULT     = EDITOR_APPLICATION_EVENT_FAMILY | 0x1,
	EDITOR_QUIT_APPLICATION_FORCED      = EDITOR_APPLICATION_EVENT_FAMILY | 0x2,

	// system -> core
	EDITOR_SYSTEM_EVENT                 = 0x0F00,  // halt, reboot, suspend, resume ?
};


enum editor_actor_kind_e {
	EDITOR_ACTOR_INVALID_KIND,
	EDITOR_ACTOR_SYSTEM,
	EDITOR_ACTOR_CORE,
	EDITOR_ACTOR_UI
};


#ifdef __cplusplus
}
#endif



#include <memory>
#include <functional>


#include <ew/Utils.hpp>

#include "../../core/input/event/input_event_map.hpp"
#include "../../api/include/screen.h"
#include "../../core/message_queue.hpp"



// move to "C" api
// export the editor_event_s
// add "C" constructors to build all public events
// add "C" destructors to relesase built events

namespace eedit
{

namespace core
{



struct event;

struct event_address {
	editor_actor_kind_e kind = EDITOR_ACTOR_INVALID_KIND;
	event_queue<::eedit::core::event *> * queue = nullptr;
};


struct event_context {
	editor_buffer_id_t editor_buffer_id = 0;
	byte_buffer_id_t   byte_buffer_id   = 0;  // will be derived from editor_buffer_id
	editor_view_id_t   view_id          = 0;
	screen_dimension_t screen_dim;
};

struct event {
	editor_event_type_e type = INVALID_EDITOR_EVENT;

	u32 id = 0; // TODO: remove ?

	event_address src;
	event_address dst;


	editor_buffer_id_t editor_buffer_id = 0;
	byte_buffer_id_t   byte_buffer_id   = 0;  // will be derived from editor_buffer_id
	editor_view_id_t   view_id          = 0;
	screen_dimension_t screen_dim;

public:
	virtual ~event() {}

protected:
	event(editor_event_type_e _type)
	{
		type = _type;
	}
};


struct application_event : public event {
	application_event(editor_event_type_e type)
		: event(type)
	{
	}

	virtual ~application_event()
	{
	}

};



/* rpc_call/rpc_answer are use to call textual api
 * TODO: editor_rpc.h
 * add get_buffer_id_list
 */


// move  to utils !?
static inline void duplicate_args(const int in_ac, const char ** in_av,
				  int & out_ac,  char ** & out_av)
{
	out_ac = in_ac;
	out_av = new char * [out_ac];
	for (int i = 0; i < out_ac; ++i)
		out_av[i] = ew::utils::c_string_dup(in_av[i]);
}

static inline void release_args(const int ac, char ** & av)
{
	for (int i = 0; i < ac; ++i)
		delete [] av[i];
	delete [] av;
	av = 0;
}

static inline void print_args(const int ac, char ** & av)
{
	for (int i = 0; i < ac; ++i) {
		app_log <<  "av[" << i << "] = " << av[i] <<  "\n";
	}

}


struct rpc_call : public event {

	rpc_call(const int ac_,  const char ** av_) : event(EDITOR_RPC_CALL_EVENT)
	{
		duplicate_args(ac_, av_, ac,  av);
		//print_args(ac, av);
	}

	virtual ~rpc_call()
	{
		release_args(ac, av);
	}

	int  ac    = 0;
	char ** av = nullptr;
};

struct rpc_answer : public event {
	rpc_answer(const rpc_call * request,  const int ac_,  const char ** av_) : event(EDITOR_RPC_ANSWER_EVENT)
	{
		if (request)
			this->id = request->id;

		duplicate_args(ac_, av_, ac,  av);
		//print_args(ac, av);
	}

	virtual ~rpc_answer()
	{
		release_args(ac, av);
	}

	int  ac    = 0;
	char ** av = nullptr;
};


struct input_event : public event {
	input_event(editor_event_type_e type)
		: event(type)
	{

	}

	::eedit::input_event_s * ev      = nullptr; // move to ? src_ctx ?

	virtual ~input_event()
	{
		delete ev; // per event type
	}

};


struct keyboard_event : public input_event {
	keyboard_event() : input_event(EDITOR_KEYBOARD_EVENT)
	{

	}

	virtual ~keyboard_event()
	{
	}

};



struct button_event : public input_event {
	button_event(editor_event_type_e t) : input_event(t)
	{
	}

	virtual ~button_event()
	{
	}

};


struct button_press_event : public button_event {
	button_press_event() : button_event(EDITOR_POINTER_BUTTON_PRESS_EVENT)
	{
	}
};

struct button_release_event : public button_event {
	button_release_event() : button_event(EDITOR_POINTER_BUTTON_RELEASE_EVENT)
	{
	}
};



struct layout_event : public event {

	layout_event(enum editor_event_type_e t) : event(t)
	{

	}

	screen_t * screen = nullptr;

	virtual ~layout_event()
	{

	}

};


struct resize_event : public event {
	resize_event(enum editor_event_type_e t) : event(t)
	{

	}

	virtual ~resize_event()
	{
		// the screen : MUST be explicitly destroyed
	}

};



} // ! namespace core

} // ! namespace eedit
