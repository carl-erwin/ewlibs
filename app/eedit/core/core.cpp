#include <cstring> // memcmp
#include <thread>
#include <mutex>
#include <map>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <algorithm>
#include <functional>

#include <ew/core/time/time.hpp>

///
#include "editor_event_queue.h"
#include "editor_screen.h"
#include "editor_codec.h"
#include "editor_message_handler.h"
#include "editor_types.h"
#include "editor_buffer.h"

#include "../api/src/editor_view_internal.h"

#include "../application/application.hpp"
#include "../core/core.hpp"
#include "../core/log/log.hpp"
#include "../core/text_layout.hpp"
#include "../core/rpc/rpc.hpp"

#include "text_layout.hpp"



#include "../core/undo/undo.h"


//
#include "process_event_ctx.h"

#include "../api/include/buffer_log.h"
#include "../api/include/text_codec.h"


namespace eedit
{

namespace core
{

static core_context_t core_ctx; // here ?




////////////////////////////////////////////////////////////////////////////////


screen_t * get_new_screen(editor_view * view)
{
    screen_t * scr = nullptr;

    screen_alloc_with_dimension(&scr, __PRETTY_FUNCTION__, &view->screen_cache.dim);
    assert(scr);
    screen_set_start_offset(scr, view->screen_cache.start_offset);
    return scr;
}


////////////////////////////////////////////////////////////////////////////////

// compute the screen dimension according to editor buffer id / fonts
bool  setup_screen_by_id(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid,  editor_view_id_t view, screen_dimension_t & dim)
{
    app_log <<  __PRETTY_FUNCTION__ <<  " ---\n";

    static int debug = 1;

    if (editor_buffer_id == 0) {
        app_log <<  __PRETTY_FUNCTION__ <<  " skipped editor_buffer_id == 0\n";
        return false;
    }

    if (view == 0) {
        app_log <<  __PRETTY_FUNCTION__ <<  " skipped bid == 0\n";
        return false;
    }


    if (debug) {
        app_log <<  __PRETTY_FUNCTION__ <<  "\n";
        app_log <<  " editor_buffer_id  = " <<  editor_buffer_id <<  "\n";
        app_log <<  " bid   = " <<  bid <<  "\n";
        app_log <<  " view   = " << view <<  "\n";
        app_log <<  " dim.w = " <<  dim.w <<  ", ";
        app_log <<  " dim.h = " <<  dim.h <<  ", ";
        app_log <<  " dim.c = " <<  dim.c <<  ", ";
        app_log <<  " dim.l = " <<  dim.l <<  "\n";
    }

    auto buffer = editor_buffer_check_id(editor_buffer_id);
    if (!buffer) {
        app_log <<  " editor_buffer_check_id error\n";
        return false;
    }

    auto view2 = editor_buffer_check_view_id(editor_buffer_id, view);
    if (view2 != 0) {
        app_log <<  " view already added to editor buffer\n";

        app_log <<  " TODO : compare dimension -> flush cache\n";


        editor_view * real_view = editor_view_get_internal_pointer(view);
        assert(real_view);
        real_view->screen_cache.dim = dim;

        return true;
    }

    app_log <<  " allocating view\n";
    editor_buffer_add_view(editor_buffer_id, view, &dim);

    editor_view * real_view = editor_view_get_internal_pointer(view);

    // FIXME: use font space horizontal/vertical advance + inter-line to compute the maximum col/line to resize to
    // get_font[' ']->width()

// TODO: setup fonts here
    editor_font_t ftid = editor_view_get_font(view);
    // cast editor_font_t to ew::graphics::font();
    assert(ftid);
    ew::graphics::fonts::font * ft = (ew::graphics::fonts::font *)ftid; // HACK

    // TODO: compute_screen_dimensions(w, h, font_config);
    ew::graphics::fonts::font_glyph_info tmp_glyph_info;
    int32_t cp = ' ';
    auto bret = ft->get_codepoint_glyph_info(cp, tmp_glyph_info);
    if (bret == false) {
        assert(0);
        return false;
    }

    // FIXME: use this to compute ncurses screen dimension
    int32_t hadvance = tmp_glyph_info.hori_advance;
    int32_t vadvance = tmp_glyph_info.vert_advance;
    // border + cp height + border
    auto number_of_lines       = dim.h / (vadvance + 0 /* interline*/)  + ((dim.h % vadvance) != 0);
    auto number_of_cp_per_line = dim.w / hadvance + ((dim.w % hadvance) != 0);
    dim.l = std::max<int32_t>(number_of_lines, 1);
    dim.c = std::max<int32_t>(number_of_cp_per_line, 1);

    if (debug) {
        app_log <<  " normalized dim.c = " <<  dim.c <<  "\n";
        app_log <<  " normalized dim.l = " <<  dim.l <<  "\n";
    }


    if (debug) {
        app_log <<  " check cache\n";
    }

    real_view->screen_cache.dim = dim;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool push_event(struct editor_message_s * msg)
{
    if (core_ctx.core_running == false) {
        // do not allow event push while quitting
        return false;
    }

    // check_editor_event
    switch (msg->type & EDITOR_EVENT_TYPE_FAMILY_MASK) {

    case EDITOR_APPLICATION_EVENT_FAMILY: {
        break;

        case EDITOR_KEYBOARD_EVENT:
        case EDITOR_POINTER_BUTTON_EVENT_FAMILY: {
            check_input_msg(msg);
        }
        break;

        case EDITOR_BUILD_LAYOUT_EVENT: {
            app_log << __PRETTY_FUNCTION__ << " EDITOR_BUILD_LAYOUT_EVENT\n";
            check_input_msg(msg);
        }
        break;

        case EDITOR_RPC_CALL_EVENT: {
        }
        break;

        default: {
            app_log <<  "core : sending : unhandled event type " <<  (void *)msg->type << "\n";
            abort();
        }
        break;
    }
    }

    return  editor_event_queue_push(core_ctx.m_msg_queue, msg);
}

////////////////////////////////////////////////////////////////////////////////

// FIXME: move to proper file event.h
void send_event_to_ui(const struct editor_message_s * ev_in, struct editor_message_s * ev_out)
{
    static uint32_t id = 1;

    ev_out->id = id++;
    ev_out->src = ev_in->dst;
    ev_out->dst = ev_in->src;

    if (ev_out->dst.queue != nullptr) {
        // app_log << " core : push event("<< ev_out->id <<") : core -> ui @" << ew::core::time::get_ticks() << "\n";
        editor_event_queue_push(ev_out->dst.queue, ev_out);
    } else {
        editor_event_free(ev_out);
        // app_log << " send_event_to_ui : no destination queue\n";
    }
}

////////////////////////////////////////////////////////////////////////////////

// FIXME: move to proper file
void send_new_layout_event_to_ui(const struct editor_message_s * ev_in,
                                 screen_t * screen)
{
    auto msg           = editor_layout_event_new(EDITOR_LAYOUT_NOTIFICATION_EVENT);
    msg->layout.screen = screen; // the new screen

    send_event_to_ui(ev_in, msg);
}

////////////////////////////////////////////////////////////////////////////////

// use for_each
template <class T>
bool release_list(std::list<T> & list)
{
    app_log << __FUNCTION__ << " pod\n";
    list.clear();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

/*
 *) check limits
 *) save current screen // duplicate ? (probably)
 *) goto start off
 *) rewind
 *) resync
 *) build screens until start_offset is reached
 *) build screens until end_offset is reached
 *) restore screen
*/

////////////////////////////////////////////////////////////////////////////////

// move to buffer ?
struct selection_record_s selection_record;

////////////////////////////////////////////////////////////////////////////////

bool save_buffer(struct editor_message_s * msg)
{
    //auto buffer = get_buffer_by_id(msg->byte_buffer_id);

    app_log << " SAVING buffer...\n";
    uint32_t t0 = ew::core::time::get_ticks();
    assert(0);
    size_t sz = 0;
    byte_buffer_size( editor_buffer_get_byte_buffer_id(msg->editor_buffer_id), &sz);
    // buffer_write_to_disk(msg->byte_buffer_id, &sz);
//    buffer->txt_buffer()->save_buffer();
    uint32_t t1 = ew::core::time::get_ticks();
    app_log << " ok...\n";
    app_log << " buffer saved in " << (t1 - t0) << "ms\n";

    return true;
}


////////////////////////////////////////////////////////////////////////////////

int dump_buffer_log(struct editor_message_s * msg)
{
    buffer_log_dump(msg->byte_buffer_id);
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

bool release_event(struct editor_message_s * msg)
{
    delete msg;
    return true;
}


////////////////////////////////////////////////////////////////////////////////

// remove/rename this
bool build_screen_layout_from_event(struct editor_message_s * msg, const codepoint_info_s * start_cpi, screen_t * scr)
{
    // now the screen is opaque we must not use it
    auto buffer = editor_buffer_check_id(msg->editor_buffer_id);
    if (buffer == INVALID_EDITOR_BUFFER_ID)
        return false;

    // check dim
    if (msg->view_id == INVALID_EDITOR_VIEW_ID)
        return false;

    auto codec_id  = editor_view_get_codec_id(msg->view_id);
    auto codec_ctx = editor_view_get_codec_ctx(msg->view_id);
    struct codec_io_ctx_s io_ctx {
        msg->editor_buffer_id,
            msg->byte_buffer_id,
            codec_id,
            codec_ctx
    };

    bool ret = build_screen_layout(&io_ctx, msg->view_id, start_cpi, scr);
    if (ret == true) {
        // adjust editor_buffer ?
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

// FIXME: replace bool by enum : SEND_SCREEN_TO_UI
bool notify_buffer_changes(struct editor_message_s * msg, codepoint_info_s * start_cpi, bool send_screen)
{
    bool notify = false;

    if (ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id) ==  true) {
        notify = true;
    }

    if (buffer_changed_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id) ==  true) {
        notify = true;
    }


    if (notify == true) {

        editor_view * view = editor_view_get_internal_pointer(msg->view_id);
        screen_release(view->screen_cache.last_screen);
        view->screen_cache.last_screen = nullptr;

        /* this screen will be used by next events, screen moves,  etc */
        // allocate screen here base on view->screen_cache.dimension
        auto last_screen = get_new_screen(view);
        assert(last_screen);

        build_screen_layout_from_event(msg, start_cpi, last_screen);
        view->screen_cache.last_screen = last_screen;

        if (send_screen == true) {
            auto new_screen = screen_clone(last_screen);
            send_new_layout_event_to_ui(msg, new_screen);
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool process_build_layout_event(struct editor_message_s * msg)
{
    set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool check_input_msg(struct editor_message_s * msg)
{
    assert(msg->editor_buffer_id);
    assert(msg->view_id);
    assert(msg->screen_dim.w);
    assert(msg->screen_dim.h);
    assert(msg->screen_dim.c);
    assert(msg->screen_dim.l);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

int quit_editor(struct editor_message_s * msg)
{
    editor_message_s * quit_msg = editor_event_alloc();

    quit_msg->type = EDITOR_QUIT_APPLICATION_DEFAULT;
    quit_msg->src = msg->src;
    quit_msg->dst = msg->dst;

    process_application_event(&core_ctx, quit_msg);
    editor_event_free(quit_msg);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

void register_core_modules_function()
{
    //
    editor_register_message_handler("quit-editor",               quit_editor);

    // undo mode
    editor_register_message_handler("undo",                      buffer_undo);
    editor_register_message_handler("redo",                      buffer_redo);
    editor_register_message_handler("dump-buffer-log",           dump_buffer_log);

}

////////////////////////////////////////////////////////////////////////////////

void main(std::shared_ptr<application> app)
{
    core_ctx.m_msg_queue = editor_event_queue_new();
    app_log << "core_ctx.m_msg_queue @" << core_ctx.m_msg_queue << "\n";

    {
        std::lock_guard<std::mutex> lock(core_ctx.m_mtx);
        if (core_ctx.core_started == true) {
            app_log << "core already started !!!\n";
            return;
        }
        core_ctx.core_started = true;
        core_ctx.core_running = true;
        // FIXME: find a proper way to notify clients
    }

    // TODO: use .so modules
    // from config file -> load-module path/to/file.so ?
    register_core_modules_function();

    app_log << " FIXME: ad other lib*.so (mark/text)";
    // text_mode_register_modules_function(); // move away : config file
    // mark_mode_register_modules_function(); // move away : config file

    static size_t default_wait_time = 1000;
    size_t wait_time = default_wait_time;
    while (core_ctx.core_running == true) {
//		auto w0 = get_ticks();
        editor_event_queue_wait(core_ctx.m_msg_queue, wait_time);
//		auto w1 = get_ticks();
//		app_log << "["<<w1<<"] wait time (" << w1 - w0 << ")\n";

        auto nr = editor_event_queue_size(core_ctx.m_msg_queue);
        while (nr) {
            struct editor_message_s * msg = nullptr;
            auto t0 = ew::core::time::get_ticks();

            if (0) {
                app_log << "["<<t0<<"] queue size (" << editor_event_queue_size(core_ctx.m_msg_queue) << ")\n";
                app_log << "["<<t0<<"] nr events to process(" << nr << ")\n";
            }

            msg = editor_event_queue_get(core_ctx.m_msg_queue);
            assert(msg);
            process_event(&core_ctx, msg);
            auto t1 = ew::core::time::get_ticks();
            if (0) {
                app_log << "["<<t1<<"] time to process event = " << t1 - t0 << "\n";
            }

            // --nr;
            nr = editor_event_queue_size(core_ctx.m_msg_queue);
        }
    }

    // clear message queue -> use stl container interface to allow for each ...
    while (editor_event_queue_size(core_ctx.m_msg_queue)) {
        struct editor_message_s * msg = editor_event_queue_get(core_ctx.m_msg_queue);
        editor_event_free(msg);
    }

    // TODO: send quit to others process/threads and join them

    editor_event_queue_delete(core_ctx.m_msg_queue);
}

////////////////////////////////////////////////////////////////////////////////

} // ! namespace core

} // ! namespace eedit
