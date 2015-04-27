#include <map>
#include <memory>
#include <algorithm>


#include "../../../core/core.hpp"// FIXME: C apis
#include "../../../core/text_layout.hpp"   // FIXME: C apis
#include "../../../core/process_event_ctx.h"

#include "editor_event_queue.h"
#include "editor_message_handler.h"
#include "editor_module.h"
#include "buffer_log.h"
#include "editor_screen.h"
#include "text_codec.h"
#include "editor_buffer.h"
#include "editor_view.h"

////////////////////////////////////////////////////////////////////////////////

const int32_t INVALID_CP    = -1;
const int32_t NO_MOVE       =  0;
const int32_t MOVE_FORWARD  =  1;
const int32_t MOVE_BACKWARD = -1;

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

enum operation_mask_e {
    EDITOR_OP_MARK_MOVE      = 1 << 0,
    EDITOR_OP_INSERT_AT_MARK = 1 << 1,
    EDITOR_OP_DELETE_AT_MARK = 1 << 2,
};

enum mark_move_direction_e {
    EDITOR_MARK_MOVE_NONE                  = NO_MOVE,
    EDITOR_MARK_MOVE_TO_NEXT_CODEPOINT     = MOVE_FORWARD,
    EDITOR_MARK_MOVE_TO_PREVIOUS_CODEPOINT = MOVE_BACKWARD,
    EDITOR_MARK_MOVE_TO_NEXT_LINE          = +2,
    EDITOR_MARK_MOVE_TO_PREVOIUS_LINE      = -2,
};

// split in buffer / view get marks
std::vector<mark_t> get_marks(struct editor_message_s * msg, mark_type_t mark_mask)
{
    auto ebid      = msg->editor_buffer_id;
    auto view      = msg->view_id;

    // get the moving marks
    uint64_t buff_moving_nmark = 0;
    if (mark_mask & MOVING_MARK) {
        buff_moving_nmark += editor_buffer_number_of_marks(ebid, MOVING_MARK);
    }

    uint64_t buff_fixed_nmark = 0;
    if (mark_mask & FIXED_MARK) {
        buff_fixed_nmark += editor_buffer_number_of_marks(ebid, FIXED_MARK);
    }

    uint64_t view_moving_nmark = 0;
    if (mark_mask & MOVING_MARK) {
        view_moving_nmark += editor_view_number_of_marks(view, MOVING_MARK);
    }
    uint64_t view_fixed_nmark = 0;
    if (mark_mask & FIXED_MARK) {
        view_fixed_nmark += editor_view_number_of_marks(view, FIXED_MARK);
    }

    // accumulate marks
    std::vector<mark_t> marks(buff_moving_nmark + buff_fixed_nmark + view_moving_nmark + view_fixed_nmark);

    size_t pos = 0;
    if (mark_mask & MOVING_MARK) {
        editor_buffer_get_marks(ebid, MOVING_MARK, buff_moving_nmark, &marks[pos]);
        pos += buff_moving_nmark;
    }
    if (mark_mask & FIXED_MARK) {
        editor_buffer_get_marks(ebid, FIXED_MARK, buff_fixed_nmark, &marks[pos]);
        pos += buff_fixed_nmark;
    }

    if (mark_mask & MOVING_MARK) {
        editor_view_get_marks(view, MOVING_MARK,   view_moving_nmark, &marks[pos]);
        pos += view_moving_nmark;
    }
    if (mark_mask & FIXED_MARK) {
        editor_view_get_marks(view, FIXED_MARK,   view_fixed_nmark, &marks[pos]);
        pos += view_fixed_nmark;
    }


    auto screen = get_previous_screen_by_id(view);
    // NB: sort in decreasing offset order
    const codepoint_info_t * cpi = nullptr;
    screen_get_first_cpinfo(screen, &cpi);
    if (cpi == nullptr) {
        return std::vector<mark_t>();
    }
    uint64_t min_offset = cpi->offset;

    cpi = nullptr;
    screen_get_last_cpinfo(screen, &cpi);
    if (cpi == nullptr) {
        return std::vector<mark_t>();
    }
    uint64_t max_offset = cpi->offset;

    // filter offscreen/onscreen
    std::vector<mark_t> marks_filtered(pos);

    for (auto & cur_mark : marks) {
        auto off = mark_get_offset(cur_mark);
        if ((mark_mask & OFFSCREEN_MARK) && ((off < min_offset) || (off > max_offset))) {
            marks_filtered.push_back(cur_mark);
        }

        if ((mark_mask & ONSCREEN_MARK) && ((off >= min_offset) && (off <= max_offset))) {
            marks_filtered.push_back(cur_mark);
        }
    }

    std::sort(marks.begin(), marks.end(), [](mark_t m1, mark_t m2) {
        return mark_get_offset(m1) > mark_get_offset(m2);
    });

    abort();
    return marks;
}

