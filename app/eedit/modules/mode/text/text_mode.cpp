#include <vector>
#include <map>

#include "editor_module.h"
#include "editor_event_queue.h"
#include "editor_message_handler.h"


#include "application/application.hpp"
#include "core.hpp"

#include "text_layout.hpp"
#include "rpc/rpc.hpp"

//
#include "event/process_event_ctx.h"

#include "buffer_log.h"
#include "editor_screen.h"
#include "text_codec.h"


/*
    slowly replace text_buffer::iterators by mark api
    after that , remove text buffer class

    class editor_object_t {
    pod: byte_array
    editor_event_t
    editor_codepoint_info_t
    pixel_t + format + coords
    font_t
    sound_t + format
    picture_t + format
    }

    "C" editor_register_codec(name, * out_id);
    "C" editor_unregister_codec(name, * out_id);

    "C" buffer_set_text_codec(buffer_id, codec_id) : more general key/val ?

    struct text_codec_cp_info_s {
      uint64_t offset;
      int32_t  cp;
      uint32_t size;
    };

    "C" text_codec_decode(codec_id, buffer_id, nb_cp_to_decode, start_offset, text_codec_cp_info_s out_cp[], const size_t out_cp_max, size_t * out_max)
    "C" text_codec_encode(codec_id, buffer_id, nb_cp_to_encode, start_offset, u8 container, container_size, offset, size_t codepoint_size); // allow 8/16/32

// picture/sound ?
    "C" video_codec_decode(codec_id, buffer_id, nb_cp_to_decode, start_offset, video_codec_cp_info_s out_cp[], const size_t out_cp_max, size_t * out_max)
    "C" video_codec_encode(codec_id, buffer_id, nb_cp_to_encode, start_offset, u8 container, container_size, offset, size_t codepoint_size); // allow 8/16/32

    "C" mark_create(buffer_id, mark_id * out)
    "C" mark_destroy(buffer_id, mark_id * out)
    "C" mark_move_forward(buffer_id, mark_id * out)
    "C" mark_move_backward(buffer_id, mark_id * out)
    "C" mark_move_to(buffer_id, mark_id * out, uint64_t offset)
    "C" mark_get_position(buffer_id, mark_id * out, uint64_t * offset)

    // see gstreamer

    codec : (from offset consume bytes -> new_offset)

    with {buffer_id screen_id} : buffer |  codec_id | int32_t cp/pictures/bytes | fold


*/

/*
  TODO:

  split CODE

   prepare
     editing_mode:

     the key/drag/paste/mouse/ events -> core_event_queue -> editing_modes pipeline


      must add drag/mouse/paste event in configuration:
      move keymap -> event_map

      editor_register_message_handler("xxx", xxx) -> eedit_register_editing_function("xxx", xxx)

      eedit_register_editing_function("cursor-motions-fn-name", xxx)
      eedit_register_editing_function("cursor-motions-fn-name", xxx)

     build_layout_mode:

      eedit_register_build_layout_function("text-mode",            build_text_mode_layout)
      eedit_register_build_layout_function("tab-expansion",        expand_tabs)
      eedit_register_build_layout_function("default-build-screen", default_build_screen_layout)

     undo_mode



screen list builder
phlX -> physical lineX
sclX -> screen   lineX

                         SCL(10)
example: the screen is { SCL     * 8C }   13*8 cp (mono fonts)

            [ 1 ][ 2 ][ 3 ][ 4 ][ 5 ][ 6 ][ 7 ][ 8 ]
8<--------------------------------------------------(0)
scl01 |phl1 [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl02 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl03 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl04 |     [xxx][xxx][xxx][xxx][\n ]
scl05 |phl  [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl06 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl07 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl08 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl09 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl10 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
8<--------------------------------------------------(1)
scl10 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl11 |     [xxx][xxx][xxx][xex][xxx][xxx][xxx][xxx]
scl12 |     [xxx][xxx][xxx][xxx][\n ]
scl13 |phl  [xxx][xxx][xxx][xxx][\n ]
scl13 |phl  [xxx][xxx][xxx][xxx][\n ]
scl14 |phl  [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl15 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl16 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl17 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl18 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
8<--------------------------------------------------(2)
scl18 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl19 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl20 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl21 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl22 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl23 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]  <--next start ?
scl24 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl25 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl26 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl27 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
8<--------------------------------------------------(3)
scl27 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl28 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl29 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl30 |     [xxx][xxx][xxx][xxx][\n ]
scl31 |phl  [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl32 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]   <-> is next last line : scl32 - scL(10) : local(5),  abs = (screnIndex(3) - 1 ) * SCL(10) + local(5) = (30 + 5) = 35
scl33 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]       abs - SCL(10) = 25
scl34 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl35 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
scl36 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
8<--------------------------------------------------
scl36 |     [xxx][xxx][xxx][xxx][xxx][xxx][xxx][xxx]
...
etc..

*/

