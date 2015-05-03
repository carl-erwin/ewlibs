#include <vector>
#include <map>

#include "editor_module.h"
#include "editor_event_queue.h"
#include "editor_message_handler.h"


#include "../../../application/application.hpp"
#include "../../../core/core.hpp"

#include "../../../core/text_layout.hpp"
#include "../../../core/rpc/rpc.hpp"

//
#include "../../../core/process_event_ctx.h"

#include "../../../api/include/buffer_log.h"

#include "editor_screen.h"

#include "../../../api/include/text_codec.h"


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
        codec_io_ctx_s io_ctx {
            ed_buffer,
            editor_buffer_get_byte_buffer_id(ed_buffer),
            editor_view_get_codec_id(ed_view),
            0 /* codex ctx */
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

    codepoint_info_s start_cpi;
    start_cpi.offset = rewind_off;
    start_cpi.used   = true;

    screen_t * tmp_scr = nullptr;
    screen_alloc_with_dimension(&tmp_scr, &scr_dim, __FUNCTION__);
    codec_io_ctx_s io_ctx {
        ed_buffer,
        editor_buffer_get_byte_buffer_id(ed_buffer),
        editor_view_get_codec_id(ed_view),
        0
    };

    // FIXME:   define editor_log() like printf // app_log << " Build screen line list (looking for offset("<< until_offset<<"))\n";
    do {
        fprintf(stderr, "screen count(%d)\n", count);

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

// TODO: create a FILE with line of 10cp newline included
// example
/*
 a0001|xxx
 b0001|xxx
 c0001|xxx
 d0001|xxx
 e0001|xxx
 f0001|xxx
 g0001|xxx
 h0001|xxx
 i0001|xxx
 j0001|xxx
 k0001|xxx
 l0001|xxx
 m0001|xxx
 n0001|xxx
 o0001|xxx
 p0001|xxx
 q0001|xxx
 r0001|xxx
 s0001|xxx
 t0001|xxx
 u0001|xxx
 .....|xxx
 z0001|
 a0002|xxx

 */



int scroll_up(struct editor_message_s * msg)
{
    auto screen = get_previous_screen_by_id(msg->view_id);

    // take : start offset
    const codepoint_info_s * cpi = nullptr;
    screen_get_first_cpinfo(screen, &cpi);
    uint64_t until_offset = cpi->offset;

    fprintf(stderr, " first cpi offset = [%lu]\n", cpi->offset);

    // take : start offset
    uint64_t start_offset = until_offset;
    uint64_t tmp_offset = 0;

    fprintf(stderr, " until_offset = [%lu]\n", until_offset);


    // start_offset = get_beginning_of_line( previous_cp(offset) )
    {
        // TODO: need resync here: but we don't know the used codec
        codec_io_ctx_s io_ctx {
            msg->editor_buffer_id,
            editor_buffer_get_byte_buffer_id(msg->editor_buffer_id),
            editor_view_get_codec_id(msg->view_id),
            0 /* codex ctx */
        };

        text_codec_io_s iovc;
        iovc.offset = start_offset;
        iovc.size = 0;

        int ret;


        // if start of line got to end of previous line
        ret = text_codec_read_backward(&io_ctx, &iovc, 1);
        if (ret) {

        }

        fprintf(stderr, " read backward(%lu) -> [%lu]\n", start_offset, iovc.offset);

        start_offset = iovc.offset;

        // goto beginnning of line
        ret = text_codec_sync_line(&io_ctx, start_offset, -1, &tmp_offset);
        if (ret) {

        }

        fprintf(stderr, " goto beginning of line (%lu) -> [%lu]\n", start_offset, tmp_offset);

        start_offset = tmp_offset;
    }

    std::vector<std::pair<uint64_t,uint64_t>> screen_line_list;


    fprintf(stderr, " build list of screen line (start_offset(%lu), stop_offset(%lu))\n", start_offset, until_offset);

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
            fprintf(stderr, "line [%lu >= %lu < %lu] ? \n", e.first, until_offset, e.second);

            if ((e.first >= until_offset) && (until_offset < e.second)) {
                fprintf(stderr, " found [%lu] @ index %lu]\n", until_offset, until_offset_index);
                break;
            }
            ++until_offset_index;
        }
    }
    if (until_offset_index == screen_line_list.size()) {
        abort();
    }

    // get start_line_offset @ (until_offset_index-1)
    if (until_offset_index)
    {
        codepoint_info_t cpi;
        cpi.offset = screen_line_list[until_offset_index-1].first;
        cpi.used = true;

        fprintf(stderr, " next start offste is %lu]\n", cpi.offset);

        set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
        set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, &cpi);
    }

    return EDITOR_STATUS_OK;
}

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

int page_down(struct editor_message_s * _msg)
{
    return page_down_internal(_msg);
}


////////////////////////////////////////////////////////////////////////////////
// FIXME: split the function
// 1 - rewind
// 2 - build list
// 3 choose new screen start
//

