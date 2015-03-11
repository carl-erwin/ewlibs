#pragma once

#include "editor_types.h"
#include "codepoint_info.h"



namespace eedit
{
namespace core
{
// REMOVE ALL THIS
void reset_buffer_view_flags(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);

void set_ui_next_screen_start_cpi(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id, const codepoint_info_s * cpi);

void set_ui_change_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);
bool ui_change_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);

void set_ui_must_resync_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);
bool ui_must_resync_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);

void set_buffer_changed_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);
bool buffer_changed_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);



}

}