////////////////////////////////////////////////////////////////////////////////

// page down until offset is on screen

// move previous screen ?

int  resync_screen_layout(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, editor_view_id_t view, screen_dimension_t & dim)
{
    auto screen = get_previous_screen_by_id(view);

    // duplicate previous screen ?

    // save offset
    uint64_t start_offset     = screen_get_start_offset(screen);
    uint64_t new_start_offset = start_offset;

    // go to beginning of line
    // get_beginning_of_line(bid, cid, off_in||mark, &off_out); wrapper -> codec_ctl

    // codec_ctl(bid, cid, start_offset, GET_BEGINNING_OF_LINE, &new_start_offset);

    // page down until offset is on screen
    codepoint_info_s start_cpi;
    codepoint_info_reset(&start_cpi);
    start_cpi.offset = start_offset;
    start_cpi.used = true;


    codec_io_ctx_s io_ctx {editor_buffer_id, bid, editor_view_get_codec_id(view), 0};

    while (true) {
        build_screen_layout(&io_ctx, view, &start_cpi, screen); // FIXME: send the last screen instead of rebuild :-)

        const screen_line_t * l;
        size_t scr_line_index;
        size_t scr_col_index;

        auto found = screen_get_line_by_offset(screen, new_start_offset, &l, &scr_line_index, &scr_col_index); // return EQ , GT, LT ?
        if (found == true) {
            break;
        }
    }

    set_ui_change_flag(editor_buffer_id, bid, view); // should be implicit when building new_screen ?
    return EDITOR_STATUS_OK;
}

/*
   RENAME : resync offset
   -)


*/
int rewind_offset(editor_buffer_id_t ed_buffer,
                  editor_view_id_t ed_view,
                  const uint32_t screen_max_cp, const uint32_t hints, uint64_t near_offset, uint64_t * resynced_offset)
{
    uint64_t rewind_off = near_offset;


    *resynced_offset = editor_view_get_start_offset(ed_view);


    // FIXME:   define editor_log() like printf // app_log << "hints & rewind_screen\n";

    if (screen_max_cp >= near_offset) {
        rewind_off = 0;
    } else {
        rewind_off = near_offset - screen_max_cp;
    }

    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " : set rewind_off to " << rewind_off << "\n";

    editor_view_set_start_offset(ed_view, rewind_off);

    return 0;
}


/*
 FIXME: rename in resync_offset_backward
*/
int rewind_and_resync_offset(editor_buffer_id_t ed_buffer,
                             editor_view_id_t ed_view,
                             const uint32_t screen_max_cp, const uint32_t hints, uint64_t near_offset, uint64_t * resynced_offset)
{
    uint64_t rewind_off = near_offset;


    *resynced_offset = editor_view_get_start_offset(ed_view);

    if (hints & rewind_screen) {

        // FIXME:   define editor_log() like printf // app_log << "hints & rewind_screen\n";

        if (screen_max_cp >= near_offset) {
            rewind_off = 0;
        } else {
            // rewind_off = near_offset - screen_max_cp
            rewind_off = near_offset; //  - screen_max_cp;
        }

        // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " : set rewind_off to " << rewind_off << "\n";

        editor_view_set_start_offset(ed_view, rewind_off);
    }

    if (hints & resync_screen) {
        // FIXME:   define editor_log() like printf // app_log << "hints & resync_screen\n";

        // TODO: need resync here: but we don't know the used codec
        text_codec_io_ctx_s io_ctx {
            {
                ed_buffer,
                editor_buffer_get_byte_buffer_id(ed_buffer),
                editor_view_get_codec_id(ed_view),
                0 /* codex ctx */
            },
            0,
            0
        };
        int ret = text_codec_sync_line(&io_ctx, near_offset, -1, resynced_offset);
        if (ret == -1) {

        } else {

        }


        // must have view_buffer->codec()->resync(offset, direction)
        // page-up/down must be brought by the text-mode

        // FIXME:   define editor_log() like printf // app_log << "FIXME FIXME FIXME !!!!!!!!!!!!\n";

        //editor_mark_to_beginnig_of_line(bid, ed_buffer->rdr_begin())
        // ed_view->rdr_begin()->toBeginningOfLine();
        // mark_set_offset(ed_view->rdr_begin(), resynced_offset);
    }



    *resynced_offset = editor_view_get_start_offset( ed_view );
    assert(*resynced_offset <= rewind_off);
    assert(*resynced_offset <= near_offset);

    return EDITOR_STATUS_OK;
}


