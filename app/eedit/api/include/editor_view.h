#pragma once

#include "editor_export.h"
#include "editor_types.h"
#include "mark.h"
#include "editor_screen.h"

#ifdef __cplusplus
extern "C" {
#endif

int editor_view_bind(editor_view_id_t view, editor_buffer_id_t ebid); // will allocate internal pointers
int editor_view_close(editor_view_id_t view);


int editor_view_set_dimension(editor_view_id_t view, const screen_dimension_t * dim);

uint64_t editor_view_get_start_offset(editor_view_id_t view);
void     editor_view_set_start_offset(editor_view_id_t view, uint64_t offset); //

uint64_t editor_view_get_end_offset(editor_view_id_t view);
void     editor_view_set_end_offset(editor_view_id_t view, uint64_t offset);


// view -> marks
uint64_t editor_view_number_of_marks(editor_view_id_t view, mark_type_t type);
int      editor_view_get_marks(editor_view_id_t view, mark_type_t type, uint64_t max_number_of_marks, mark_t * marks);



// view -> codec
codec_id_t         editor_view_get_codec_id(editor_view_id_t  view);
codec_context_id_t editor_view_get_codec_ctx(editor_view_id_t view);

// view -> input_map
// TODO:


// view -> font
editor_font_t editor_view_get_font(editor_view_id_t view);


// view -> screen
size_t       editor_view_preallocate_screens(editor_view_id_t view_id, size_t n);
void         editor_view_release_preallocated_screens(editor_view_id_t view_id);

screen_t *   get_previous_screen_by_id(editor_view_id_t view_id);
screen_t *   editor_view_allocate_screen_by_id(editor_view_id_t view_id); // view screen pool
int          editor_view_release_screen_by_id(editor_view_id_t view_id, screen_t * scr); // view screen pool

/* scrool the view
 the unit depends on the main/codec for text is lines
   n=line(s)
        m=frame(s) in future version

 */
screen_t *         editor_view_scroll_n(editor_view_id_t screen_id, int64_t nb); // will call the configured layout modules for this screen/view_id


//
void editor_view_set_region_changed_flag(editor_view_id_t view, bool);
bool editor_view_get_region_changed_flag(editor_view_id_t view);

void editor_view_set_ui_need_refresh(editor_view_id_t view, bool);
bool editor_view_get_ui_need_refresh(editor_view_id_t view);

void editor_view_set_ui_must_resync(editor_view_id_t view, bool);
bool editor_view_get_ui_must_resync(editor_view_id_t view);


void editor_view_set_start_cpi(editor_view_id_t view, const codepoint_info_t * cpi);
void editor_view_get_start_cpi(editor_view_id_t view, codepoint_info_t * cpi);


#ifdef __cplusplus
}
#endif
