
#include "editor_message_handler.h"
#include "editor_view.h"

#include "../../api/src/editor_view_internal.h"
#include "../../core/core.hpp"


namespace eedit
{

namespace core
{


bool process_editor_message(core_context_t * core_ctx, struct editor_message_s * msg)
{
    app_logln(-1, "%s", __PRETTY_FUNCTION__);


    bool ret = false;

//FIXME:	editor_view_reset_flags(msg->editor_buffer_id, msg->view_id);
    reset_buffer_view_flags(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
    // refresh screen cache

    setup_screen_by_id(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, msg->screen_dim);

    auto buffer = msg->editor_buffer_id;
    auto view   = msg->view_id;


    // fix message
    msg->byte_buffer_id = editor_buffer_get_byte_buffer_id(buffer);


    if (buffer == INVALID_EDITOR_BUFFER_ID) {
        app_logln(-1, " no buffer_id defined");
    }

    switch (msg->type & EDITOR_EVENT_TYPE_FAMILY_MASK) {

    case EDITOR_APPLICATION_EVENT_FAMILY: {
        ret = process_application_message(core_ctx, msg);
    }
    break;

    case EDITOR_KEYBOARD_EVENT:
    case EDITOR_POINTER_BUTTON_EVENT_FAMILY: {
        check_input_message(msg);
        ret = process_input_event(msg);
    }
    break;

    case EDITOR_BUILD_LAYOUT_EVENT: {
        check_input_message(msg);
        ret = trigger_new_layout(msg);
    }
    break;

    case EDITOR_RPC_CALL_EVENT: {
        ret = eedit::core::process_rpc_call_event(msg);
    }
    break;

    default: {
        app_logln(-1, "core : receive : unhandled event type %p", (void *)msg->type);
    }
    break;
    }


    codepoint_info_s * start_cpi = nullptr;
    codepoint_info_s start_cpi_ref;

    if (view && editor_view_get_ui_must_resync(view)) {
        // set resync flag
        abort(); // MOVE AWAY : text-mode setup ....
        // resync_screen_layout(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, msg->screen_dim);
        //start_cpi = view->start_cpi_ptr; // temporary
    }


    if (view) {
        editor_view_get_start_cpi(view, &start_cpi_ref);
        if (start_cpi_ref.used) {
            start_cpi = &start_cpi_ref;
            app_logln(-1, "  start_cpi->offset      = %lu" ,start_cpi->offset);
            app_logln(-1, "  start_cpi->split_count = %u"  ,start_cpi->split_count);
            app_logln(-1, "  start_cpi->split_flag  = %u"  ,start_cpi->split_flag );

            assert(start_cpi_ref.cp_index != uint64_t(-1));
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
bool eval_input_event(struct editor_message_s * msg)
{
    app_logln(-1, "%s", __PRETTY_FUNCTION__);


    auto buffer = editor_buffer_check_id(msg->editor_buffer_id);
    if (buffer == INVALID_EDITOR_BUFFER_ID) {
        app_log(-1, "no buffer id %lu", msg->byte_buffer_id);
        assert(0);
        return true;
    }

    if (msg->view_id == INVALID_EDITOR_VIEW_ID) {
        app_log(-1, "no view/screen id %lu", msg->view_id);
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
    bool found = eval_input_event(&msg->input.ev, cur_seq, &match_found);
    if (!found) {
        //
        app_logln(-1, "no match : reset user keymap context");
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
        app_logln(-1, " need more input(s)");
        return true;
    }

    // exec action
    assert(cur_seq->size() == 0);

    // TODO: cache fn in action
    editor_message_handler_t fn = editor_get_message_handler(match_found->action->fn_name);
    if (fn) {
        app_logln(-1, "'%s' is defined", match_found->action->fn_name);
        app_logln(-1, "BEGIN '%s'", match_found->action->fn_name);
        fn(msg);
	app_logln(-1, "END '%s'", match_found->action->fn_name);
    } else {
        app_logln(-1, "'%s' is not defined", match_found->action->fn_name);
    }


    app_logln(-1, "match : reset user keymap context");
    view->input.cur_event_map      = nullptr;
    view->input.last_keymap_entry = nullptr;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool process_input_event(struct editor_message_s * msg)
{
    eval_input_event(msg);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool process_application_message(core_context_t * core_ctx, struct editor_message_s * msg)
{
    switch (msg->type) {
    case EDITOR_QUIT_APPLICATION_DEFAULT:
    case EDITOR_QUIT_APPLICATION_FORCED: {
        // here ?
        struct editor_message_s * quit_ans = editor_event_alloc();
        quit_ans->type = EDITOR_QUIT_APPLICATION_DEFAULT;
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