/* TODO: use dichotomic search */
int screen_list_find_offset(const std::list<screen_t *> & scr_list,
                            const uint64_t target_offset,
                            size_t & screen_index,
                            size_t & abs_line_index,
                            size_t & line_index,
                            const screen_line_t ** l,
                            size_t & col_index)
{
    bool found = false;
    screen_index = 0;
    size_t abs_index = 0;
    auto it     = scr_list.begin();
    auto it_end = scr_list.end();
    while (it != it_end) {

        screen_t * scr = *it;
        found = screen_get_line_by_offset(scr, target_offset, l, &line_index, &col_index);
        if (found)
            break;
        ++it;
        ++screen_index;
        abs_index += screen_get_number_of_used_lines(scr);
    }

    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " abs_index  " << abs_index  << "\n";
    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " line_index " << line_index << "\n";
    abs_line_index = abs_index + line_index;
    return found;
}


// TODO: move to common, view.h ?
int view_clip_offset(
    editor_buffer_id_t ed_buffer,
    editor_view_id_t ed_view, uint64_t & offset)
{
    size_t buffer_sz;
    byte_buffer_size( editor_buffer_get_byte_buffer_id(ed_buffer), &buffer_sz);
    if (offset > buffer_sz) {
        offset = buffer_sz;
    }

    return EDITOR_STATUS_OK;
}




// FIXME: check if eob is on screen
// FIXME: rename in prepare page down start offset
int fast_page_down(editor_buffer_id_t ed_buffer, editor_view_id_t view, screen_t * screen, const codepoint_info_s ** start_cpi)
{
    // FIXME: must check line splitting
    // ex: tab expanded on two lines

    size_t index;
    const screen_line_t * l;
    auto b = screen_get_last_line(screen, &l, &index); // the last line become the first of the next screen;
    if (!b) {
        *start_cpi = nullptr;
        assert(0);
        return EDITOR_STATUS_ERROR;
    }

    const codepoint_info_s * cpi = nullptr;
    screen_line_get_cpinfo(l, 0, &cpi, screen_line_hint_no_column_fix);

    if (start_cpi) {
        assert(cpi->used);
        *start_cpi = cpi;
    }

    editor_view_set_start_offset(view, cpi->offset);
    return EDITOR_STATUS_OK;
}

/*
 FIXME: the first rewind is to wide
 */
