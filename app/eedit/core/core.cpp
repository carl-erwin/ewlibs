#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <algorithm>
#include <functional>


//
#include <ew/core/object/object_locker.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/mutex_locker.hpp>
#include <ew/core/threading/condition_variable.hpp>

#include <ew/core/time/time.hpp>

///
#include "../application/application.hpp"
#include "../core/core.hpp"
#include "../core/message_queue.hpp"

#include "../api/include/screen.h"
#include "../api/include/codec.h"

#include "../core/text_layout.hpp"
#include "../core/module/module.hpp"
#include "../core/rpc/rpc.hpp"

#include "editor_types.h"
#include "editor_buffer.h"
#include "codec.h"


#include "../core/undo/undo.h"


//
#include "process_event_ctx.h"

#include "../api/include/buffer_log.h"
#include "../api/include/text_codec.h"

#include "../core/mode/text/text_mode.h"
#include "../core/mode/marks/marks_mode.h"



namespace eedit
{

namespace core
{

static core_context_t core_ctx; // here ?


////////////////////////////////////////////////////////////////////////////////

std::map<editor_view_id_t, screen_cache *> screen_id_map;

////////////////////////////////////////////////////////////////////////////////


screen_cache * get_screen_cache(u64 id)
{
	auto ret = screen_id_map.find(id);
	if (ret->first != id) {
		assert(0);
		return nullptr;
	}

	screen_cache * cache = ret->second;
	return cache;
}


////////////////////////////////////////////////////////////////////////////////

void set_last_screen(u64 id, screen_t * scr)
{
	screen_cache * cache = get_screen_cache(id);

	assert((scr == nullptr) || (cache->last_screen != scr));
	cache->last_screen       = scr;
	if (scr) {
		const codepoint_info_s * first_cpinfo;
		screen_get_first_cpinfo(scr, &first_cpinfo);
		cache->start_offset  = first_cpinfo->offset;
	}
}

////////////////////////////////////////////////////////////////////////////////

screen_t * get_last_screen(u64 id)
{
	screen_cache * cache = get_screen_cache(id);
	assert(cache);
	return cache->last_screen;
}

////////////////////////////////////////////////////////////////////////////////

screen_t * get_previous_screen_by_id(u64 id)
{
	return get_last_screen(id);
}

////////////////////////////////////////////////////////////////////////////////

screen_t * get_new_screen_by_id(u64 screen_id)
{
	auto cache = get_screen_cache(screen_id);
	if (!cache) {
		assert(0);
		return nullptr;
	}

	screen_t * scr = nullptr;

	screen_alloc_with_dimension(&scr, __PRETTY_FUNCTION__, &cache->dim);

	assert(scr);

	screen_set_start_offset(scr, cache->start_offset);

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
		app_log <<  " view   = " <<  view <<  "\n";
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
		return true;
	}

	app_log <<  " allocating view\n";
	editor_buffer_add_view(editor_buffer_id, view, &dim);

	// FIXME: use font space horizontal/vertical advance + inter-line to compute the maximum col/line to resize to
	// get_font[' ']->width()

// TODO: setup fonts here
	editor_font_t ftid = editor_view_get_font(view);
	// cast editor_font_t to ew::graphics::font();
	assert(ftid);
	ew::graphics::fonts::font * ft = (ew::graphics::fonts::font *)ftid; // HACK

	// TODO: compute_screen_dimensions(w, h, font_config);
	ew::graphics::fonts::font_glyph_info tmp_glyph_info;
	s32 cp = ' ';
	auto bret = ft->get_codepoint_glyph_info(cp, tmp_glyph_info);
	if (bret == false) {
		assert(0);
		return false;
	}

	// FIXME: use this to compute ncurses screen dimension
	s32 hadvance = tmp_glyph_info.hori_advance;
	s32 vadvance = tmp_glyph_info.vert_advance;
	// border + cp height + border
	auto number_of_lines       = dim.h / (vadvance + 0 /* interline*/)  + ((dim.h % vadvance) != 0);
	auto number_of_cp_per_line = dim.w / hadvance + ((dim.w % hadvance) != 0);
	dim.l = std::max<s32>(number_of_lines, 1);
	dim.c = std::max<s32>(number_of_cp_per_line, 1);

	if (debug) {
		app_log <<  " normalized dim.c = " <<  dim.c <<  "\n";
		app_log <<  " normalized dim.l = " <<  dim.l <<  "\n";
	}


	if (debug) {
		app_log <<  " check cache\n";
	}

	screen_cache * cache;
	if (!screen_id_map[ view ]) {
		app_log <<  " allocating screen cache for view(" << view << ")\n";


		cache = new screen_cache;
		screen_id_map[ view ] = cache;
		assert(cache->last_screen == nullptr);

	} else {
		app_log <<  " reusing screen cache for view(" << view << ")\n";

		cache = screen_id_map[ view ];
	}

	// FIXME: compare dimension

	cache->dim = dim;

	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool push_event(eedit::core::event * msg)
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

