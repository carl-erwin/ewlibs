#pragma once

#include <stddef.h>
#include <stdint.h>

#include "editor_types.h"


#include "byte_buffer.h"
#include "buffer_log.h"
#include "mark.h"
#include "screen.h"

#ifdef __cplusplus
extern "C" {
#endif


// root
uint64_t editor_number_of_byte_buffer(); // TODO: remove rpc


int      editor_get_byte_buffer_ids(uint64_t max_number_of_buffer, byte_buffer_id_t * ids);

editor_buffer_id_t editor_buffer_open(const char * file_name, const char * buffer_name);
editor_buffer_id_t editor_buffer_check_id(editor_buffer_id_t editor_buffer_id);

int editor_buffer_close(editor_buffer_id_t editor_buffer_id);


//
uint64_t editor_number_of_buffer(); // TODO: remove rpc
int      editor_get_buffers(uint64_t max_number_of_editor_buffer_ids, editor_buffer_id_t * editor_buffer_ids);

// TODO: bind a buffer to a byte_buffer
int      editor_buffer_set_buffer_id(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid);
byte_buffer_id_t editor_buffer_get_byte_buffer_id(editor_buffer_id_t editor_buffer_id);

// TODO: bind a buffer log to an editor buffer
int      editor_buffer_set_buffer_log_id(editor_buffer_id_t editor_buffer_id, buffer_log_id_t lid);

// buffer -> views
uint64_t editor_buffer_number_of_views(editor_buffer_id_t editor_buffer_id);

editor_view_id_t editor_buffer_check_view_id(editor_buffer_id_t editor_buffer_id, editor_view_id_t view);


int      editor_buffer_get_views(editor_buffer_id_t editor_buffer_id, editor_view_id_t * views, uint64_t max_number_of_view);
int	editor_buffer_add_view(editor_buffer_id_t editor_buffer_id, editor_view_id_t view, screen_dimension_t * dim);// implicit ?
int	editor_buffer_remove_view(editor_buffer_id_t editor_buffer_id, editor_view_id_t view);// implicit ?


// buffer -> marks
uint64_t editor_buffer_number_of_marks(editor_buffer_id_t editor_buffer_id);
int      editor_buffer_get_marks(editor_buffer_id_t editor_buffer_id, uint64_t max_number_of_view, mark_t * marks);


void editor_buffer_set_changed_flag(editor_buffer_id_t, bool);
bool editor_buffer_get_changed_flag(editor_buffer_id_t);


#ifdef __cplusplus
}
#endif