/*
  The operation are executed in this order:
  insert
  move
  delete

  delete is last because delete-left-char is implemented like this : move the marks to the left and then delete the codepoint(s)

 */
int mark_operation(struct editor_message_s * msg, int op_mask, int32_t codepoint, enum mark_move_direction_e move_type)
{
    // TODO: TEXT MODE CONTEXT { ebid, view, codec_id(view), codec_ctx(view) } ?

    // setup context
    auto ebid      = msg->editor_buffer_id;
    auto bid       = msg->byte_buffer_id;

    auto view      = msg->view_id;
    auto codec_id  = editor_view_get_codec_id(view);
    auto codec_ctx = editor_view_get_codec_ctx(view);

    // get the moving marks
    auto buff_nmark = editor_buffer_number_of_marks(ebid, MOVING_MARK);
    auto view_nmark = editor_view_number_of_marks(view, MOVING_MARK);

    // accumulate marks
    std::vector<mark_t> marks(buff_nmark + view_nmark);

    editor_buffer_get_marks(ebid, MOVING_MARK, buff_nmark, &marks[0]);
    editor_view_get_marks(view, MOVING_MARK,   view_nmark, &marks[buff_nmark]);

    // NB: sort in decreasing offset order
    std::sort(marks.begin(), marks.end(), [](mark_t m1, mark_t m2) {
        return mark_get_offset(m1) > mark_get_offset(m2);
    });

    // build text codec context :
    codec_io_ctx_s codec_io_ctx = {
        .editor_buffer_id = msg->editor_buffer_id,
        .bid              = msg->byte_buffer_id,
        .codec_id         = codec_id,
        .codec_ctx        = codec_ctx,
    };

    for (auto & cur_mark : marks) {

        // if (mark_get_type(cur_mark) == FIXED) continue;

        auto old_offset = mark_get_offset(cur_mark);

        // function ?
        if (op_mask & EDITOR_OP_INSERT_AT_MARK) {
            struct text_codec_io_s write_io {
                .offset = old_offset, .cp = codepoint, .size = 0
            };

            int ret = text_codec_write(&codec_io_ctx, &write_io, 1);
            if (ret <= 0) {
                // FIXME: editor_log(const char * s);
                continue;
            }

            // move all the marks after (> offset) this one of write_io.size
            // FIXME: the FIXED MARKS MUST BE MOVED !!

            // must add type = mark_get_type(m);
            for (auto it = &marks[0]; it < &cur_mark; it++) {
                auto new_off = mark_get_offset(*it) + write_io.size;
                mark_set_offset(*it, new_off);
            }
        }


        if (op_mask & EDITOR_OP_MARK_MOVE) {

            if ((old_offset == 0) && (move_type == EDITOR_MARK_MOVE_TO_PREVIOUS_CODEPOINT))
                continue;

            if (move_type != EDITOR_MARK_MOVE_NONE) {
                struct text_codec_io_s read_io {
                    .offset = old_offset, .cp = -1, .size = 0
                };
                int ret = text_codec_read(&codec_io_ctx, move_type, &read_io, 1);
                if (ret > 0) {
                    mark_set_offset(cur_mark, read_io.offset + ((move_type == EDITOR_MARK_MOVE_TO_NEXT_CODEPOINT) ? read_io.size : 0));
                } else {

                }
            }
        }

        if (op_mask & EDITOR_OP_DELETE_AT_MARK) {
            auto cur_offset = mark_get_offset(cur_mark);
            struct text_codec_io_s read_io {
                .offset = cur_offset, .cp = -1, .size = 0
            };
            int ret = text_codec_read(&codec_io_ctx, MOVE_FORWARD, &read_io, 1);
            if (ret <= 0) {
                continue;
            }

            uint8_t removed[8];
            size_t nb_rm = 0;
            byte_buffer_remove(bid, cur_offset, removed, read_io.size, &nb_rm);

            // move all the marks after (> offset) this one of -read_io.size
            // FIXME: the FIXED MARKS MUST BE MOVED !!
            // must add type = mark_get_type(m);
            for (auto it = &marks[0]; it < &cur_mark; it++) {
                auto new_off = mark_get_offset(*it) - read_io.size;
                mark_set_offset(*it, new_off);
            }
        }
    }

    // notify change
    set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
    return EDITOR_STATUS_OK;
}