int page_up_internal(struct editor_message_s * msg, codepoint_info_s  & start_cpi)
{
    auto buffer = editor_buffer_check_id(msg->byte_buffer_id);
    auto view   = msg->view_id;
    auto screen = get_previous_screen_by_id(msg->view_id);

    // get dimension
    screen_dimension_t scr_dim = screen_get_dimension(screen);

    uint32_t max_cp = (scr_dim.l * scr_dim.c) * 2; // 4 is codec->max_codepoint_size(); in utf8
    uint64_t save_start_off = editor_view_get_start_offset( view );
    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " : save_start_off to " << save_start_off << "\n";

    if (save_start_off == 0) {
        // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " : nothing to do" << save_start_off << "\n";
        // nothing to do
        return EDITOR_STATUS_OK;
    }

    // (1)
    // compute max rewind offset
    uint64_t rewind_off = save_start_off;
    rewind_and_resync_offset(buffer, view, max_cp, rewind_screen|resync_screen, save_start_off, &rewind_off);

    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " : set rewind_off to " << rewind_off << "\n";


    // FIXME: rewrite this ?
    // (2) : save line start/end offsets
    std::vector<std::pair<uint64_t,uint64_t>> screen_line_list;
    build_screen_line_list(buffer, view,
                           rewind_off, save_start_off,
                           0,
                           scr_dim,
                           screen_line_list);

    // FIXME:   define editor_log() like printf // app_log << " screen_line_list.size() = " << screen_line_list.size() << "\n";


    // (3) : get page up offset from screen_line_list
    for (size_t index = screen_line_list.size(); index > 0; ) {
        --index;

        /// // FIXME:   define editor_log() like printf // app_log << " checking index " << index << " off {" << screen_line_list[index].first << " << " << save_start_off << " >> " << screen_line_list[index].second << "}\n";

        if ((screen_line_list[index].first <= save_start_off) && (screen_line_list[index].second >= save_start_off)) {
            //// FIXME:   define editor_log() like printf // app_log  << " found offset on screen_line_list index = " << index << "\n";

            size_t screen_h = scr_dim.l + 1;

            if (index >= screen_h)
                index -= screen_h;
            else
                index = 0;

            //// FIXME:   define editor_log() like printf // app_log  << " NEX START index = " << index << "\n";

            codepoint_info_t cpi;
            codepoint_info_reset(&cpi);
            cpi.used = true;
            cpi.offset = screen_line_list[index].first;
            //// FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " NEW START : cpi.offset " << cpi.offset << "\n";
            set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
            set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, &cpi);
            start_cpi = cpi;
            editor_view_set_start_offset( view, cpi.offset );
            return EDITOR_STATUS_OK;
        }

    }

    assert(0);

    return EDITOR_STATUS_OK;
}



////////////////////////////////////////////////////////////////////////////////



int ___new_page_up(struct editor_message_s * msg)
{
    // get max number of screen lines
    auto cur_screen = get_previous_screen_by_id(msg->view_id);
    int64_t nb_lines = screen_get_max_number_of_lines(cur_screen);
    if (nb_lines == 0) {
        return EDITOR_STATUS_OK;
    }

    // advance screen of nb_lines - 1
    auto new_screen = editor_view_scroll_n(msg->view_id, nb_lines - 1);

    const screen_line_t * l = nullptr;
    screen_get_first_line(new_screen, &l);

    size_t column_index;
    const codepoint_info_t * cpi;
    screen_line_get_first_cpinfo(l, &cpi, &column_index);
    set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, cpi);
    set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);

    return EDITOR_STATUS_OK;
}



int page_up(struct editor_message_s * msg)
{
    codepoint_info_s  start_cpi;
    return page_up_internal(msg, start_cpi);
}


int goto_beginning_of_buffer(struct editor_message_s * msg)
{
    codepoint_info_t cpi;
    codepoint_info_reset(&cpi);
    cpi.offset = 0;
    cpi.used   = true;
    set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, &cpi);
    set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
    return EDITOR_STATUS_OK;
}


