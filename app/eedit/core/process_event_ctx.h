#pragma once

#include "editor_export.h"
#include "editor_types.h"
#include "codepoint_info.h"

#ifdef __cplusplus
extern "C" {
#endif

EDITOR_EXPORT
void reset_buffer_view_flags(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);

EDITOR_EXPORT
void set_ui_next_screen_start_cpi(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id, const codepoint_info_s * cpi);

EDITOR_EXPORT
void set_ui_change_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);

EDITOR_EXPORT
bool ui_change_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);

EDITOR_EXPORT
void set_ui_must_resync_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);

EDITOR_EXPORT
bool ui_must_resync_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);

EDITOR_EXPORT
void set_buffer_changed_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);

EDITOR_EXPORT
bool buffer_changed_flag(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, uint64_t screen_id);


}
