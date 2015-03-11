#pragma once

#include <memory>

#include "../application/application.hpp"
#include "../core/event/event.hpp"
#include "../core/process_event_ctx.h"


////////////////////////////////////////////////////////////////////////////////

namespace eedit
{

class application;

namespace core
{

enum build_screen_list_hints {
	build_screen_no_hints = 0,
	rewind_screen         = 1,
	resync_screen         = 2,
};


struct selection_record_s {
	u64 bid;

	int b_x;
	int b_y;
	u64 b_offset;
	bool use = false;

	int e_x;
	int e_y;
	u64 e_offset;

	codepoint_info_s start_cpi;
};


struct core_context_t {
	std::mutex m_mtx;
	bool core_started = false;
	bool core_running = true;
	event_queue<eedit::core::event *> m_msg_queue;
};

////////////////////////////////////////////////////////////////////////////////

// FIXME: rename
struct screen_cache {
	screen_cache()
	{
		assert(last_screen == nullptr);
	}

	~screen_cache()
	{
		screen_release(last_screen);
	}

	screen_dimension_t dim;
	u64 start_offset = 0;
	screen_t * last_screen = nullptr;
};


// FIXME: cleanup/remove all this ????
void main(std::shared_ptr<application> app);

bool push_event(eedit::core::event * msg);

bool process_rpc_call_event(eedit::core::rpc_call * msg);

screen_cache * get_screen_cache(u64 id);

void set_last_screen(u64 id, screen_t * scr);

screen_t * get_last_screen(u64 id);

bool notify_buffer_changes(event * msg, codepoint_info_s * start_cpi, bool send_screen = false);

screen_t * get_previous_screen_by_id(u64 id);

screen_t * get_new_screen_by_id(u64 screen_id);

bool resync_screen_layout(uint64_t editor_buffer_id, uint64_t bid, u64 screen_id, screen_dimension_t & dim);


bool setup_screen_by_id(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, editor_view_id_t sid, screen_dimension_t & dim);

void send_event_to_ui(const eedit::core::event * ev_in, eedit::core::event * ev_out);

void send_new_layout_event_to_ui(const eedit::core::event * ev_in, screen_t * screen);

bool build_screen_layout(event * msg, const codepoint_info_s * start_cpi, screen_t * scr);


bool save_buffer(event * msg);

bool dump_buffer_log(event * msg);

bool quit_editor(event * msg);


bool eval_input_event(event * base_msg);
bool process_input_event(event * msg);
bool release_event(event * msg);

bool notify_buffer_changes(event * msg, codepoint_info_s * start_cpi, bool send_screen);

bool process_build_layout_event(eedit::core::layout_event * msg);

bool check_input_msg(event * msg);

bool process_application_event(core_context_t * core_ctx, application_event * msg);

bool process_event(core_context_t * core_ctx, event * msg);



} // ! namespace core

} // ! namespace eedit