int goto_end_of_buffer(struct editor_message_s * msg)
{
    auto view   = msg->view_id;

    size_t buffer_sz;
    byte_buffer_size(msg->byte_buffer_id, &buffer_sz);

    editor_view_set_start_offset(view, buffer_sz );

    // TODO: center view ?
    // page_up(msg);

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

////////////////////////////////////////////////////////////////////////////////

// old implementation

#if 0


////////////////////////////////////////////////////////////////////////////////
/*
 * TODO: use the user screen
 */
int it_to_previous_line(text_buffer::iterator & ref)
{
    text_buffer::iterator old_point = ref;
    text_buffer::iterator & it = ref;

    uint64_t it_off    = it.offset();
    if (it_off == 0)
        return EDITOR_STATUS_OK;

    int column = 1;
    it.toBeginningOfLine();
    while (it != old_point) {
        ++column;
        ++it;
    }


    // goto previous line
    it.toBeginningOfLine();
    --it;
    it.toBeginningOfLine();

    for (auto new_column = 1; new_column < column; ++new_column) {
        if ((*it == '\r') || (*it == '\n'))
            break;
        ++it;
    }

    it_off    = it.offset();

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


/*
  TODO:

  we need a graphic context / view screen(L, C)
  to :
  - render font glyph
  - scroll etc ...
  - the event must contain layout helpers that mask this ????
  - msg->view->screen->lock() // get ctx for font ... ?
  - or else must be mono font based ... :-(

  - the screen can provide a "hidden" gfx/ctx to the the core
  - acquired/locked with  screen->lock()/ screen->unlock()
  - auto ftmgr = screen->get_font_manager();
  - ftmgt->get_font(bold|italic)->glyp_info()->xadvance() ... the layout code is there ?
  -

  //
  uint32_t nr is the number of repetition ... (to-previous-line N &args) a la emacs --> lisp ?

*/


int to_previous_line(struct editor_message_s * _msg)
{
    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";

    input_event * msg = (input_event *)_msg;
    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto view = buffer->get_buffer_view_by_screen_id(msg->view_id);


    text_buffer::iterator old_point = view->cursor_it();
    it_to_previous_line(old_point);

    uint64_t rdr_begin_off  = buffer->rdr_begin()->offset();
    // uint64_t rdr_end_off   = d->rdr_end()->offset();
    uint64_t cursor_off    = buffer->cursor_it()->offset();

    if (cursor_off < rdr_begin_off) {
        it_to_previous_line(*buffer->rdr_begin());
        it_to_previous_line(*buffer->rdr_end());
    }

    set_ui_change_flag(msg->byte_buffer_id, msg->view_id);

    return EDITOR_STATUS_OK;
}


///////////////////////////////////////////////////////////////////////////////

int it_to_next_physical_line(text_buffer::iterator & ref)
{
    text_buffer::iterator old_point = ref;
    text_buffer::iterator & it = ref;

    // FIXME: always precompute start of line ...
    // compute current column : use indexer etc ...
    int column = 1;
    it.toBeginningOfLine();
    while (it != old_point) {
        ++column;
        ++it;
    }

    // goto next line
    it.toEndOfLine();
    ++it;

    // FIXME: save mark_motion_context.target_column = column

    for (auto new_column = 1; new_column < column; ++new_column) {
        if ((*it == '\r') || (*it == '\n'))
            break;
        ++it;
    }

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


/* physical line */
int to_next_line(struct editor_message_s * _msg)
{
    input_event * msg = (input_event *)_msg;
    auto d = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    it_to_next_physical_line(*d->cursor_it());

    uint64_t rdr_end_off   = d->rdr_end()->offset();
    uint64_t cursor_off    = d->cursor_it()->offset();

    if (cursor_off > rdr_end_off) {
        it_to_next_physical_line(*d->rdr_begin());
        //        it_to_next_physical_line(*d->rdr_end()); // not needed
    }

    set_ui_change_flag(process_ev_ctx);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


int scroll_down(editor_buffer_id_t ed_buffer, uint32_t n, screen_t * screen)
{
    return EDITOR_STATUS_ERROR;
}


////////////////////////////////////////////////////////////////////////////////



/* TODO: check offset on screen
 * if the cursor is not on last line -> restart the screen with the 2nd line 1st offset
 *
 * compare the start of line offsets
 *
 * avoid layout recomputations by updating the last screen directly ? and send a clone ?
 *
*/
int to_next_screen_line_by_offset(struct editor_message_s * _msg, const uint64_t screen_offset, uint64_t & new_screen_offset, codepoint_info_s & next_start_cpi)
{
    new_screen_offset = screen_offset;

    input_event * msg = (input_event *)_msg;
    auto ed_buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto previous_screen = get_previous_screen_by_id(_msg->view_id);

    auto last_screen = previous_screen;

    // 1) need re-layout ?
    screen_line_t * first_l = nullptr;

    screen_line_t * prev_line = nullptr;

    screen_line_t * line = nullptr;
    codepoint_info_s * cpi = nullptr;
    size_t saved_line;
    size_t saved_column;
    size_t column;
    bool b;

    next_start_cpi = previous_screen->first_cpinfo;
    next_start_cpi.check_invariant();

    b = previous_screen->get_line(0, &first_l);
    if (!b) {
        assert(0);
        return EDITOR_STATUS_ERROR;
    }

    b = previous_screen->get_line_by_offset(screen_offset, &prev_line, saved_line, saved_column);
    if (!b) {

        // FIXME:   define editor_log() like printf // app_log << " cannot find offset " << screen_offset << " on screen\n";
        // FIXME:   define editor_log() like printf // app_log << " screen->first_cpinfo.offset = " << previous_screen->first_cpinfo.offset << "\n";
        // FIXME:   define editor_log() like printf // app_log << " screen->last_cpinfo.offset = " << previous_screen->last_cpinfo.offset << "\n";
        // FIXME:   define editor_log() like printf // app_log << " cannot find offset " << screen_offset << "on screen\n";

        // resync marks ?
        // assert(0);
        return EDITOR_STATUS_ERROR;
    }

    // FIXME:   define editor_log() like printf // app_log << " scr_line_idx " << saved_line << "\n";
    // FIXME:   define editor_log() like printf // app_log << " scr_col_idx  " << saved_column << "\n";
    // FIXME:   define editor_log() like printf // app_log << " screen->number_of_used_lines()  " <<  previous_screen->number_of_used_lines() << "\n";

    size_t scr_new_line_idx = saved_line + 1;
    auto number_of_used_lines = previous_screen->number_of_used_lines();
    if (scr_new_line_idx >= number_of_used_lines) {

        // have eof ?
        if (number_of_used_lines == 1) {
            // FIXME:   define editor_log() like printf // app_log << " END OF BUFFER DETECTED\n";
            return EDITOR_STATUS_ERROR;
        }

        // FIXME: if rdr_end on screen return
        // rebuild layout scroll down 1
        // FIXME : add function : scroll_down(screen_in, &screen_out);
        // FIXME:   define editor_log() like printf // app_log << "MUST SCROLL\n";
        b = previous_screen->get_line(1, &line);
        if (b == false) {
            // FIXME:   define editor_log() like printf // app_log << "cannot get line index 1\n";
            assert(0);
            return EDITOR_STATUS_ERROR;
        }

        b = line->get_first_cpinfo(&cpi, column);
        if (b == false) {
            assert(0);
            return EDITOR_STATUS_ERROR;
        }

        uint64_t saved_rdr_begin = ed_buffer->rdr_begin()->offset();

        cpi->check_invariant();
        next_start_cpi = *cpi;


        ed_buffer->rdr_begin()->move_to_offset(cpi->offset);

        // FIXME:   define editor_log() like printf // app_log << "cpi->offset " << cpi->offset << "\n";
        // FIXME:   define editor_log() like printf // app_log << "cpi->split_flag  = " << cpi->split_flag << "\n";
        // FIXME:   define editor_log() like printf // app_log << "cpi->split_count = " << cpi->split_count << "\n";

        auto new_screen = new screen_t(*previous_screen);
        build_screen_layout(ed_buffer, cpi, new_screen);

        bool b = new_screen->get_line_by_offset(ed_buffer->cursor_it()->offset(), &line, saved_line, saved_column);
        if (!b) {
            assert(0);
            ed_buffer->rdr_begin()->move_to_offset(saved_rdr_begin);
            delete new_screen;
            return EDITOR_STATUS_ERROR;
        }

        // FIXME:   define editor_log() like printf // app_log << " new scr_line_idx " << line << "\n";
        // FIXME:   define editor_log() like printf // app_log << " new scr_col_idx  " << column << "\n"; // FIXME
        // FIXME:   define editor_log() like printf // app_log << " new_screen->number_of_used_lines()  " <<  new_screen->number_of_used_lines() << "\n";

        scr_new_line_idx = saved_line + 1;

        if (scr_new_line_idx >= new_screen->number_of_used_lines()) {
            // FIXME:   define editor_log() like printf // app_log << "ERROR\n";
            ed_buffer->rdr_begin()->move_to_offset(saved_rdr_begin);
            delete new_screen;
            return EDITOR_STATUS_ERROR;
        }

        last_screen = new_screen;

    } else {

        // on screen
        last_screen->first_cpinfo.check_invariant();
        next_start_cpi = last_screen->first_cpinfo;
    }

    b = last_screen->get_line(scr_new_line_idx, &line);
    if (!b) {
        delete last_screen;
        return EDITOR_STATUS_OK;
    }

    column = std::min<uint32_t>(line->number_of_used_columns() - 1, saved_column);
    b = line->get_cpinfo((uint32_t)column, &cpi, screen_line_t::fix_column_overflow);
    if (!b) {
        delete last_screen;
        return EDITOR_STATUS_OK;
    }

    // FIXME: apply to to_previous_screen_line
    while ((prev_line->last_offset()) == cpi->offset && (column < line->number_of_used_columns() - 1)) {
        ++column;
        b = line->get_cpinfo((uint32_t)column, &cpi, screen_line_t::fix_column_overflow);
        if (!b) {
            break;
        }
    }

    new_screen_offset = cpi->offset;

    // FIXME:   define editor_log() like printf // app_log << "start_cpi->split_flag  = " << next_start_cpi.split_flag << "\n";
    // FIXME:   define editor_log() like printf // app_log << "start_cpi->split_count = " << next_start_cpi.split_count << "\n";

    next_start_cpi.check_invariant();

    set_ui_next_screen_start_cpi(process_ev_ctx, &next_start_cpi);
    set_ui_change_flag(process_ev_ctx);

    if (last_screen != previous_screen) {
        // FIXME:   define editor_log() like printf // app_log << "delete last_screen {\n";
        delete last_screen;
        // FIXME:   define editor_log() like printf // app_log << "}\n\n";
    }

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// FIXME: remove cursor_it -> replace by mark list
int to_next_screen_line_internal(struct editor_message_s * _msg, codepoint_info_s & next_start_cpi)
{
    auto buffer = get_buffer_info_by_ll_bid(_msg->byte_buffer_id);

    uint64_t new_offset = buffer->cursor_it()->offset();
    auto b = to_next_screen_line_by_offset(_msg, new_offset, new_offset, next_start_cpi);
    if (b) {
        next_start_cpi.check_invariant();
    }

    buffer->cursor_it()->move_to_offset(new_offset);
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


// FIXME: must sync screen arround mark
// remove mark dep ?
int to_next_screen_line(struct editor_message_s * _msg)
{
    codepoint_info_s cpi;
    auto b = to_next_screen_line_internal(_msg, cpi);
    if (b) {
        cpi.check_invariant();
    }
    return b;
}



////////////////////////////////////////////////////////////////////////////////


/*
 *  FIXME: memory leaks screen list
 *  must update start cp info for next build layout call
 *  pass mark offset as parameter
 *  and add : cons uint64_t in_offset, uint64_t & out_offset
*/
int to_previous_screen_line_internal(struct editor_message_s * msg, codepoint_info_s ** next_start_cpi)
{
    // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : Enter\n";

    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto screen = get_previous_screen_by_id(msg->view_id);

    uint64_t offset = buffer->cursor_it()->offset();
    screen_line_t * l = nullptr;
    size_t scr_line_index = 0;
    size_t scr_col_index  = 0;
    bool found = screen->get_line_by_offset(offset, &l, scr_line_index, scr_col_index);
    if (!found) {
        return EDITOR_STATUS_OK;
    }

    // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : target col " << scr_col_index << "\n";
    // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : current scr_line_index " << scr_line_index << "\n";
    // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : current scr_col_index " << scr_col_index << "\n";


    // inside of screen move ?
    if (scr_line_index > 0) {

        scr_line_index--;
        found = screen->get_line(scr_line_index, &l);
        if (!found) {
            return EDITOR_STATUS_OK;
        }

        codepoint_info_s * cpi = nullptr;
        scr_col_index = std::min<uint32_t>(l->number_of_used_columns() - 1, scr_col_index);
        found = l->get_cpinfo((uint32_t)scr_col_index, &cpi);
        buffer->cursor_it()->move_to_offset(cpi->offset);

        // FIXME: copy screen first cpi, in ctx
        set_ui_change_flag(process_ev_ctx);
        set_ui_next_screen_start_cpi(process_ev_ctx, &screen->first_cpinfo);
        return EDITOR_STATUS_OK;
    }


    // outside of screen move
    {
        // FIXME: must fix reverse utf8 decoding

        if (buffer->rdr_begin()->offset() == 0) {
            return EDITOR_STATUS_OK;
        }

        --(*buffer->rdr_begin());
        buffer->rdr_begin()->toBeginningOfLine();;

        size_t list_total_lines;

        std::list<screen_t *> scr_list;
        build_screen_list(buffer,
                          buffer->rdr_begin()->offset(),
                          offset,
                          build_screen_no_hints,
                          screen->dimension(),
                          scr_list,
                          list_total_lines);

        size_t screen_index;
        size_t abs_line_index;

        // FIXME : pass walk order to this function, find_forward find_reverse
        screen_list_find_offset(scr_list,
                                offset,
                                screen_index,
                                abs_line_index,
                                scr_line_index,
                                &l,
                                scr_col_index);

        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : screen_index " << screen_index << "\n";
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : abs_line_index " << abs_line_index << "\n";
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : scr_line_index " << scr_line_index << "\n";
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : scr_col_index " << scr_col_index << "\n";

        screen_t * scr = nullptr;
        auto it  = scr_list.begin();
        auto end = scr_list.end();
        for (size_t count = 0; count != screen_index; ++count) {
            ++it;
            if (it == end)
                return EDITOR_STATUS_ERROR;
        }
        scr = *it;

        found = scr->get_line_by_offset(offset, &l, scr_line_index, scr_col_index);
        if (!found) {
            return EDITOR_STATUS_OK;
        }

        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : new scr_line_index " << scr_line_index << "\n";
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : new scr_col_index " << scr_col_index << "\n";

        if (scr_line_index)
            scr_line_index--;
        found = scr->get_line(scr_line_index, &l);
        if (!found) {
            return EDITOR_STATUS_OK;
        }

        codepoint_info_s * cpi = nullptr;
        scr_col_index = std::min<uint32_t>(l->number_of_used_columns() - 1, scr_col_index);
        found = l->get_cpinfo((uint32_t)scr_col_index, &cpi);
        buffer->cursor_it()->move_to_offset(cpi->offset);

        found = l->get_cpinfo((uint32_t)0, &cpi);
        buffer->rdr_begin()->move_to_offset(cpi->offset);

        set_ui_change_flag(process_ev_ctx);
        set_ui_next_screen_start_cpi(process_ev_ctx, cpi);
    }

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


int to_previous_screen_line(struct editor_message_s * msg)
{
    return to_previous_screen_line_internal(msg, nullptr);
}




////////////////////////////////////////////////////////////////////////////////

/* if end - start > screen set to begin */
int goto_beginning_of_line(struct editor_message_s * msg)
{
    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";

    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto screen = get_previous_screen_by_id(msg->view_id);

    screen_line_t * l;
    size_t scr_line_index;
    size_t scr_col_index;

    text_buffer::iterator & rdr_it = *buffer->rdr_begin();
    //  uint64_t old_rdr_begin = rdr_it.offset();

    text_buffer::iterator & it = *buffer->cursor_it();

    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";
    it.toBeginningOfLine();
    assert(it.column() == 0);
    // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";

    uint64_t cur_off = it.offset();

    codepoint_info_s start_cpi;

    bool found = screen->get_line_by_offset(cur_off, &l, scr_line_index, scr_col_index);
    if (!found) {
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : ! found on screen\n";
        rdr_it = it;
    } else {
        // FIXME:   define editor_log() like printf // app_log << __PRETTY_FUNCTION__ << " : found cursor @ (l="<<scr_line_index<< ", c=" << scr_col_index<< ")\n";
        start_cpi = screen->first_cpinfo;
    }

    while (!found) {
        found = screen->get_line_by_offset(cur_off, &l, scr_line_index, scr_col_index);
        if (found) {
            // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " found cursor_offset("<<cur_off<<") on screen\n";
            start_cpi = screen->first_cpinfo;
            break;
        }
        page_up_internal(msg, start_cpi);
        build_screen_layout(msg, &start_cpi, screen);
    }

    assert(it.column() == 0);

    // need centering ?
    // while !found offset page_up();

    assert(start_cpi.cp_index != uint64_t(-1));

    set_mark_changed_flag(process_ev_ctx);
    set_ui_next_screen_start_cpi(process_ev_ctx, &start_cpi);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int goto_end_of_line(struct editor_message_s * msg)
{
    // // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";

    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto screen = get_previous_screen_by_id(msg->view_id);

    text_buffer::iterator & it = *buffer->cursor_it(); // take cursor ref

    auto t0 = ew::core::time::get_ticks();
    it.toEndOfLine();
    uint64_t cur_off = it.offset();
    auto t1 = ew::core::time::get_ticks();
    // FIXME:   define editor_log() like printf // app_log << "tb = " << (t1 - t0) << " ms\n";

    codepoint_info_s * start_cpi = nullptr;

    int count = 0;
    bool found = false;
    while (!found) {
        screen_line_t * l;
        size_t scr_line_index;
        size_t scr_col_index;
        found = screen->get_line_by_offset(cur_off, &l, scr_line_index, scr_col_index);
        if (found) {
            // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << " found cursor_offset("<<cur_off<<") on screen\n";
            break;
        }
        bool ret = fast_page_down(buffer, screen, &start_cpi);
        if (ret != true)
            break;

        build_screen_layout(buffer, start_cpi, screen);
        ++count;
    }

    auto t2 = ew::core::time::get_ticks();

    // FIXME:   define editor_log() like printf // app_log << "toEndOfLine = " << (t1 - t0) << " ms\n";
    // FIXME:   define editor_log() like printf // app_log << "page down count(" << count << ") = " << (t2 - t1) << " ms\n";

    // need centering ?
    // while !found offset page_up();

    set_mark_changed_flag(process_ev_ctx);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int goto_beginning_of_screen_line(struct editor_message_s * _msg)
{
    // // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int goto_end_of_screen_line(struct editor_message_s * _msg)
{
    // // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

// move to buffer ?
struct {
    uint64_t bid;

    int b_x;
    int b_y;
    uint64_t b_offset;
    bool use = false;

    int e_x;
    int e_y;
    uint64_t e_offset;

    codepoint_info_s start_cpi;

} selection_record;

////////////////////////////////////////////////////////////////////////////////


int begin_selection(struct editor_message_s * msg)
{
    // // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";

    selection_record.use = true;

    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);


    switch (msg->type & EDITOR_EVENT_TYPE_FAMILY_MASK) {

    case EDITOR_POINTER_BUTTON_EVENT_FAMILY: {
        //compute target offset
        auto scr = get_previous_screen_by_id(msg->view_id);

        selection_record.start_cpi = scr->first_cpinfo;
        set_ui_next_screen_start_cpi(process_ev_ctx, &selection_record.start_cpi);

        button_event * bev = static_cast<button_event *>(msg);

        codepoint_info_s * cpi = nullptr;
        scr->get_codepoint_by_coords(bev->ev->x, bev->ev->y, &cpi);
        // FIXME:   define editor_log() like printf // app_log << "cpi = " << cpi << "\n";
        if (cpi) {
            selection_record.e_offset = selection_record.b_offset = cpi->offset;


            // FIXME:   define editor_log() like printf // app_log << "cpi->offset = " << cpi->offset << "\n";
            buff->cursor_it()->move_to_offset(selection_record.e_offset);
            set_ui_change_flag(process_ev_ctx);
            ;
        } else {
            selection_record.e_offset = selection_record.b_offset = buff->cursor_it()->offset();

        }

        // save cursor _pose
    }
    break;


    case EDITOR_KEYBOARD_EVENT: {
        // save cursor pos
        selection_record.e_offset = selection_record.b_offset = buff->cursor_it()->offset();
    }
    break;


    default: {

    }
    }


    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


int end_selection(struct editor_message_s * msg)
{
    // // FIXME:   define editor_log() like printf // app_log << __FUNCTION__ << "\n";
    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    auto scr = get_previous_screen_by_id(msg->view_id);
    selection_record.start_cpi = scr->first_cpinfo;
    set_ui_next_screen_start_cpi(process_ev_ctx, &selection_record.start_cpi);


    if (selection_record.b_offset == selection_record.e_offset) {
        // move cursor to

        buff->cursor_it()->move_to_offset(selection_record.e_offset);

        set_ui_change_flag(process_ev_ctx);
        set_ui_next_screen_start_cpi(process_ev_ctx, &selection_record.start_cpi);
    }

    selection_record.use = false;
    return EDITOR_STATUS_OK;
}

///////////////////////////////////////////////////////////////////////////////

int insert_codepoint(struct editor_message_s * _msg)
{
    input_event * msg = static_cast<input_event *>(_msg);

    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    int32_t codepoint = msg->ev->start_value;

    text_buffer::iterator & it = *buffer->cursor_it();

    // save the view "important" marks cursor
    // should provide an atomatic refresh for registered iterators...
    uint64_t rdr_begin_off =  buffer->rdr_begin()->offset();
    uint64_t rdr_end_off   =  buffer->rdr_end()->offset();
    uint64_t cursor_off    =  buffer->cursor_it()->offset();

    // not needed ?
    //    std::lock_guard < decltype(*buffer->txt_buffer()) > lock(*buffer->txt_buffer());

    // the actual insert
    buffer->txt_buffer()->insert_codepoint(msg->byte_buffer_id, codepoint, it);

    // update view cursors/marks
    // should provide an atomatic refresh for registered iterators...
    *(buffer->rdr_begin()) = buffer->txt_buffer()->get_iterator(rdr_begin_off);
    *(buffer->rdr_end())   = buffer->txt_buffer()->get_iterator(rdr_end_off);
    *(buffer->cursor_it()) = buffer->txt_buffer()->get_iterator(cursor_off);

    if (cursor_off > rdr_end_off) {
        it_to_next_physical_line(*buffer->rdr_begin());
        it_to_next_physical_line(*buffer->rdr_end());
    }

    set_buffer_changed_flag(process_ev_ctx);

    // update cursor : put cursor after last inserted byte
    to_next_char(msg);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int insert_newline(struct editor_message_s * _msg)
{
    // FIXME: CR LF ?
    // input_event * msg = (input_event *)_msg;
    // msg->ev->start_value = '\r';
    // insert_codepoint(_msg);

    input_event * msg = (input_event *)_msg;
    msg->ev->start_value = '\n';
    return insert_codepoint(_msg);
}

////////////////////////////////////////////////////////////////////////////////

/* general expected behavior

  -) update low level buffer
  -) refresh all higher structs that depends on the updated range(s)
  -) set ctx flags to indicate the change (+ the changed range )

  fundamental-mode : refresh text layout

  send the new computed "screen" to the registered ui (found in msg/ ctx->viewers ?)

  the new screen is computed using the ctx->current_screen (remmeber that keyboard event is from )
*/
int  remove_current_char(struct editor_message_s * msg)
{
    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    std::lock_guard < decltype(*buffer->txt_buffer()) > lock(*buffer->txt_buffer());

    buffer->cursor_it()->check_invariants();
    buffer->rdr_begin()->check_invariants();
    buffer->rdr_end()->check_invariants();

    // save cursors
    uint64_t rdr_begin_off = buffer->rdr_begin()->offset();
    uint64_t rdr_end_off   = buffer->rdr_end()->offset();
    uint64_t cursor_off    = buffer->cursor_it()->offset();

    // the actual remove
    buffer->txt_buffer()->remove_current_char(msg->byte_buffer_id, *buffer->cursor_it());

    // restore cursors
    *(buffer->rdr_begin()) = buffer->txt_buffer()->get_iterator(rdr_begin_off);
    *(buffer->rdr_end())   = buffer->txt_buffer()->get_iterator(rdr_end_off);
    *(buffer->cursor_it()) = buffer->txt_buffer()->get_iterator(cursor_off);

    buffer->cursor_it()->check_invariants();
    buffer->rdr_begin()->check_invariants();
    buffer->rdr_end()->check_invariants();

    set_buffer_changed_flag(process_ev_ctx);


    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// TODO: gfx_ctx->center_screen arround cursor = true;
int remove_previous_char(struct editor_message_s * msg)
{
    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    text_buffer::iterator & it = *buffer->cursor_it();

    if (it.offset() != 0) {
        --it;
        remove_current_char(msg);
    }

    set_buffer_changed_flag(process_ev_ctx);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// TODO: use intermediate cursor class, pass cursor as parameter
int to_beginning_of_line(struct editor_message_s * msg)
{
    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    // linear move
    buffer->cursor_it()->toBeginningOfLine();


    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

int to_end_of_line(struct editor_message_s * msg)
{
    auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

    std::lock_guard < decltype(*buffer->txt_buffer()) > lock(*buffer->txt_buffer());

    // linear move
    uint64_t off = buffer->cursor_it()->offset();
    buffer->cursor_it()->toEndOfLine();
    uint64_t off2 = buffer->cursor_it()->offset();
    assert(off <= off2);
    if (off == off2) {
        // FIXME:   define editor_log() like printf // app_log << __FUNCTION__  << "(off == off2)\n";
        return EDITOR_STATUS_OK;
    }

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// need unicode character class

// dump version : what behavior in fold ?

int belong_to_word(const int32_t c)
{
    // need char class
    if ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c == '_')) {
        return EDITOR_STATUS_OK;
    }

    return EDITOR_STATUS_ERROR;
}


// dump version : what behavior in fold ?
int right_word(struct editor_message_s * msg)
{
    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    uint64_t save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack

    auto end_off = buff->raw_buffer()->size();
    if (save_off == end_off) {
        return EDITOR_STATUS_OK;
    }

    auto & it = *buff->cursor_it();

    bool start_on_word = belong_to_word(*it);

    if (start_on_word == false ) {
        while (!belong_to_word(*it)) {
            ++it;
        }
    }

    while (belong_to_word(*it)) {
        ++it;
    }

    set_ui_change_flag(process_ev_ctx);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// dump version : what behavior in fold ?
int left_word(struct editor_message_s * msg)
{
    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    uint64_t save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack
    if (save_off == 0)
        return EDITOR_STATUS_OK;

    auto & it = *buff->cursor_it();

    // get word start
    while (belong_to_word(*it)) {
        --it;
        if (it.offset() == 0) {
            set_ui_change_flag(process_ev_ctx);
            return EDITOR_STATUS_OK;
        }
    }

    // get prev word end
    while (!belong_to_word(*it)) {
        --it;
        if (it.offset() == 0) {
            set_ui_change_flag(process_ev_ctx);
            return EDITOR_STATUS_OK;
        }
    }

    // get word start
    while (belong_to_word(*it)) {
        --it;
        if (it.offset() == 0) {
            set_ui_change_flag(process_ev_ctx);
            return EDITOR_STATUS_OK;
        }
    }

    // ensure word start
    ++it;

    set_ui_change_flag(process_ev_ctx);
    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


int mouse_wheel_up(struct editor_message_s * msg)
{
    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    uint64_t save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack

    for (int i = 0; i < 3; ++i) {
        buff->cursor_it()->move_to_offset( buff->rdr_begin()->offset() );

        codepoint_info_s * next_start_cpi = nullptr;
        to_previous_screen_line_internal(msg, &next_start_cpi);

        // this update last last ref: FIXME: remove shared pointers and use copy operator, we really mean it
        // AND ALWAYS send a copy to the ui
        notify_buffer_changes(msg, next_start_cpi, false);

    }

    buff->cursor_it()->move_to_offset(save_off);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////


int mouse_wheel_down(struct editor_message_s * msg)
{
    auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
    uint64_t save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack

    for (int i = 0; i < 3; ++i) {
        buff->cursor_it()->move_to_offset( buff->rdr_end()->offset() );
        codepoint_info_s next_start_cpi;
        to_next_screen_line_internal(msg, next_start_cpi);
        next_start_cpi.check_invariant();
        notify_buffer_changes(msg, &next_start_cpi, false);
    }

    buff->cursor_it()->move_to_offset(save_off);

    return EDITOR_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////

// text-mode
// FIXME: move to text-mode init
/*
 every module function takes
 id(0)
 will not be allowed
 { buffer_id: id, screen_id: id, repeat }
*/

#endif

