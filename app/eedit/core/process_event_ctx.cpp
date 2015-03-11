#include "byte_buffer.h"
#include "editor_buffer.h"
#include "editor_view.h"

#include "process_event_ctx.h"

#include "../core/core.hpp"

namespace eedit
{
namespace core
{

void reset_buffer_view_flags(editor_buffer_id_t editor_buffer_id,  byte_buffer_id_t bid, editor_view_id_t view)
{
	auto buffer = editor_buffer_check_id(editor_buffer_id);
	if (buffer == INVALID_EDITOR_BUFFER_ID) {
		// no buffer
		return;
	}

	if (view == INVALID_EDITOR_VIEW_ID) {
		// no view configured yet
		assert(0);
		return;
	}

	editor_buffer_set_changed_flag(editor_buffer_id, false);
	editor_view_set_region_changed_flag(view, false);
	editor_view_set_ui_need_refresh(view, false);


}


void set_ui_next_screen_start_cpi(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, editor_view_id_t view, const codepoint_info_s * cpi)
{
	auto buffer = editor_buffer_check_id(editor_buffer_id);
	if (buffer == INVALID_EDITOR_BUFFER_ID) {
		// no buffer
		return;
	}

	if (view == INVALID_EDITOR_VIEW_ID) {
		// no view configured yet
		assert(0);
		return;
	}

	// view->start_cpi_ptr = nullptr;

	if (cpi) {
		assert(cpi->used);
		editor_view_set_start_cpi(view, cpi);
		//view->start_cpi = *cpi;
	}
}

void set_ui_change_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, editor_view_id_t view)
{
	auto buffer = editor_buffer_check_id(editor_buffer_id);
	if (buffer == INVALID_EDITOR_BUFFER_ID) {
		// no buffer
		return;
	}

	if (view == INVALID_EDITOR_VIEW_ID) {
		// no view configured yet
		assert(0);
		return;
	}

	editor_view_set_ui_need_refresh(view, true);
}


bool ui_change_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, editor_view_id_t view)
{
	auto buffer = editor_buffer_check_id(editor_buffer_id);
	if (buffer == INVALID_EDITOR_BUFFER_ID) {
		// no buffer
		return false;
	}

	if (view == INVALID_EDITOR_VIEW_ID) {
		// no view configured yet
		assert(0);
		return false;
	}

	return editor_view_get_ui_need_refresh(view);
}

void set_ui_must_resync_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, editor_view_id_t view)
{
	auto buffer = editor_buffer_check_id(editor_buffer_id);
	if (buffer == INVALID_EDITOR_BUFFER_ID) {
		// no buffer
		return;
	}

	if (view == INVALID_EDITOR_VIEW_ID) {
		// no view configured yet
		return;
	}

	//editor_ctl(enum entity_kind, handle, enum action, param_union * arg);
	//editor_view_ctl(EDITOR_VIEW, view, SET_UI_CHANGE_FLAG, param_union * arg);

	editor_view_set_ui_must_resync(view, true);
}

bool ui_must_resync_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, editor_view_id_t view)
{
	auto buffer = editor_buffer_check_id(bid);
	if (buffer == INVALID_EDITOR_BUFFER_ID) {
		// no buffer
		return false;
	}

	if (view == INVALID_EDITOR_VIEW_ID) {
		// no view configured yet
		return false;
	}

	return editor_view_get_ui_must_resync(view);
}



void set_buffer_changed_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id)
{
	auto buffer = editor_buffer_check_id(bid);
	if (buffer == INVALID_EDITOR_BUFFER_ID) {
		// no buffer
		return;
	}

	editor_buffer_set_changed_flag(editor_buffer_id, true);
}

bool buffer_changed_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id)
{
	auto buffer = editor_buffer_check_id(bid);
	if (buffer == INVALID_EDITOR_BUFFER_ID) {
		// no buffer
		return false;
	}

	return  editor_buffer_get_changed_flag(editor_buffer_id);
}

}

}
