#include "../../core/core.hpp"

#include "../../core/module/module.hpp"

#include "editor_view.h"
#include "../../api/src/editor_view_internal.h"


namespace eedit
{

namespace core
{


bool process_event(core_context_t * core_ctx, event * msg)
{
	app_log << __PRETTY_FUNCTION__ << "\n";

	bool ret = false;

//FIXME:	editor_view_reset_flags(msg->editor_buffer_id, msg->view_id);
	reset_buffer_view_flags(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
	app_log << " reset_buffer_view_flags..ok\n";
	// refresh screen cache

	setup_screen_by_id(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, msg->screen_dim);
	app_log << " setup_screen_by_id..ok\n";

	auto buffer = msg->editor_buffer_id;
	auto view   = msg->view_id;


	// fix message
	msg->byte_buffer_id = editor_buffer_get_byte_buffer_id(buffer);


	if (buffer == INVALID_EDITOR_BUFFER_ID) {
		app_log << " no buffer_id defined\n";
	}

	switch (msg->type & EDITOR_EVENT_TYPE_FAMILY_MASK) {

	case EDITOR_APPLICATION_EVENT_FAMILY: {
		ret = process_application_event(core_ctx, static_cast<eedit::core::application_event *>(msg));
	}
	break;

	case EDITOR_KEYBOARD_EVENT:
	case EDITOR_POINTER_BUTTON_EVENT_FAMILY: {
		app_log << __PRETTY_FUNCTION__ << " EDITOR_KEYB/POINTER EVENT\n";

		check_input_msg(msg);
		ret = process_input_event(msg);
	}
	break;

	case EDITOR_BUILD_LAYOUT_EVENT: {
		app_log << __PRETTY_FUNCTION__ << " EDITOR_BUILD_LAYOUT_EVENT\n";

		check_input_msg(msg);
		ret = process_build_layout_event(static_cast<eedit::core::layout_event *>(msg));
	}
	break;

	case EDITOR_RPC_CALL_EVENT: {
		ret = eedit::core::process_rpc_call_event(static_cast<eedit::core::rpc_call *>(msg));
	}
	break;

	default: {
		app_log <<  "core : receive : unhandled event type " <<  (void *)msg->type << "\n";
	}
	break;
	}


	codepoint_info_s * start_cpi = nullptr;
	codepoint_info_s start_cpi_ref;

	if (view && editor_view_get_ui_must_resync(view)) {
		resync_screen_layout(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, msg->screen_dim);
		//start_cpi = view->start_cpi_ptr; // temporary
	}

	//    app_log <<  " call notify_buffer_changes : start_cpi = " << process_ev_ctx.start_cpi_ptr << "\n";
	if (view) {
		editor_view_get_start_cpi(view, &start_cpi_ref);
		if (start_cpi_ref.used) {
			start_cpi = &start_cpi_ref;
			app_log <<  "  start_cpi->offset      = " << start_cpi->offset << "\n";
			app_log <<  "  start_cpi->split_count = " << start_cpi->split_count << "\n";
			app_log <<  "  start_cpi->split_flag  = " << start_cpi->split_flag << "\n";
		}
	}

	notify_buffer_changes(msg, start_cpi, true);

	release_event(msg);

	return ret;
}


/*
	The input map is represented by a tree-like structure
	the first input event is match against the tree's root
	if a match is found, the node is selected as the next root
 */
bool eval_input_event(event * base_msg)
{
	app_log << __PRETTY_FUNCTION__ << "\n";

	input_event * msg = static_cast<input_event *>(base_msg);

	assert(msg->ev);

	auto buffer = editor_buffer_check_id(msg->editor_buffer_id);
	if (buffer == INVALID_EDITOR_BUFFER_ID) {
		app_log << "no buffer id " << msg->byte_buffer_id << "\n";
		assert(0);
		return true;
	}

	if (msg->view_id == INVALID_EDITOR_VIEW_ID) {
		app_log << "no view/screen id " << msg->view_id << "\n";
		assert(0);
		return true;
	}


	editor_view * view = editor_view_get_internal_pointer(msg->view_id);

	eedit::editor_input_event_map * current_keymap = view->input.cur_event_map;
	std::vector< input_map_entry * > * cur_seq = view->input.last_keymap_entry;

	// reset keymap ?
	if (current_keymap == nullptr) {
		auto it = view->input.event_map.find("default"); // <- get_major_mode()->get_default_inputmap();
		if (it == view->input.event_map.end()) {
			assert(0);
			return false;
		}

		current_keymap = it->second; // type of it->second is eedit::editor_input_event_map *
	}

	// reset sequence ?
	if (cur_seq == nullptr) {
		cur_seq = &current_keymap->entries;
	}

	// read input event
	input_map_entry * match_found = nullptr;
	bool found = eval_input_event(msg->ev, cur_seq, &match_found);
	if (!found) {
		//
		app_log << "no match : reset user keymap context\n";
		view->input.cur_event_map      = nullptr;
		view->input.last_keymap_entry = nullptr;
		return false;
	}

	// select next keymap
	cur_seq = &(match_found->entries);
	view->input.last_keymap_entry = cur_seq;

	// exec action
	if (match_found->action == nullptr) {

		assert(cur_seq->size() != 0);

		// the sequence is valid but non terminal -> need more key
		app_log << " need more input(s)\n";
		return true;
	}

	// exec action
	assert(cur_seq->size() == 0);
	assert(msg->ev);

	// TODO: cache fn in action
	module_fn fn = editor_get_module_function(match_found->action->fn_name);
	if (fn) {
		app_log << "'" << match_found->action->fn_name << "' is defined\n";
		app_log << "BEGIN '" << match_found->action->fn_name << "'\n";
		fn(base_msg);
		app_log << "END '" << match_found->action->fn_name << "'\n";
	} else {
		app_log << "'" << match_found->action->fn_name << "' is not defined\n";
	}


	app_log << "match : reset user keymap context\n";
	view->input.cur_event_map      = nullptr;
	view->input.last_keymap_entry = nullptr;

	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool process_input_event(event * msg)
{
	eval_input_event(msg);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool process_application_event(core_context_t * core_ctx, application_event * msg)
{
	switch (msg->type) {
	case EDITOR_QUIT_APPLICATION_DEFAULT:
	case EDITOR_QUIT_APPLICATION_FORCED: {
		// here ?
		application_event * quit_ans = new application_event(EDITOR_QUIT_APPLICATION_DEFAULT);
		send_event_to_ui(msg, quit_ans);
		core_ctx->core_running = false;

	}
	break;

	default: {
		assert(0);
	}
	break;
	}

	return true;
}



} // ! namespace core

} // ! namespace eedit