int mark_move_backward(struct editor_message_s * msg)
{
    return mark_operation(msg, EDITOR_OP_MARK_MOVE, INVALID_CP, EDITOR_MARK_MOVE_TO_PREVIOUS_CODEPOINT);
}

////////////////////////////////////////////////////////////////////////////////

int mark_move_forward(struct editor_message_s * msg)
{
    return mark_operation(msg, EDITOR_OP_MARK_MOVE, INVALID_CP, EDITOR_MARK_MOVE_TO_NEXT_CODEPOINT);
}

////////////////////////////////////////////////////////////////////////////////

// TODO: add function to collect the marks
int mark_move_to_screen_line(struct editor_message_s * msg, enum mark_move_direction_e dir)
{
    //  a) sort moving marks in increasing offset order based on move-offscreen-marks flags (codec context)
    //
    //  b) cur_screen <- clone view->last_screen
    //  -) build the a "previous screen" starting at screen().fisrt_line().previous_line().resynced(-1).start_offset();
    //  -) build the a "next screen"     starting at screen().last_line().next_line().resynced(-1);.start_offset();.
    //
    //  -) foreach mark
    //  -) if mark not on cur_screen (rebuild {cur/prev/next}_screen  cur_mar.offset see (b))
    //  -) move  marks with  {cur/prev/next}_screen screen :
    //        if no previous/next codepoints at new(l,c) -> take last cp of target line // FIXME: explain behavior
    //  -)



    return EDITOR_STATUS_OK;
}