int build_screen_line_list(editor_buffer_id_t ed_buffer,
                           editor_view_id_t ed_view,
                           const uint64_t start_offset_, const uint64_t until_offset_on_screen,
                           const uint32_t hints,
                           const screen_dimension_t & scr_dim,
                           std::vector<std::pair<uint64_t,uint64_t>> & screen_line_list)
{
    assert(start_offset_ <= until_offset_on_screen);

    // check buffer limits
    uint64_t start_offset = start_offset_;
    uint64_t until_offset = until_offset_on_screen;
    view_clip_offset(ed_buffer, ed_view, until_offset);

    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " : start_offset = " << start_offset << "\n";
    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " : until_offset = " << until_offset << "\n";

    assert(start_offset <= until_offset);

    editor_view_set_start_offset(ed_view, start_offset);

    // rewind / sync
    const uint32_t screen_max_cp = (scr_dim.l * scr_dim.c) * 1; // 4 is codec->max_codepoint_size(); in utf8
    uint64_t rewind_off = start_offset;
    if (build_screen_no_hints != 0) {
        rewind_and_resync_offset(ed_buffer, ed_view, screen_max_cp, hints, start_offset, &rewind_off);
    }

    editor_view_set_start_offset(ed_view, rewind_off);

    assert(rewind_off <= start_offset);
    assert(rewind_off <= until_offset);

    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " : after resync rdr_begin     = " << rewind_off << "\n";
    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " : after resync target_offset = " << until_offset << "\n";

    // FIXME:   define editor_log() like printf // app_log << "    -----------------\n";

    int count = 0;
    int found = 0;
    // uint64_t restart_offset = 0;

    // TODO: fix cp_index
    codepoint_info_s start_cpi;
    codepoint_info_reset(&start_cpi);
    start_cpi.offset = rewind_off;
    start_cpi.used   = true;

    screen_t * tmp_scr = editor_view_allocate_screen_by_id(ed_view);
    if (!tmp_scr)
        return -1;

    codec_io_ctx_s io_ctx {
        ed_buffer,
        editor_buffer_get_byte_buffer_id(ed_buffer),
        editor_view_get_codec_id(ed_view),
        0
    };

    // FIXME:   define editor_log() like printf // app_log << " Build screen line list (looking for offset("<< until_offset<<"))\n";
    do {
//        fprintf(stderr, "screen count(%d)\n", count);

        // FIXME:   define editor_log() like printf // app_log << " XXX Build screen list loop("<<count<<") offset(" << editor_view_get_start_offset( ed_view ) <<")\n";

        build_screen_layout(&io_ctx, ed_view, &start_cpi,tmp_scr);

        const codepoint_info_s * first_cpinfo = nullptr;

        screen_get_first_cpinfo(tmp_scr, &first_cpinfo);
#ifndef NDEBUG
        uint64_t first_off = first_cpinfo->offset;
#endif
        const codepoint_info_s * last_cpinfo  = nullptr;
        screen_get_last_cpinfo(tmp_scr, &last_cpinfo);

        // uint64_t last_off  = last_cpinfo->offset;
        // // FIXME:   define editor_log() like printf // app_log <<  " screen.start off = " <<  first_off << "\n";
        // // FIXME:   define editor_log() like printf // app_log <<  " target offset    = " <<  until_offset << "\n";
        // // FIXME:   define editor_log() like printf // app_log <<  " screen.end_off   = " <<  last_off <<  "\n";
        // // FIXME:   define editor_log() like printf // app_log <<  " nb used lines    = " <<  screen_get_number_of_used_lines(tmp_scr) <<  "\n";

        // FIXME:   define editor_log() like printf // app_log <<  "   ----------------\n";

        for (size_t li = 0; li != screen_get_number_of_used_lines(tmp_scr); ++li) {
            const screen_line_t * l = nullptr;
            screen_get_line(tmp_scr, li, &l);
            size_t col_index;
            const codepoint_info_t * cpi_first = nullptr;
            const codepoint_info_t * cpi_last  = nullptr;

            screen_line_get_first_cpinfo(l, &cpi_first, &col_index);
            screen_line_get_last_cpinfo(l, &cpi_last, &col_index);

            // // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " : PUSH " << screen_line_list.size() << " " << cpi_first->offset << " , " << cpi_last->offset << "\n";
            screen_line_list.push_back(std::pair<uint64_t,uint64_t>(cpi_first->offset, cpi_last->offset));
        }

        // compute  restart offset
        const screen_line_t * l = nullptr;
        size_t scr_line_index;
        size_t scr_col_index;

        assert(until_offset >= first_off);

        found = screen_get_line_by_offset(tmp_scr, until_offset, &l, &scr_line_index, &scr_col_index); // return EQ , GT, LT ?
        if (found == 1) {
            assert(l);
            const codepoint_info_s * cpi = nullptr;

            screen_line_get_cpinfo(l, 0, &cpi, screen_line_hint_no_column_fix);
            // restart_offset = cpi->offset;
            editor_view_set_start_offset(ed_view, cpi->offset);

            // FIXME:   define editor_log() like printf // app_log << " found restart_offset(" << restart_offset << ") on screen count "<< count << "\n";
            // FIXME:   define editor_log() like printf // app_log << " scr_line_index = "<< scr_line_index << "\n";
            // FIXME:   define editor_log() like printf // app_log << " scr_col_index   = "<< scr_col_index << "\n";

            break;

        } else {
            // FIXME:   define editor_log() like printf // app_log << " start offset (" << restart_offset << ") NOT FOUND\n";
        }

        if (screen_contains_offset(tmp_scr, until_offset) == true) {
            assert(0);
        }

        // set rdr_begin to first cp offset of LAST LINE of the current screen
        const codepoint_info_t * next_start_cpi = nullptr;
        bool ret = fast_page_down(ed_buffer, ed_view, tmp_scr, &next_start_cpi);
        if (ret == false) {
            // EOB
        }
        start_cpi = *next_start_cpi;

        // drop last push line , because it is the first one of next screen
        screen_line_list.pop_back(); // FIXME: check this in pag up/down

        count++;
    } while (true);

    screen_release(tmp_scr);

    return EDITOR_STATUS_OK;
}


