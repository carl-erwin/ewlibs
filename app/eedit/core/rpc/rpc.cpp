#include "../../core/core.hpp"
#include "../../core/event/event.hpp"

#include "editor_buffer.h"
#include "editor_view.h"

#include "rpc.hpp"

#include "../process_event_ctx.h"

namespace eedit
{

namespace core
{

void send_rpc_answer(const eedit::core::event * ev_in,   rpc_answer * msg)
{
	assert (ev_in->src.queue);
	if (ev_in->src.queue == nullptr) {
		delete msg;
		return;
	}

	msg->src = ev_in->dst;
	msg->dst = ev_in->src;
	msg->dst.queue->push(msg);
}

// FIXME: move the application class to core
//
void get_buffer_id_list(eedit::core::rpc_call *request, int ac,  char * av[])
{
	app_log << __PRETTY_FUNCTION__ << "\n";

	auto app = get_application();

	auto & list = app->buffer_desc_lst();

	char * ans_av[1 + list.size()];
	int i  = 1;
	ans_av[0] = ew::utils::c_string_dup("get_buffer_id_list");
	for (auto & ebid : list) {
		char buffer[32];
		snprintf(buffer,  sizeof (buffer),  "%lu", (uint64_t)ebid);
		ans_av[i] = ew::utils::c_string_dup(buffer);
		++i;
	}
	rpc_answer * rpc_ans = new rpc_answer(request, i, (const char **)ans_av);
	send_rpc_answer(request, rpc_ans);
}

void set_screen_id_start_offset(eedit::core::rpc_call *request, int ac,  char * av[])
{
	// orig -> core -> rpc -> core -> orig
	auto msg             = new eedit::core::layout_event(EDITOR_BUILD_LAYOUT_EVENT);
	msg->src             = request->src;
	msg->dst.kind        = EDITOR_ACTOR_CORE;


	assert(request->byte_buffer_id);

	msg->editor_buffer_id = request->editor_buffer_id;
	msg->byte_buffer_id   = request->byte_buffer_id; //
	msg->view_id          = request->view_id; //
	msg->screen_dim       = request->screen_dim;

	// TODO: move to offset, add offset to msg + resync flag ?
	//       set resync flag
	auto editor_buffer_id = request->editor_buffer_id;

	auto view  = editor_buffer_check_view_id(editor_buffer_id, request->view_id);
	if (!view) {
		assert(0);
		return;
	}


	u64 target_offset = atof(av[2]);

	// check overflow
	size_t buf_size;
	byte_buffer_size(request->byte_buffer_id, &buf_size);
	if (target_offset > buf_size)
		target_offset = buf_size;

	u64 cur_start_offset = editor_view_get_start_offset(view);
	if (target_offset == cur_start_offset) {
		return;
	}

	app_log << " target offset to : " << target_offset << "\n";

	// auto screen = get_scbuffer_by_id(request->buffer_id);
	// if offset on screen -> return

	editor_view_set_start_offset(view, target_offset);

	codepoint_info_s cpi;
	codepoint_info_reset(&cpi);
	cpi.offset = target_offset;
	cpi.used = true;

	set_ui_next_screen_start_cpi(request->editor_buffer_id, request->byte_buffer_id, request->view_id, &cpi);
	set_ui_change_flag(request->editor_buffer_id, request->byte_buffer_id, request->view_id);
}



bool process_rpc_call_event(eedit::core::rpc_call * msg)
{
	if (msg->ac == 0) {
		return false;
	}

	auto f_name = std::string(msg->av[0]);

	if (f_name == "get_buffer_id_list") {
		get_buffer_id_list(msg, msg->ac-1, msg->av+1);
	}

	if (f_name == "set_screen_id_start_offset") {
		set_screen_id_start_offset(msg, msg->ac-1, msg->av+1);
	}

	return true;
}

} // ! namespace core

} // ! namespace eedit
