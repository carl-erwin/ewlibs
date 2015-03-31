#include <map>
#include <memory>
#include <algorithm>


#include "../../../core/core.hpp"// FIXME: C apis

#include "editor_event_queue.h"

#include "../../../core/text_layout.hpp"   // FIXME: C apis

#include "../../../core/module/module.hpp" // FIXME: C apis

#include "../../../core/process_event_ctx.h"

#include "module.h"
#include "buffer_log.h"
#include "screen.h"
#include "text_codec.h"
#include "editor_buffer.h"
#include "editor_view.h"

using namespace eedit::core;
////////////////////////////////////////////////////////////////////////////////

const int32_t INVALID_CP    = -1;
const int32_t NO_MOVE       = 0;
const int32_t MOVE_FORWARD  = 1;
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

/*
  The operation are executed in this order:
  insert
  move
  delete

  delete is last because delete-left-char is implemented like this : move the marks to the left and then delete the codepoint(s)

 */
bool mark_operation(eedit::core::event * msg, int op_mask, int32_t codepoint, int move_direction)
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

            if ((old_offset == 0) && (move_direction < 0))
                continue;

            if (move_direction) {
                struct text_codec_io_s read_io {
                    .offset = old_offset, .cp = -1, .size = 0
                };
                int ret = text_codec_read(&codec_io_ctx, move_direction, &read_io, 1);
                if (ret > 0) {
                    mark_set_offset(cur_mark, read_io.offset + (move_direction > 0 ? read_io.size : 0));
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

    // FIXME: the screen may change or not, call when there is a change
    set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
    return true;
}


bool mark_move_backward(eedit::core::event * msg)
{
    return mark_operation(msg, EDITOR_OP_MARK_MOVE, INVALID_CP, MOVE_BACKWARD);
}

////////////////////////////////////////////////////////////////////////////////

bool mark_move_forward(eedit::core::event * msg)
{
    return mark_operation(msg, EDITOR_OP_MARK_MOVE, INVALID_CP, MOVE_FORWARD);
}

////////////////////////////////////////////////////////////////////////////////

bool insert_codepoint_val(eedit::core::event * _msg, int32_t codepoint)
{
    input_event * msg = static_cast<input_event *>(_msg);
    mark_operation(msg, EDITOR_OP_INSERT_AT_MARK|EDITOR_OP_MARK_MOVE, codepoint, MOVE_FORWARD);
    return true;
}


bool insert_codepoint(eedit::core::event * _msg)
{
    input_event * msg = static_cast<input_event *>(_msg);
    insert_codepoint_val(_msg, msg->ev->start_value);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool insert_newline(eedit::core::event * _msg)
{
    insert_codepoint_val(_msg, (s32)'\n');
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool delete_left_char(eedit::core::event * _msg)
{
    mark_operation(_msg, EDITOR_OP_MARK_MOVE|EDITOR_OP_DELETE_AT_MARK, INVALID_CP, MOVE_BACKWARD);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool delete_right_char(eedit::core::event * _msg)
{
    mark_operation(_msg, EDITOR_OP_DELETE_AT_MARK, INVALID_CP, NO_MOVE);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

/*
 * provide per mode context
 * this will allow amrk mode to keep track of the view's main mark
 * all clone operation depends on main mark
 *
 */
bool mark_clone_and_move_left(eedit::core::event * _msg)
{

    return true;
}

bool mark_clone_and_move_right(eedit::core::event * _msg)
{
    return true;
}

bool mark_clone_and_move_up(eedit::core::event * _msg)
{
    return true;
}

bool mark_clone_and_move_down(eedit::core::event * _msg)
{
    return true;
}

bool mark_delete(eedit::core::event * _msg)
{
    return true;
}

bool mark_select_next(eedit::core::event * _msg)
{
    return true;
}

bool mark_select_previous(eedit::core::event * _msg)
{
    return true;
}


////////////////////////////////////////////////////////////////////////////////

void mark_mode_register_modules_function()
{
    /* basic moves */
    editor_register_module_function("left-char",                  (module_fn)mark_move_backward);
    editor_register_module_function("right-char",                 (module_fn)mark_move_forward);
    /* insert/delete */
    editor_register_module_function("self-insert",                (module_fn)insert_codepoint);
    editor_register_module_function("insert-newline",             (module_fn)insert_newline);
    editor_register_module_function("delete-left-char",           (module_fn)delete_left_char);
    editor_register_module_function("delete-right-char",          (module_fn)delete_right_char);
    /* clone operation */
    editor_register_module_function("mark-clone-and-move-left",   (module_fn)mark_clone_and_move_left);
    editor_register_module_function("mark-clone-and-move-right",  (module_fn)mark_clone_and_move_right);
    editor_register_module_function("mark-clone-and-move-up",     (module_fn)mark_clone_and_move_up);
    editor_register_module_function("mark-clone-and-move-down",   (module_fn)mark_clone_and_move_down);
    editor_register_module_function("mark-delete",                (module_fn)mark_delete);
    editor_register_module_function("mark-select-next",           (module_fn)mark_select_next);
    editor_register_module_function("mark-select-previous",       (module_fn)mark_select_previous);
}


void mark_mode_unregister_modules_function()
{
#if 0
    /* basic moves */
    editor_unregister_module_function("left-char",                  (module_fn)mark_move_backward);
    editor_register_module_function("right-char",                 (module_fn)mark_move_forward);
    /* insert/delete */
    editor_register_module_function("self-insert",                (module_fn)insert_codepoint);
    editor_register_module_function("insert-newline",             (module_fn)insert_newline);
    editor_register_module_function("delete-left-char",           (module_fn)delete_left_char);
    editor_register_module_function("delete-right-char",          (module_fn)delete_right_char);
    /* clone operation */
    editor_register_module_function("mark-clone-and-move-left",   (module_fn)mark_clone_and_move_left);
    editor_register_module_function("mark-clone-and-move-right",  (module_fn)mark_clone_and_move_right);
    editor_register_module_function("mark-clone-and-move-up",     (module_fn)mark_clone_and_move_up);
    editor_register_module_function("mark-clone-and-move-down",   (module_fn)mark_clone_and_move_down);
    editor_register_module_function("mark-delete",                (module_fn)mark_delete);
    editor_register_module_function("mark-select-next",           (module_fn)mark_select_next);
    editor_register_module_function("mark-select-previous",       (module_fn)mark_select_previous);
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