////////////////////////////////////////////////////////////////////////////////
// 1 - rewind * N
// 2 - build line list
// 3 - choose new screen start

int scroll_up_N_lines(struct editor_message_s * msg, uint64_t N)
{
    if (!N)
        return 0;

    auto screen = get_previous_screen_by_id(msg->view_id);

    // take : start offset
    const codepoint_info_s * cpi = nullptr;
    screen_get_first_cpinfo(screen, &cpi);
    uint64_t until_offset = cpi->offset;

//    fprintf(stderr, " first cpi offset = [%lu]\n", cpi->offset);

    // take : start offset
    uint64_t start_offset = until_offset;
    uint64_t tmp_offset = 0;

//    fprintf(stderr, " until_offset = [%lu]\n", until_offset);



    // start_offset = get_beginning_of_previous_N_lines( previous_cp(offset), N)
    {
        //
        screen_dimension_t scr_dim = screen_get_dimension(screen);

        // resync here
        text_codec_io_ctx_s io_ctx {
            {
                msg->editor_buffer_id,
                editor_buffer_get_byte_buffer_id(msg->editor_buffer_id),
                editor_view_get_codec_id(msg->view_id),
                0 /* codex ctx */
            },
            0,
            0

        };


        {
            // FIXME:
            // must build a screen list from the new offset
            // if size(list) < N
            // restart with new N = size(list) - N
            if (start_offset > scr_dim.c * N * 2)
                start_offset -= scr_dim.c * N * 2;
            else
                start_offset = 0;

            // goto beginnning of line
            text_codec_sync_line(&io_ctx, start_offset, -1, &tmp_offset);
            // TODO: must return the number of decoded code points
            //        fprintf(stderr, " text_codec_sync_line -> [%d]\n", ret);
            start_offset = tmp_offset;
        }

    }

    // the minimum number of codepoints required to build the "scrolled up" layout is reached
    // now build layout, with the new start offset

    std::vector<std::pair<uint64_t,uint64_t>> screen_line_list;

//    fprintf(stderr, " build list of screen line (start_offset(%lu), stop_offset(%lu))\n", start_offset, until_offset);

    // build list of screen line (start_offset, stop_offset) until until_offset//eob is found
    {
        screen_dimension_t scr_dim = screen_get_dimension(screen);

        build_screen_line_list(msg->editor_buffer_id,
                               msg->view_id,
                               start_offset, until_offset,
                               0,
                               scr_dim,
                               screen_line_list);
    }

    // get until_offset index
    size_t until_offset_index = 0;
    {
        for (const auto & e : screen_line_list) {
//            fprintf(stderr, "line [%lu >= %lu < %lu] ? \n", e.first, until_offset, e.second);

            if ((e.first >= until_offset) && (until_offset <= e.second)) {
//                fprintf(stderr, " found [%lu] @ index %lu]\n", until_offset, until_offset_index);
                break;
            }
            ++until_offset_index;
        }
    }

    // get start_line_offset @ (until_offset_index-1)

    if (N > until_offset_index) {
        until_offset_index = 0;
    } else {
        until_offset_index -= N;
    }

    // notify layout change
    {
        codepoint_info_t cpi;
        codepoint_info_reset(&cpi);
        cpi.offset = screen_line_list[until_offset_index].first;
        cpi.used = true;

//        fprintf(stderr, " next start offset is %lu]\n", cpi.offset);

        set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
        set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, &cpi);
    }

    return EDITOR_STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////


