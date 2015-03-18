#include <map>
#include <memory>
#include <algorithm>

#include <ew/core/Time.hpp>

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

/*
 * FIXME: prepare multi cursor :
 * get mark list -> mark_id
 * iterate over mark_ids
 * mark_move_backward(mark_id)
 * move the screen ?
 *
 * TODO:
 *  We must not allow multiple marks on the same offset, or the insert/remove will go wrong :-)
 *  when marks are moved and read the same offset of another moving mark they must be  merged
 *  and the log must be updated to reflect the mark_merge offset
 *
 *  undo mark move
 *  register mark creation/merge/deletion in buffer log/view log
*/

bool mark_move(eedit::core::event * msg, int direction)
{
	// TODO: TEXT MODE CONTEXT { ebid, view, codec_id(view), codec_ctx(view) } ?

	// setup context
	auto ebid      = msg->editor_buffer_id;
	auto view      = msg->view_id;
	auto codec_id  = editor_view_get_codec_id(view);
	auto codec_ctx = editor_view_get_codec_ctx(view);

	// get the moving marks
	auto buff_nmark = editor_buffer_number_of_marks(ebid, MOVING_MARK);
	auto view_nmark = editor_view_number_of_marks(view, MOVING_MARK);


	// accumulate marks
	std::vector<mark_t> marks(buff_nmark + view_nmark);

	app_log << __PRETTY_FUNCTION__ << " buff_nmark = " << buff_nmark << "\n";
	app_log << __PRETTY_FUNCTION__ << " view_nmark = " << view_nmark << "\n";
	app_log << __PRETTY_FUNCTION__ << " marks.size() = " << marks.size() << "\n";

	editor_buffer_get_marks(ebid, MOVING_MARK, buff_nmark, &marks[0]);
	editor_view_get_marks(view, MOVING_MARK, view_nmark, &marks[buff_nmark]);

	// NB: sort in decreasing offset order
	std::sort(marks.begin(), marks.end(), [](mark_t m1, mark_t m2) { return mark_get_offset(m1) > mark_get_offset(m2); });

	// build text codec context :
	codec_io_ctx_s codec_io_ctx = {
		.editor_buffer_id = msg->editor_buffer_id,
		.bid              = msg->byte_buffer_id,
		.codec_id         = codec_id,
		.codec_ctx        = codec_ctx,
	};

	int count = 0;
	auto t0 = ew::core::time::get_ticks();
	for (auto cur_mark : marks) {

		auto old_offset = mark_get_offset(cur_mark);

		struct text_codec_io_s text_codec_io {
			mark_get_offset(cur_mark), // offset
					-1,        // cp
					0          // size
		};

		int ret = text_codec_read(&codec_io_ctx, direction, &text_codec_io, 1);
		if (ret > 0) {
			mark_set_offset(cur_mark, text_codec_io.offset + (direction > 0 ? text_codec_io.size : 0));
		}

		auto new_offset = mark_get_offset(cur_mark);
		app_log << __PRETTY_FUNCTION__ << " moved  mark(" << count << ")  " << old_offset << " --> " << new_offset << "\n";
		++count;
	}
	auto t1 = ew::core::time::get_ticks();

	app_log << __PRETTY_FUNCTION__ << " moved  " << marks.size() << " marks in " << t1 - t0 << " ms\n";

	// FIXME: the screen may change or not, call when there is a change
	set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
	return true;
}



bool mark_move_backward(eedit::core::event * msg)
{
	return mark_move(msg, -1);
}

////////////////////////////////////////////////////////////////////////////////

// mark_move_forward(buffer, mark_index)

bool mark_move_forward(eedit::core::event * msg)
{
	return mark_move(msg, 1);
}

////////////////////////////////////////////////////////////////////////////////


void mark_mode_register_modules_function()
{
	// TODO: cursor-mode cursor_mode.cpp
	editor_register_module_function("left-char",                 (module_fn)mark_move_backward);
	editor_register_module_function("right-char",                (module_fn)mark_move_forward);


}
