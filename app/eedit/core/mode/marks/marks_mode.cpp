#include <map>

#include "../../../application/application.hpp"
#include "../../../core/core.hpp"
#include "../../../core/message_queue.hpp"
#include "../../../core/text_layout.hpp"
#include "../../../core/module/module.hpp"
#include "../../../core/rpc/rpc.hpp"

//
#include "../../../core/process_event_ctx.h"

#include "buffer_log.h"
#include "screen.h"
#include "text_codec.h"
#include "editor_buffer.h"
#include "editor_view.h"

using namespace eedit::core;
////////////////////////////////////////////////////////////////////////////////

// FIXME: prepare multi cursor :
// get mark list -> mark_id
// iterate over mark_ids
// mark_move_backward(mark_id)
// move the screen ?
bool mark_move_backward(eedit::core::event * msg)
{
	app_log << __PRETTY_FUNCTION__ << "\n";

#if 0
	auto buff_nmark = editor_buffer_number_of_marks(msg->editor_buffer_id);
	auto view_nmark = editor_view_number_of_marks(msg->view_id);

	app_log << __PRETTY_FUNCTION__ << " buff_nmark = " << buff_nmark << "\n";
	app_log << __PRETTY_FUNCTION__ << " view_nmark = " << view_nmark << "\n";

#endif

	// TODO: TEXT MODE CONTEXT ?
	auto view      = msg->view_id;
	auto codec_id  = editor_view_get_codec_id(view);
	auto codec_ctx = editor_view_get_codec_ctx(view);

	// auto screen = get_previous_screen_by_id(msg->view_id);

	// FIXME: use // buffer_id,screen_id,codec_id,
	// add cursor api

	mark_t it = nullptr; // cursor_mode_get_main_mark(msg->view_id);
	if (it == nullptr)
		return false;

	// build text codec context :
	codec_io_ctx_s codec_io_ctx = {
		.editor_buffer_id = msg->editor_buffer_id,
		.bid       = msg->byte_buffer_id,
		.codec_id  = codec_id,
		.codec_ctx = codec_ctx,
	};

	struct text_codec_io_s text_codec_io {
		mark_get_offset(it), // offset
				-1, // cp
				0 // size
	};

	// --mark
	// add direction read_forward/read_backward
	int ret = text_codec_reverse_read(&codec_io_ctx, &text_codec_io, 1);
	if (ret == 0) {
		mark_set_offset(it, text_codec_io.offset);
	}

	// FIXME: the screen may change or not, call when there is a change
	set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

// mark_move_forward(buffer, mark_index)

bool mark_move_forward(eedit::core::event * msg)
{
	app_log << __PRETTY_FUNCTION__ << "\n";

	// TODO: TEXT MODE CONTEXT ?
	auto view      = msg->view_id;
	auto codec_id  = editor_view_get_codec_id(view);
	auto codec_ctx = editor_view_get_codec_ctx(view);

	// auto screen = get_previous_screen_by_id(msg->view_id);

	// FIXME: use // buffer_id,screen_id,codec_id,
	// add cursor api

	mark_t it = nullptr; // cursor_mode_get_main_mark(msg->view_id);
	if (it == nullptr)
		return false;

	// build text codec context :
	struct codec_io_ctx_s text_codec_io_ctx {
		msg->editor_buffer_id,
		    msg->byte_buffer_id,
		    codec_id,
		    codec_ctx
	};
	struct text_codec_io_s text_codec_io {
		mark_get_offset(it),
				-1,
				0
	};

	// ++mark
	// add direction read_forward/read_backward
	int ret = text_codec_read(&text_codec_io_ctx, &text_codec_io, 1);
	if (ret == 0) {
		mark_set_offset(it, text_codec_io.offset + text_codec_io.size);
	}

	// FIXME: the screen may change or not, call when there is a change
	set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
	return true;

}

////////////////////////////////////////////////////////////////////////////////


void mark_mode_register_modules_function()
{
	// TODO: cursor-mode cursor_mode.cpp
	eedit_register_module_function("left-char",                 (module_fn)mark_move_backward);
	eedit_register_module_function("right-char",                (module_fn)mark_move_forward);


}