int scroll_up(struct editor_message_s * msg)
{
    return scroll_up_N_lines(msg, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////

int page_up(struct editor_message_s * msg)
{
    auto cur_screen = get_previous_screen_by_id(msg->view_id);
    int64_t nb_lines = screen_get_max_number_of_lines(cur_screen);
    if (nb_lines <= 1) {
        return EDITOR_STATUS_OK;
    }

    return scroll_up_N_lines(msg, nb_lines - 2);
}

//////////////////////////////////////////////////////////////////////////////////////////

// FIXME: use the same algo as scroll_up : ie build line start/end list if list size > scroll_N
// it is slower but we can remove duplicated code
int scroll_down(struct editor_message_s * msg)
{
    auto screen = get_previous_screen_by_id(msg->view_id);

    // if eob on screen : do nothing
    auto index = screen_get_number_of_used_lines(screen); // the second line will become the first of the next screen;
    if (index < 2) {
        return EDITOR_STATUS_OK;
    }

    const screen_line_t * l = nullptr;
    screen_get_line(screen, 1, &l); /* get the second line */

    const codepoint_info_s * cpi = nullptr;
    screen_line_get_cpinfo(l, 0, &cpi, screen_line_hint_no_column_fix); // get fisrt cp of last line

    // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : cpi->offset " << cpi->offset << "\n";
    set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
    set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, cpi);

    return EDITOR_STATUS_OK;
}


////////////////////////////////////////////////////////////////////////////////


int page_down_internal(struct editor_message_s * msg)
{
    auto screen = get_previous_screen_by_id(msg->view_id);

    // if eob on screen : do nothing

    const screen_line_t * l = nullptr;
    size_t index;
    screen_get_last_line(screen, &l, &index); // the last line will become the first of the next screen;
    const codepoint_info_s * cpi = nullptr;
    screen_line_get_cpinfo(l, 0, &cpi, screen_line_hint_no_column_fix); // get fisrt cp of last line

    const codepoint_info_s * last_cpinfo;
    screen_get_last_cpinfo(screen, &last_cpinfo);

    size_t buf_size;
    byte_buffer_size(msg->byte_buffer_id, &buf_size);

    if (last_cpinfo->used && last_cpinfo->offset == buf_size) {
        return EDITOR_STATUS_OK;
    }


    // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : cpi->offset " << cpi->offset << "\n";

    set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
    set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, cpi);

    return EDITOR_STATUS_OK;
}


////////////////////////////////////////////////////////////////////////////////
// 1 - forward * N
// 2 - build line list
// 3 - choose new screen start