	return  core_ctx.m_msg_queue.push(msg);
}

////////////////////////////////////////////////////////////////////////////////

// FIXME: move to proper file event.h
void send_event_to_ui(const eedit::core::event * ev_in, eedit::core::event * ev_out)
{
	static u32 id = 1;

	ev_out->id = id++;
	ev_out->src = ev_in->dst;
	ev_out->dst = ev_in->src;

	if (ev_out->dst.queue != nullptr) {
		// app_log << " core : push event("<< ev_out->id <<") : core -> ui @" << ew::core::time::get_ticks() << "\n";
		ev_out->dst.queue->push(ev_out);
	} else {
		delete ev_out;
		// app_log << " send_event_to_ui : no destination queue\n";
	}
}

////////////////////////////////////////////////////////////////////////////////

// FIXME: move to proper file
void send_new_layout_event_to_ui(const eedit::core::event * ev_in,
				 screen_t * screen)
{
	auto msg           = new eedit::core::layout_event(EDITOR_LAYOUT_NOTIFICATION_EVENT);
	msg->screen        = screen; // the new screen

	send_event_to_ui(ev_in, msg);
}

////////////////////////////////////////////////////////////////////////////////

// FIXME: move to proper file
bool  build_screen_layout(struct codec_io_ctx_s * io_ctx, u64 screen_id, const codepoint_info_s *start_cpi, screen_t *scr);

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

bool save_buffer(event * msg)
{
	//auto buffer = get_buffer_by_id(msg->byte_buffer_id);

	app_log << " SAVING buffer...\n";
	u32 t0 = ew::core::time::get_ticks();
	assert(0);
	size_t sz = 0;
	byte_buffer_size( editor_buffer_get_byte_buffer_id(msg->editor_buffer_id), &sz);
	// buffer_write_to_disk(msg->byte_buffer_id, &sz);
//    buffer->txt_buffer()->save_buffer();
	u32 t1 = ew::core::time::get_ticks();
	app_log << " ok...\n";
	app_log << " buffer saved in " << (t1 - t0) << "ms\n";

	return true;
}


////////////////////////////////////////////////////////////////////////////////

bool dump_buffer_log(event * msg)
{
	buffer_log_dump(msg->byte_buffer_id);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool release_event(event * msg)
{
	delete msg;
	return true;
}


////////////////////////////////////////////////////////////////////////////////

// remove/rename this
bool build_screen_layout(event * msg, const codepoint_info_s * start_cpi, screen_t * scr)
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
bool notify_buffer_changes(event * msg, codepoint_info_s * start_cpi, bool send_screen)
{
	bool notify = false;

	if (ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id) ==  true) {
		notify = true;
	}

	if (buffer_changed_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id) ==  true) {
		notify = true;
	}


	if (notify == true) {

		auto old_scr = get_last_screen(msg->view_id);
		screen_release(old_scr);
		set_last_screen(msg->view_id, nullptr);

		/* this screen will be used by next events, screen moves,  etc */
		auto last_screen = get_new_screen_by_id(msg->view_id);
		assert(last_screen);

		build_screen_layout(msg, start_cpi, last_screen);
		set_last_screen(msg->view_id, last_screen);

		if (send_screen == true) {
			auto new_screen = screen_clone(last_screen);
			send_new_layout_event_to_ui(msg, new_screen);
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool process_build_layout_event(eedit::core::layout_event * msg)
{
	set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool check_input_msg(event* msg)
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

bool quit_editor(event * msg)
{
	application_event quit_msg(EDITOR_QUIT_APPLICATION_DEFAULT);
	quit_msg.src = msg->src;
	quit_msg.dst = msg->dst;

	process_application_event(&core_ctx, &quit_msg);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void register_core_modules_function()
{
	//
	eedit_register_module_function("quit-editor",               (module_fn)quit_editor);

	// undo mode
	eedit_register_module_function("undo",                      (module_fn)buffer_undo);
	eedit_register_module_function("redo",                      (module_fn)buffer_redo);
	eedit_register_module_function("dump-buffer-log",           (module_fn)dump_buffer_log);

}

////////////////////////////////////////////////////////////////////////////////

void main(std::shared_ptr<application> app)
{
	{
		std::lock_guard<std::mutex> lock(core_ctx.m_mtx);
		if (core_ctx.core_started == true) {
			app_log << "core already started !!!\n";
			return;
		}
		core_ctx.core_started = true;
		core_ctx.core_running = true;
	}


	// TODO: use .so modules
	// from config file -> load-module path/to/file.so ?
	register_core_modules_function();
	text_mode_register_modules_function(); // move away : config file
	mark_mode_register_modules_function(); // move away : config file

	static size_t default_wait_time = 1000;
	size_t wait_time = default_wait_time;
	while (core_ctx.core_running == true) {
//		auto w0 = get_ticks();
		core_ctx.m_msg_queue.wait(wait_time);
//		auto w1 = get_ticks();
//		app_log << "["<<w1<<"] wait time (" << w1 - w0 << ")\n";

		auto nr = core_ctx.m_msg_queue.size();
		while (nr) {
			event * msg = nullptr;
			auto t0 = ew::core::time::get_ticks();

			if (0) {
				app_log << "["<<t0<<"] queue size (" << core_ctx.m_msg_queue.size() << ")\n";
				app_log << "["<<t0<<"] nr events to process(" << nr << ")\n";
			}

			core_ctx.m_msg_queue.get(msg);
			assert(msg);
			process_event(&core_ctx, msg);
			auto t1 = ew::core::time::get_ticks();
			if (0) {
				app_log << "["<<t1<<"] time to process event = " << t1 - t0 << "\n";
			}

			// --nr;
			nr = core_ctx.m_msg_queue.size();
		}
	}

	// clear message queue -> use stl container interface to allow for each ...
	while (core_ctx.m_msg_queue.size()) {
		event * msg = nullptr;
		core_ctx.m_msg_queue.get(msg);
	}

	// release res
	for (auto it : screen_id_map) {
		delete it.second;
	}

	// TODO: send quit to others process/threads and join them
}

////////////////////////////////////////////////////////////////////////////////

} // ! namespace core

} // ! namespace eedit