int mark_move_to_previous_screen_line(struct editor_message_s * msg)
{
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


int mark_move_to_next_screen_line(struct editor_message_s * msg)
{
    return EDITOR_STATUS_OK;
}


////////////////////////////////////////////////////////////////////////////////

int insert_codepoint_val(struct editor_message_s * msg, int32_t codepoint)
{
    mark_operation(msg, EDITOR_OP_INSERT_AT_MARK|EDITOR_OP_MARK_MOVE, codepoint, EDITOR_MARK_MOVE_TO_NEXT_CODEPOINT);
    return EDITOR_STATUS_OK;
}


int insert_codepoint(struct editor_message_s * msg)
{
    insert_codepoint_val(msg, msg->input.ev.start_value);
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////
int insert_newline(struct editor_message_s * _msg)
{
    insert_codepoint_val(_msg, (int32_t)'\n');
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int delete_left_char(struct editor_message_s * _msg)
{
    mark_operation(_msg, EDITOR_OP_MARK_MOVE|EDITOR_OP_DELETE_AT_MARK, INVALID_CP, EDITOR_MARK_MOVE_TO_PREVIOUS_CODEPOINT);
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int delete_right_char(struct editor_message_s * _msg)
{
    mark_operation(_msg, EDITOR_OP_DELETE_AT_MARK, INVALID_CP, EDITOR_MARK_MOVE_NONE);
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

/*
 * provide per mode context
 * this will allow amrk mode to keep track of the view's main mark
 * all clone operation depends on main mark
 *
 */
int mark_clone_and_move_left(struct editor_message_s * _msg)
{

    return EDITOR_STATUS_OK;
}

int mark_clone_and_move_right(struct editor_message_s * _msg)
{
    return EDITOR_STATUS_OK;
}

int mark_clone_and_move_up(struct editor_message_s * _msg)
{
    return EDITOR_STATUS_OK;
}

int mark_clone_and_move_down(struct editor_message_s * _msg)
{
    return EDITOR_STATUS_OK;
}

int mark_delete(struct editor_message_s * _msg)
{
    return EDITOR_STATUS_OK;
}

int mark_select_next(struct editor_message_s * _msg)
{
    return EDITOR_STATUS_OK;
}

int mark_select_previous(struct editor_message_s * _msg)
{
    return EDITOR_STATUS_OK;
}


////////////////////////////////////////////////////////////////////////////////

void mark_mode_register_modules_function()
{
    /* basic moves */
    editor_register_message_handler("left-char",                  mark_move_backward);
    editor_register_message_handler("right-char",                 mark_move_forward);

    editor_register_message_handler("previous-line",              mark_move_to_previous_screen_line);
    editor_register_message_handler("next-line",                  mark_move_to_next_screen_line);

    /* insert/delete */
    editor_register_message_handler("self-insert",                insert_codepoint);
    editor_register_message_handler("insert-newline",             insert_newline);
    editor_register_message_handler("delete-left-char",           delete_left_char);
    editor_register_message_handler("delete-right-char",          delete_right_char);
    /* clone operation */
    editor_register_message_handler("mark-clone-and-move-left",   mark_clone_and_move_left);
    editor_register_message_handler("mark-clone-and-move-right",  mark_clone_and_move_right);
    editor_register_message_handler("mark-clone-and-move-up",     mark_clone_and_move_up);
    editor_register_message_handler("mark-clone-and-move-down",   mark_clone_and_move_down);
    editor_register_message_handler("mark-delete",                mark_delete);
    editor_register_message_handler("mark-select-next",           mark_select_next);
    editor_register_message_handler("mark-select-previous",       mark_select_previous);
}


void mark_mode_unregister_modules_function()
{
#if 0
    /* basic moves */
    editor_unregister_module_function("left-char",                  mark_move_backward);
    editor_unregister_message_handler("right-char",                 mark_move_forward);
    /* insert/delete */
    editor_unregister_message_handler("self-insert",                insert_codepoint);
    editor_unregister_message_handler("insert-newline",             insert_newline);
    editor_unregister_message_handler("delete-left-char",           delete_left_char);
    editor_unregister_message_handler("delete-right-char",          delete_right_char);
    /* clone operation */
    editor_unregister_message_handler("mark-clone-and-move-left",   mark_clone_and_move_left);
    editor_unregister_message_handler("mark-clone-and-move-right",  mark_clone_and_move_right);
    editor_unregister_message_handler("mark-clone-and-move-up",     mark_clone_and_move_up);
    editor_unregister_message_handler("mark-clone-and-move-down",   mark_clone_and_move_down);
    editor_unregister_message_handler("mark-delete",                mark_delete);
    editor_unregister_message_handler("mark-select-next",           mark_select_next);
    editor_unregister_message_handler("mark-select-previous",       mark_select_previous);
#endif
}


/* TODO:

	mode_id_t editor_register_mode(const char * name); // std::map<std::string, mode_info_t{ id, mode_ops } >
	int editor_unregister_mode(mode_id_t); // std::map<std::string, mode_info_t{ id, mode_ops } >

        view -> config -> "core:mark-mode"/mode_id_t    ->
          editor_view_mode_setup(ebid, view, mode_id_t) ->
          mode_ctx_t * mode_context_new();
          mode_context_release(mode_ctx_t *);

          view_add_mode_ctx(view, mode_id, mode_ctx_t *)
          view_add_mode_ctx(view, mode_id, mode_ctx_t *)


	provide function to update the view from external modules :

	ex: if sync view on mark move

	editor_view_scroll(view, int scroll_n_lines); ~~ page_up page_down
*/

////////////////////////////////////////////////////////////////////////////////

// Module interface

extern "C"
SHOW_SYMBOL const char * module_name()
{
    return "mode/marks";
}


extern "C"
SHOW_SYMBOL const char * module_version()
{
    return "1.0.0";
}

extern "C"
SHOW_SYMBOL eedit_module_type_e  module_type()
{
    return MODULE_TYPE_EDITOR_MODE;
}

extern "C"
SHOW_SYMBOL const char * module_depends()
{
    return "";
}


extern "C"
SHOW_SYMBOL eedit_module_init_status_e  module_init()
{
    mark_mode_register_modules_function();
    return MODULE_INIT_OK;
}

extern "C"
SHOW_SYMBOL int  module_quit()
{
    mark_mode_unregister_modules_function();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