int scroll_down_N_lines(struct editor_message_s * msg, uint64_t N)
{
    if (!N)
        return 0;

    auto screen = get_previous_screen_by_id(msg->view_id);

    uint64_t buffer_size;
    byte_buffer_size(msg->byte_buffer_id, &buffer_size);

    // take : start offset
    const codepoint_info_s * cpi = nullptr;
    screen_get_first_cpinfo(screen, &cpi);
    uint64_t start_offset = cpi->offset;

    screen_dimension_t scr_dim = screen_get_dimension(screen);

    std::vector<std::pair<uint64_t,uint64_t>> screen_line_list;

    auto tmp_scr = screen_clone(screen);
    if (!tmp_scr) {
        return EDITOR_STATUS_ERROR;
    }

    uint64_t until_offset = 0;

    // simplify api by direct return return code
    size_t li = screen_get_number_of_used_lines(tmp_scr);
    const screen_line_t * l = nullptr;
    screen_get_line(tmp_scr, li - 1, &l);
    size_t col_index = 0;
    const codepoint_info_t * cpi_first = nullptr;
    screen_line_get_first_cpinfo(l, &cpi_first, &col_index);
    //
    until_offset = cpi_first->offset;

    while (true) {

        std::vector<std::pair<uint64_t,uint64_t>> tmp_list;

        build_screen_line_list(msg->editor_buffer_id,
                               msg->view_id,
                               start_offset, until_offset,
                               0,
                               scr_dim,
                               tmp_list);

        start_offset = until_offset; // next screen
        until_offset += (scr_dim.c * scr_dim.l) / 2;

        auto p = tmp_list.back();

        int eob = 0;
        if (p.second == buffer_size) {
            // drop last line if ! eob
            tmp_list.pop_back();
            eob = 1;
        } else {
            // nothing
        }

        screen_line_list.insert(screen_line_list.end(), tmp_list.begin(), tmp_list.end());

        if (screen_line_list.size() > N) {
            eob = 1;
        }

        if (eob) {
            break;
        }
    }

    N = std::min<uint64_t>(screen_line_list.size(), N);

    if (N) {
        start_offset = 0;
        start_offset = screen_line_list[N-1].first;

        // notify layout change
        {
            codepoint_info_t cpi;
            codepoint_info_reset(&cpi);

            cpi.offset = start_offset;
            cpi.used   = true;

            set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
            set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, &cpi);
        }
    }
    screen_release(tmp_scr);

    return EDITOR_STATUS_OK;
}


int page_down(struct editor_message_s * msg)
{
    auto cur_screen = get_previous_screen_by_id(msg->view_id);
    auto nb_lines = screen_get_max_number_of_lines(cur_screen);
    //    fprintf(stderr, "nb_lines= %u\n", nb_lines);
    if (nb_lines == 0) {
        return EDITOR_STATUS_OK;
    }

    int ret = scroll_down_N_lines(msg, nb_lines - 1);
    //abort();
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
// FIXME: split the function
//




////////////////////////////////////////////////////////////////////////////////


int goto_beginning_of_buffer(struct editor_message_s * msg)
{
    codepoint_info_t cpi;
    codepoint_info_reset(&cpi);
    cpi.offset      = 0;
    cpi.used        = true;
    cpi.cp_index    = 0;
    cpi.split_count = 0;

    set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, &cpi);
    set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
    return EDITOR_STATUS_OK;
}


int goto_end_of_buffer(struct editor_message_s * msg)
{
    auto view   = msg->view_id;

    size_t buffer_sz;

    byte_buffer_size(msg->byte_buffer_id, &buffer_sz);

    editor_view_set_start_offset(view, buffer_sz);

    // TODO: center view ?
    // scroll_up(L/2);

    codepoint_info_t cpi;
    codepoint_info_reset(&cpi);
    cpi.offset = buffer_sz;
    cpi.used   = true;
    set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, &cpi);
    set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
    return EDITOR_STATUS_OK;
}

void text_mode_register_modules_function()
{
    // text-mode
    editor_register_message_handler("page-down",                 page_down);
    editor_register_message_handler("page-up",                   page_up);
    editor_register_message_handler("goto-beginning-of-buffer",  goto_beginning_of_buffer);
    editor_register_message_handler("goto-end-of-buffer",        goto_end_of_buffer);

    editor_register_message_handler("scroll-up",                 scroll_up);
    editor_register_message_handler("scroll-down",               scroll_down);


#if 0
    // selection-mode
    editor_register_message_handler("begin-selection",           begin_selection);
    editor_register_message_handler("end-selection",             end_selection);
    editor_register_message_handler("mouse-wheel-up",            mouse_wheel_up);
    editor_register_message_handler("mouse-wheel-down",          mouse_wheel_down);
#endif
}

void text_mode_unregister_modules_function()
{

}


////////////////////////////////////////////////////////////////////////////////




// Module interface

extern "C"
SHOW_SYMBOL const char * module_name()
{
    return "mode/text";
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
    text_mode_register_modules_function();
    return MODULE_INIT_OK;
}

extern "C"
SHOW_SYMBOL int  module_quit()
{
    text_mode_unregister_modules_function();
    return 0;
}
