#include <map>

#include "../../../application/application.hpp"
#include "../../../core/core.hpp"
#include "../../../core/message_queue.hpp"
#include "../../../core/text_layout.hpp"
#include "../../../core/module/module.hpp"
#include "../../../core/rpc/rpc.hpp"

//
#include "../../../core/process_event_ctx.h"

#include "../../../api/include/buffer_log.h"

#include "../../../api/include/screen.h"

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

    with {buffer_id screen_id} : buffer |  codec_id | s32 cp/pictures/bytes | fold


*/

/*
  TODO:

  split CODE

   prepare
     editing_mode:

     the key/drag/paste/mouse/ events -> core_event_queue -> editing_modes pipeline


      must add drag/mouse/paste event in configuration:
      move keymap -> event_map

      eedit_register_module_function("xxx", (module_fn)xxx) -> eedit_register_editing_function("xxx", (module_fn)xxx)

      eedit_register_editing_function("cursor-motions-fn-name", (module_fn)xxx)
      eedit_register_editing_function("cursor-motions-fn-name", (module_fn)xxx)

     build_layout_mode:

      eedit_register_build_layout_function("text-mode",            (module_fn)build_text_mode_layout)
      eedit_register_build_layout_function("tab-expansion",        (module_fn)expand_tabs)
      eedit_register_build_layout_function("default-build-screen", (module_fn)default_build_screen_layout)

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


namespace eedit
{

namespace core
{

//
bool fast_page_down(editor_buffer_id_t ed_buffer, editor_view_id_t view, screen_t * screen, const codepoint_info_s ** start_cpi);


bool rewind_and_resync_screen(editor_buffer_id_t ed_buffer,
			      editor_view_id_t ed_view,
			      const u32 screen_max_cp, const u32 hints, u64 near_offset, u64 * resynced_offset);



bool to_next_screen_line_by_offset(event * _msg, const u64 screen_offset, u64 & new_screen_offset, codepoint_info_s & next_start_cpi);

bool to_next_screen_line_internal(event * _msg, codepoint_info_s & next_start_cpi);

bool to_next_screen_line(event * _msg);

bool page_up(eedit::core::event * _msg);

bool to_previous_screen_line_internal(event * msg, codepoint_info_s ** next_start_cpi);

bool to_previous_screen_line(event * msg);

bool page_up_internal(event * _msg, codepoint_info_s  & start_cpi);

// bool page_down_internal(eedit::core::event * _msg, codepoint_info_s & start_cpi);

// bool page_down(eedit::core::event * _msg);

bool goto_beginning_of_line(event * msg);

bool goto_end_of_line(event * msg);

bool goto_beginning_of_screen_line(event * _msg);




////////////////////////////////////////////////////////////////////////////////

// page down until offset is on screen
bool  resync_screen_layout(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, editor_view_id_t view, screen_dimension_t & dim)
{
	auto screen = get_previous_screen_by_id(view);


	// save offset
	u64 start_offset = screen_get_start_offset(screen); // SCREEN API ??!!?
	u64 new_start_offset = start_offset;

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
	return true;
}

// TODO: pass view(sid) as parameter
bool rewind_and_resync_screen(editor_buffer_id_t ed_buffer,
			      editor_view_id_t ed_view,
			      const u32 screen_max_cp, const u32 hints, u64 near_offset, u64 * resynced_offset)
{
	u64 rewind_off = near_offset;


	*resynced_offset = editor_view_get_start_offset(ed_view);

	if (hints & rewind_screen) {

		app_log << "hints & rewind_screen\n";


		if (screen_max_cp >= near_offset) {
			rewind_off = 0;
		} else {
			rewind_off = near_offset - screen_max_cp;
		}

		app_log << __FUNCTION__ << " : set rewind_off to " << rewind_off << "\n";

		editor_view_set_start_offset(ed_view, rewind_off);

		if (hints & resync_screen) {
			app_log << "hints & resync_screen\n";

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

			app_log << "FIXME FIXME FIXME !!!!!!!!!!!!\n";

			//editor_mark_to_beginnig_of_line(bid, ed_buffer->rdr_begin())
			// ed_view->rdr_begin()->toBeginningOfLine();
			// mark_set_offset(ed_view->rdr_begin(), resynced_offset);
		}
	}

	*resynced_offset = editor_view_get_start_offset( ed_view );
	assert(*resynced_offset <= rewind_off);
	assert(*resynced_offset <= near_offset);

	return true;
}


/* TODO: use dichotomic search */
bool screen_list_find_offset(const std::list<screen_t *> & scr_list,
			     const u64 target_offset,
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

	app_log << __FUNCTION__ << " abs_index  " << abs_index  << "\n";
	app_log << __FUNCTION__ << " line_index " << line_index << "\n";
	abs_line_index = abs_index + line_index;
	return found;
}


// TODO: move to common, view.h ?
bool view_clip_offset(
	editor_buffer_id_t ed_buffer,
	editor_view_id_t ed_view, u64 & offset)
{
	size_t buffer_sz;
	byte_buffer_size( editor_buffer_get_byte_buffer_id(ed_buffer), &buffer_sz);
	if (offset > buffer_sz) {
		offset = buffer_sz;
	}

	return true;
}


bool build_screen_line_list(editor_buffer_id_t ed_buffer,
			    editor_view_id_t ed_view,
			    const u64 start_offset_, const u64 until_offset_on_screen,
			    const u32 hints,
			    const screen_dimension_t & scr_dim,
			    std::vector<std::pair<uint64_t,uint64_t>> & screen_line_list)
{
	assert(start_offset_ <= until_offset_on_screen);

	// check buffer limits
	u64 start_offset = start_offset_;
	u64 until_offset   = until_offset_on_screen;
	view_clip_offset(ed_buffer, ed_view, until_offset);

	app_log << __FUNCTION__ << " : start_offset = " << start_offset << "\n";
	app_log << __FUNCTION__ << " : until_offset = " << until_offset << "\n";

	assert(start_offset <= until_offset);

	editor_view_set_start_offset(ed_view, start_offset);

	// rewind / sync
	const u32 screen_max_cp = (scr_dim.l * scr_dim.c) * 1; // 4 is codec->max_codepoint_size(); in utf8
	u64 rewind_off = start_offset;
	if (build_screen_no_hints != 0) {
		rewind_and_resync_screen(ed_buffer, ed_view, screen_max_cp, hints, start_offset, &rewind_off);
	}

	editor_view_set_start_offset(ed_view, rewind_off);

	assert(rewind_off <= start_offset);
	assert(rewind_off <= until_offset);

	app_log << __FUNCTION__ << " : after resync rdr_begin     = " << rewind_off << "\n";
	app_log << __FUNCTION__ << " : after resync target_offset = " << until_offset << "\n";

	app_log << "    -----------------\n";

	int count = 0;
	int found = 0;
	u64 restart_offset = 0;

	codepoint_info_s start_cpi;
	start_cpi.offset = rewind_off;
	start_cpi.used   = true;

	screen_t * tmp_scr = nullptr;
	screen_alloc_with_dimension(&tmp_scr, __FUNCTION__, &scr_dim);
	codec_io_ctx_s io_ctx {
		ed_buffer,
		editor_buffer_get_byte_buffer_id(ed_buffer),
		editor_view_get_codec_id(ed_view),
		0
	};

	app_log << " Build screen line list (looking for offset("<< until_offset<<"))\n";
	do {
		app_log << " XXX Build screen list loop("<<count<<") offset(" << editor_view_get_start_offset( ed_view ) <<")\n";

		build_screen_layout(&io_ctx, ed_view, &start_cpi,tmp_scr);

		const codepoint_info_s * first_cpinfo = nullptr;

		screen_get_first_cpinfo(tmp_scr, &first_cpinfo);
		u64 first_off = first_cpinfo->offset;

		const codepoint_info_s * last_cpinfo  = nullptr;
		screen_get_last_cpinfo(tmp_scr, &last_cpinfo);

		// u64 last_off  = last_cpinfo->offset;
		// app_log <<  " screen.start off = " <<  first_off << "\n";
		// app_log <<  " target offset    = " <<  until_offset << "\n";
		// app_log <<  " screen.end_off   = " <<  last_off <<  "\n";
		// app_log <<  " nb used lines    = " <<  screen_get_number_of_used_lines(tmp_scr) <<  "\n";

		app_log <<  "   ----------------\n";

		for (size_t li = 0; li != screen_get_number_of_used_lines(tmp_scr); ++li) {
			const screen_line_t * l = nullptr;
			screen_get_line(tmp_scr, li, &l);
			size_t col_index;
			const codepoint_info_t * cpi_first = nullptr;
			const codepoint_info_t * cpi_last  = nullptr;

			screen_line_get_first_cpinfo(l, &cpi_first, &col_index);
			screen_line_get_last_cpinfo(l, &cpi_last, &col_index);

			// app_log << __FUNCTION__ << " : PUSH " << screen_line_list.size() << " " << cpi_first->offset << " , " << cpi_last->offset << "\n";
			screen_line_list.emplace_back(cpi_first->offset, cpi_last->offset);
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
			restart_offset = cpi->offset;
			editor_view_set_start_offset(ed_view, cpi->offset);

			app_log << " found restart_offset(" << restart_offset << ") on screen count "<< count << "\n";
			app_log << " scr_line_index = "<< scr_line_index << "\n";
			app_log << " scr_col_index   = "<< scr_col_index << "\n";

			break;

		} else {
			app_log << " start offset (" << restart_offset << ") NOT FOUND\n";
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

		count++;
	} while (true);

	screen_release(tmp_scr);

	return true;
}


////////////////////////////////////////////////////////////////////////////////

bool page_down_internal(eedit::core::event * _msg)
{
	input_event * msg = (input_event *)_msg;
	auto view   = msg->view_id; // FIXEM: to check
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
		return true;
	}

	editor_view_set_start_offset(view, cpi->offset); // TODO: return offset ?

	app_log << __PRETTY_FUNCTION__ << " : cpi->offset " << cpi->offset << "\n";

	set_ui_change_flag(msg->editor_buffer_id,  msg->byte_buffer_id, msg->view_id);
	set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, cpi);

	return true;
}

bool page_down(eedit::core::event * _msg)
{
	return page_down_internal(_msg);
}


////////////////////////////////////////////////////////////////////////////////
// FIXME: split the function
// 1 - rewind
// 2 - build list
// 3 choose new screen start
//

bool page_up_internal(event * _msg, codepoint_info_s  & start_cpi)
{
	input_event * msg = (input_event *)_msg;

	auto buffer = editor_buffer_check_id(msg->byte_buffer_id);
	auto view   = msg->view_id;
	auto screen = get_previous_screen_by_id(msg->view_id);

	// get dimension
	screen_dimension_t scr_dim = screen_get_dimension(screen);

	u32 max_cp = (scr_dim.l * scr_dim.c) * 2; // 4 is codec->max_codepoint_size(); in utf8
	u64 save_start_off = editor_view_get_start_offset( view );
	app_log << __FUNCTION__ << " : save_start_off to " << save_start_off << "\n";

	if (save_start_off == 0) {
		app_log << __FUNCTION__ << " : nothing to do" << save_start_off << "\n";
		// nothing to do
		return true;
	}

	// (1)
	// compute max rewind offset
	u64 rewind_off;
	eedit::core::rewind_and_resync_screen(buffer, view, max_cp, rewind_screen, save_start_off, &rewind_off);

	app_log << __FUNCTION__ << " : set rewind_off to " << rewind_off << "\n";

	std::vector<std::pair<uint64_t,uint64_t>> screen_line_list;

	// (2)
	build_screen_line_list(buffer, view,
			       rewind_off, save_start_off,
			       0,
			       screen_get_dimension(screen),
			       screen_line_list);

	app_log << " screen_line_list.size() = " << screen_line_list.size() << "\n";

	for (size_t index = screen_line_list.size(); index > 0; ) {
		--index;

		/// app_log << " checking index " << index << " off {" << screen_line_list[index].first << " << " << save_start_off << " >> " << screen_line_list[index].second << "}\n";

		if ((screen_line_list[index].first <= save_start_off) && (screen_line_list[index].second >= save_start_off)) {
			//app_log  << " found offset on screen_line_list index = " << index << "\n";

			size_t screen_h = scr_dim.l + 1;

			if (index >= screen_h)
				index -= screen_h;
			else
				index = 0;

			//app_log  << " NEX START index = " << index << "\n";

			codepoint_info_t cpi;
			codepoint_info_reset(&cpi);
			cpi.used = true;
			cpi.offset = screen_line_list[index].first;
			//app_log << __PRETTY_FUNCTION__ << " NEW START : cpi.offset " << cpi.offset << "\n";
			set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
			set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, &cpi);
			start_cpi = cpi;
			editor_view_set_start_offset( view, cpi.offset );
			return true;
		}

	}

	assert(0);

	return true;
}



////////////////////////////////////////////////////////////////////////////////

bool page_up(event * _msg)
{
	codepoint_info_s  start_cpi;
	return page_up_internal(_msg,start_cpi);
}


// FIXME: check if eob is on screen
// FIXME: rename in prepare page down start offset
bool fast_page_down(editor_buffer_id_t ed_buffer, editor_view_id_t view, screen_t * screen, const codepoint_info_s ** start_cpi)
{
	// FIXME: must check line splitting
	// ex: tab expanded on two lines

	size_t index;
	const screen_line_t * l;
	auto b = screen_get_last_line(screen, &l, &index); // the last line become the first of the next screen;
	if (!b) {
		*start_cpi = nullptr;
		assert(0);
		return false;
	}

	const codepoint_info_s * cpi = nullptr;
	screen_line_get_cpinfo(l, 0, &cpi, screen_line_hint_no_column_fix);

	if (start_cpi) {
		assert(cpi->used);
		*start_cpi = cpi;
	}

	editor_view_set_start_offset(view, cpi->offset);
	return true;
}


bool goto_beginning_of_buffer(event * msg)
{
	codepoint_info_t cpi;
	codepoint_info_reset(&cpi);
	cpi.offset = 0;
	cpi.used   = true;
	set_ui_next_screen_start_cpi(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id, &cpi);
	set_ui_change_flag(msg->editor_buffer_id, msg->byte_buffer_id, msg->view_id);
	return true;
}


bool goto_end_of_buffer(event * msg)
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
	return true;
}


#if 0


////////////////////////////////////////////////////////////////////////////////
/*
 * TODO: use the user screen
 */
bool it_to_previous_line(text_buffer::iterator & ref)
{
	text_buffer::iterator old_point = ref;
	text_buffer::iterator & it = ref;

	u64 it_off    = it.offset();
	if (it_off == 0)
		return true;

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

	return true;
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
  u32 nr is the number of repetition ... (to-previous-line N &args) a la emacs --> lisp ?

*/


bool to_previous_line(event * _msg)
{
	app_log << __FUNCTION__ << "\n";

	input_event * msg = (input_event *)_msg;
	auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
	auto view = buffer->get_buffer_view_by_screen_id(msg->view_id);


	text_buffer::iterator old_point = view->cursor_it();
	it_to_previous_line(old_point);

	u64 rdr_begin_off  = buffer->rdr_begin()->offset();
	// u64 rdr_end_off   = d->rdr_end()->offset();
	u64 cursor_off    = buffer->cursor_it()->offset();

	if (cursor_off < rdr_begin_off) {
		it_to_previous_line(*buffer->rdr_begin());
		it_to_previous_line(*buffer->rdr_end());
	}

	set_ui_change_flag(msg->byte_buffer_id, msg->view_id);

	return true;
}


///////////////////////////////////////////////////////////////////////////////

bool it_to_next_physical_line(text_buffer::iterator & ref)
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

	return true;
}

////////////////////////////////////////////////////////////////////////////////


/* physical line */
bool to_next_line(event * _msg)
{
	input_event * msg = (input_event *)_msg;
	auto d = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

	it_to_next_physical_line(*d->cursor_it());

	u64 rdr_end_off   = d->rdr_end()->offset();
	u64 cursor_off    = d->cursor_it()->offset();

	if (cursor_off > rdr_end_off) {
		it_to_next_physical_line(*d->rdr_begin());
		//        it_to_next_physical_line(*d->rdr_end()); // not needed
	}

	set_ui_change_flag(process_ev_ctx);

	return true;
}

////////////////////////////////////////////////////////////////////////////////


bool scroll_down(editor_buffer_id_t ed_buffer, u32 n, screen_t * screen)
{
	return false;
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
bool to_next_screen_line_by_offset(event * _msg, const u64 screen_offset, u64 & new_screen_offset, codepoint_info_s & next_start_cpi)
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
		return false;
	}

	b = previous_screen->get_line_by_offset(screen_offset, &prev_line, saved_line, saved_column);
	if (!b) {

		app_log << " cannot find offset " << screen_offset << " on screen\n";
		app_log << " screen->first_cpinfo.offset = " << previous_screen->first_cpinfo.offset << "\n";
		app_log << " screen->last_cpinfo.offset = " << previous_screen->last_cpinfo.offset << "\n";
		app_log << " cannot find offset " << screen_offset << "on screen\n";

		// resync marks ?
		// assert(0);
		return false;
	}

	app_log << " scr_line_idx " << saved_line << "\n";
	app_log << " scr_col_idx  " << saved_column << "\n";
	app_log << " screen->number_of_used_lines()  " <<  previous_screen->number_of_used_lines() << "\n";

	size_t scr_new_line_idx = saved_line + 1;
	auto number_of_used_lines = previous_screen->number_of_used_lines();
	if (scr_new_line_idx >= number_of_used_lines) {

		// have eof ?
		if (number_of_used_lines == 1) {
			app_log << " END OF BUFFER DETECTED\n";
			return false;
		}

		// FIXME: if rdr_end on screen return
		// rebuild layout scroll down 1
		// FIXME : add function : scroll_down(screen_in, &screen_out);
		app_log << "MUST SCROLL\n";
		b = previous_screen->get_line(1, &line);
		if (b == false) {
			app_log << "cannot get line index 1\n";
			assert(0);
			return false;
		}

		b = line->get_first_cpinfo(&cpi, column);
		if (b == false) {
			assert(0);
			return false;
		}

		u64 saved_rdr_begin = ed_buffer->rdr_begin()->offset();

		cpi->check_invariant();
		next_start_cpi = *cpi;


		ed_buffer->rdr_begin()->move_to_offset(cpi->offset);

		app_log << "cpi->offset " << cpi->offset << "\n";
		app_log << "cpi->split_flag  = " << cpi->split_flag << "\n";
		app_log << "cpi->split_count = " << cpi->split_count << "\n";

		auto new_screen = new screen_t(*previous_screen);
		build_screen_layout(ed_buffer, cpi, new_screen);

		bool b = new_screen->get_line_by_offset(ed_buffer->cursor_it()->offset(), &line, saved_line, saved_column);
		if (!b) {
			assert(0);
			ed_buffer->rdr_begin()->move_to_offset(saved_rdr_begin);
			delete new_screen;
			return false;
		}

		app_log << " new scr_line_idx " << line << "\n";
		app_log << " new scr_col_idx  " << column << "\n"; // FIXME
		app_log << " new_screen->number_of_used_lines()  " <<  new_screen->number_of_used_lines() << "\n";

		scr_new_line_idx = saved_line + 1;

		if (scr_new_line_idx >= new_screen->number_of_used_lines()) {
			app_log << "ERROR\n";
			ed_buffer->rdr_begin()->move_to_offset(saved_rdr_begin);
			delete new_screen;
			return false;
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
		return true;
	}

	column = std::min<u32>(line->number_of_used_columns() - 1, saved_column);
	b = line->get_cpinfo((u32)column, &cpi, screen_line_t::fix_column_overflow);
	if (!b) {
		delete last_screen;
		return true;
	}

	// FIXME: apply to to_previous_screen_line
	while ((prev_line->last_offset()) == cpi->offset && (column < line->number_of_used_columns() - 1)) {
		++column;
		b = line->get_cpinfo((u32)column, &cpi, screen_line_t::fix_column_overflow);
		if (!b) {
			break;
		}
	}

	new_screen_offset = cpi->offset;

	app_log << "start_cpi->split_flag  = " << next_start_cpi.split_flag << "\n";
	app_log << "start_cpi->split_count = " << next_start_cpi.split_count << "\n";

	next_start_cpi.check_invariant();

	set_ui_next_screen_start_cpi(process_ev_ctx, &next_start_cpi);
	set_ui_change_flag(process_ev_ctx);

	if (last_screen != previous_screen) {
		app_log << "delete last_screen {\n";
		delete last_screen;
		app_log << "}\n\n";
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

// FIXME: remove cursor_it -> replace by mark list
bool to_next_screen_line_internal(event * _msg, codepoint_info_s & next_start_cpi)
{
	auto buffer = get_buffer_info_by_ll_bid(_msg->byte_buffer_id);

	u64 new_offset = buffer->cursor_it()->offset();
	auto b = to_next_screen_line_by_offset(_msg, new_offset, new_offset, next_start_cpi);
	if (b) {
		next_start_cpi.check_invariant();
	}

	buffer->cursor_it()->move_to_offset(new_offset);
	return true;
}

////////////////////////////////////////////////////////////////////////////////


// FIXME: must sync screen arround mark
// remove mark dep ?
bool to_next_screen_line(event * _msg)
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
 *  and add : cons u64 in_offset, u64 & out_offset
*/
bool to_previous_screen_line_internal(event * msg, codepoint_info_s ** next_start_cpi)
{
	app_log << __PRETTY_FUNCTION__ << " : Enter\n";

	auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
	auto screen = get_previous_screen_by_id(msg->view_id);

	u64 offset = buffer->cursor_it()->offset();
	screen_line_t * l = nullptr;
	size_t scr_line_index = 0;
	size_t scr_col_index  = 0;
	bool found = screen->get_line_by_offset(offset, &l, scr_line_index, scr_col_index);
	if (!found) {
		return true;
	}

	app_log << __PRETTY_FUNCTION__ << " : target col " << scr_col_index << "\n";
	app_log << __PRETTY_FUNCTION__ << " : current scr_line_index " << scr_line_index << "\n";
	app_log << __PRETTY_FUNCTION__ << " : current scr_col_index " << scr_col_index << "\n";


	// inside of screen move ?
	if (scr_line_index > 0) {

		scr_line_index--;
		found = screen->get_line(scr_line_index, &l);
		if (!found) {
			return true;
		}

		codepoint_info_s * cpi = nullptr;
		scr_col_index = std::min<u32>(l->number_of_used_columns() - 1, scr_col_index);
		found = l->get_cpinfo((u32)scr_col_index, &cpi);
		buffer->cursor_it()->move_to_offset(cpi->offset);

		// FIXME: copy screen first cpi, in ctx
		set_ui_change_flag(process_ev_ctx);
		set_ui_next_screen_start_cpi(process_ev_ctx, &screen->first_cpinfo);
		return true;
	}


	// outside of screen move
	{
		// FIXME: must fix reverse utf8 decoding

		if (buffer->rdr_begin()->offset() == 0) {
			return true;
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

		app_log << __PRETTY_FUNCTION__ << " : screen_index " << screen_index << "\n";
		app_log << __PRETTY_FUNCTION__ << " : abs_line_index " << abs_line_index << "\n";
		app_log << __PRETTY_FUNCTION__ << " : scr_line_index " << scr_line_index << "\n";
		app_log << __PRETTY_FUNCTION__ << " : scr_col_index " << scr_col_index << "\n";

		screen_t * scr = nullptr;
		auto it  = scr_list.begin();
		auto end = scr_list.end();
		for (size_t count = 0; count != screen_index; ++count) {
			++it;
			if (it == end)
				return false;
		}
		scr = *it;

		found = scr->get_line_by_offset(offset, &l, scr_line_index, scr_col_index);
		if (!found) {
			return true;
		}

		app_log << __PRETTY_FUNCTION__ << " : new scr_line_index " << scr_line_index << "\n";
		app_log << __PRETTY_FUNCTION__ << " : new scr_col_index " << scr_col_index << "\n";

		if (scr_line_index)
			scr_line_index--;
		found = scr->get_line(scr_line_index, &l);
		if (!found) {
			return true;
		}

		codepoint_info_s * cpi = nullptr;
		scr_col_index = std::min<u32>(l->number_of_used_columns() - 1, scr_col_index);
		found = l->get_cpinfo((u32)scr_col_index, &cpi);
		buffer->cursor_it()->move_to_offset(cpi->offset);

		found = l->get_cpinfo((u32)0, &cpi);
		buffer->rdr_begin()->move_to_offset(cpi->offset);

		set_ui_change_flag(process_ev_ctx);
		set_ui_next_screen_start_cpi(process_ev_ctx, cpi);
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////


bool to_previous_screen_line(event * msg)
{
	return to_previous_screen_line_internal(msg, nullptr);
}




////////////////////////////////////////////////////////////////////////////////

/* if end - start > screen set to begin */
bool goto_beginning_of_line(event * msg)
{
	app_log << __FUNCTION__ << "\n";

	auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
	auto screen = get_previous_screen_by_id(msg->view_id);

	screen_line_t * l;
	size_t scr_line_index;
	size_t scr_col_index;

	text_buffer::iterator & rdr_it = *buffer->rdr_begin();
	//  u64 old_rdr_begin = rdr_it.offset();

	text_buffer::iterator & it = *buffer->cursor_it();

	app_log << __FUNCTION__ << "\n";
	it.toBeginningOfLine();
	assert(it.column() == 0);
	app_log << __FUNCTION__ << "\n";

	u64 cur_off = it.offset();

	codepoint_info_s start_cpi;

	bool found = screen->get_line_by_offset(cur_off, &l, scr_line_index, scr_col_index);
	if (!found) {
		app_log << __PRETTY_FUNCTION__ << " : ! found on screen\n";
		rdr_it = it;
	} else {
		app_log << __PRETTY_FUNCTION__ << " : found cursor @ (l="<<scr_line_index<< ", c=" << scr_col_index<< ")\n";
		start_cpi = screen->first_cpinfo;
	}

	while (!found) {
		found = screen->get_line_by_offset(cur_off, &l, scr_line_index, scr_col_index);
		if (found) {
			app_log << __FUNCTION__ << " found cursor_offset("<<cur_off<<") on screen\n";
			start_cpi = screen->first_cpinfo;
			break;
		}
		page_up_internal(msg, start_cpi);
		build_screen_layout(msg, &start_cpi, screen);
	}

	assert(it.column() == 0);

	// need centering ?
	// while !found offset page_up();

	assert(start_cpi.cp_index != u64(-1));

	set_mark_changed_flag(process_ev_ctx);
	set_ui_next_screen_start_cpi(process_ev_ctx, &start_cpi);

	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool goto_end_of_line(event * msg)
{
	// app_log << __FUNCTION__ << "\n";

	auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
	auto screen = get_previous_screen_by_id(msg->view_id);

	text_buffer::iterator & it = *buffer->cursor_it(); // take cursor ref

	auto t0 = ew::core::time::get_ticks();
	it.toEndOfLine();
	u64 cur_off = it.offset();
	auto t1 = ew::core::time::get_ticks();
	app_log << "tb = " << (t1 - t0) << " ms\n";

	codepoint_info_s * start_cpi = nullptr;

	int count = 0;
	bool found = false;
	while (!found) {
		screen_line_t * l;
		size_t scr_line_index;
		size_t scr_col_index;
		found = screen->get_line_by_offset(cur_off, &l, scr_line_index, scr_col_index);
		if (found) {
			app_log << __FUNCTION__ << " found cursor_offset("<<cur_off<<") on screen\n";
			break;
		}
		bool ret = fast_page_down(buffer, screen, &start_cpi);
		if (ret != true)
			break;

		build_screen_layout(buffer, start_cpi, screen);
		++count;
	}

	auto t2 = ew::core::time::get_ticks();

	app_log << "toEndOfLine = " << (t1 - t0) << " ms\n";
	app_log << "page down count(" << count << ") = " << (t2 - t1) << " ms\n";

	// need centering ?
	// while !found offset page_up();

	set_mark_changed_flag(process_ev_ctx);

	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool goto_beginning_of_screen_line(event * _msg)
{
	// app_log << __FUNCTION__ << "\n";
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool goto_end_of_screen_line(event * _msg)
{
	// app_log << __FUNCTION__ << "\n";
	return true;
}

////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

// move to buffer ?
struct {
	u64 bid;

	int b_x;
	int b_y;
	u64 b_offset;
	bool use = false;

	int e_x;
	int e_y;
	u64 e_offset;

	codepoint_info_s start_cpi;

} selection_record;

////////////////////////////////////////////////////////////////////////////////


bool begin_selection(event * msg)
{
	// app_log << __FUNCTION__ << "\n";

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
		app_log << "cpi = " << cpi << "\n";
		if (cpi) {
			selection_record.e_offset = selection_record.b_offset = cpi->offset;


			app_log << "cpi->offset = " << cpi->offset << "\n";
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


	return true;
}

////////////////////////////////////////////////////////////////////////////////


bool end_selection(event * msg)
{
	// app_log << __FUNCTION__ << "\n";
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
	return true;
}

///////////////////////////////////////////////////////////////////////////////

bool insert_codepoint(eedit::core::event * _msg)
{
	input_event * msg = static_cast<input_event *>(_msg);

	auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

	s32 codepoint = msg->ev->start_value;

	text_buffer::iterator & it = *buffer->cursor_it();

	// save the view "important" marks cursor
	// should provide an atomatic refresh for registered iterators...
	u64 rdr_begin_off =  buffer->rdr_begin()->offset();
	u64 rdr_end_off   =  buffer->rdr_end()->offset();
	u64 cursor_off    =  buffer->cursor_it()->offset();

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

	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool insert_newline(event * _msg)
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
bool  remove_current_char(event * msg)
{
	auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

	std::lock_guard < decltype(*buffer->txt_buffer()) > lock(*buffer->txt_buffer());

	buffer->cursor_it()->check_invariants();
	buffer->rdr_begin()->check_invariants();
	buffer->rdr_end()->check_invariants();

	// save cursors
	u64 rdr_begin_off = buffer->rdr_begin()->offset();
	u64 rdr_end_off   = buffer->rdr_end()->offset();
	u64 cursor_off    = buffer->cursor_it()->offset();

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


	return true;
}

////////////////////////////////////////////////////////////////////////////////

// TODO: gfx_ctx->center_screen arround cursor = true;
bool remove_previous_char(event * msg)
{
	auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

	text_buffer::iterator & it = *buffer->cursor_it();

	if (it.offset() != 0) {
		--it;
		remove_current_char(msg);
	}

	set_buffer_changed_flag(process_ev_ctx);

	return true;
}

////////////////////////////////////////////////////////////////////////////////

// TODO: use intermediate cursor class, pass cursor as parameter
bool to_beginning_of_line(event * msg)
{
	auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

	// linear move
	buffer->cursor_it()->toBeginningOfLine();


	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool to_end_of_line(event * msg)
{
	auto buffer = get_buffer_info_by_ll_bid(msg->byte_buffer_id);

	std::lock_guard < decltype(*buffer->txt_buffer()) > lock(*buffer->txt_buffer());

	// linear move
	u64 off = buffer->cursor_it()->offset();
	buffer->cursor_it()->toEndOfLine();
	u64 off2 = buffer->cursor_it()->offset();
	assert(off <= off2);
	if (off == off2) {
		app_log << __FUNCTION__  << "(off == off2)\n";
		return true;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

// need unicode character class

// dump version : what behavior in fold ?

bool belong_to_word(const s32 c)
{
	// need char class
	if ((c >= '0' && c <= '9') ||
	    (c >= 'a' && c <= 'z') ||
	    (c >= 'A' && c <= 'Z') ||
	    (c == '_')) {
		return true;
	}

	return false;
}


// dump version : what behavior in fold ?
bool right_word(event * msg)
{
	auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
	u64 save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack

	auto end_off = buff->raw_buffer()->size();
	if (save_off == end_off) {
		return true;
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

	return true;
}

////////////////////////////////////////////////////////////////////////////////

// dump version : what behavior in fold ?
bool left_word(event * msg)
{
	auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
	u64 save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack
	if (save_off == 0)
		return true;

	auto & it = *buff->cursor_it();

	// get word start
	while (belong_to_word(*it)) {
		--it;
		if (it.offset() == 0) {
			set_ui_change_flag(process_ev_ctx);
			return true;
		}
	}

	// get prev word end
	while (!belong_to_word(*it)) {
		--it;
		if (it.offset() == 0) {
			set_ui_change_flag(process_ev_ctx);
			return true;
		}
	}

	// get word start
	while (belong_to_word(*it)) {
		--it;
		if (it.offset() == 0) {
			set_ui_change_flag(process_ev_ctx);
			return true;
		}
	}

	// ensure word start
	++it;

	set_ui_change_flag(process_ev_ctx);
	return true;
}

////////////////////////////////////////////////////////////////////////////////


bool mouse_wheel_up(event * msg)
{
	auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
	u64 save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack

	for (int i = 0; i < 3; ++i) {
		buff->cursor_it()->move_to_offset( buff->rdr_begin()->offset() );

		codepoint_info_s * next_start_cpi = nullptr;
		to_previous_screen_line_internal(msg, &next_start_cpi);

		// this update last last ref: FIXME: remove shared pointers and use copy operator, we really mean it
		// AND ALWAYS send a copy to the ui
		notify_buffer_changes(msg, next_start_cpi, false);

	}

	buff->cursor_it()->move_to_offset(save_off);

	return true;
}

////////////////////////////////////////////////////////////////////////////////


bool mouse_wheel_down(event * msg)
{
	auto buff = get_buffer_info_by_ll_bid(msg->byte_buffer_id);
	u64 save_off = buff->cursor_it()->offset(); // push_mark // pop_mark -> mark_stack

	for (int i = 0; i < 3; ++i) {
		buff->cursor_it()->move_to_offset( buff->rdr_end()->offset() );
		codepoint_info_s next_start_cpi;
		to_next_screen_line_internal(msg, next_start_cpi);
		next_start_cpi.check_invariant();
		notify_buffer_changes(msg, &next_start_cpi, false);
	}

	buff->cursor_it()->move_to_offset(save_off);

	return true;
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

void text_mode_register_modules_function()
{
	// text-mode
	eedit_register_module_function("page-down",                 (module_fn)page_down);
	eedit_register_module_function("page-up",                   (module_fn)page_up);
	eedit_register_module_function("goto-beginning-of-buffer",  (module_fn)goto_beginning_of_buffer);
	eedit_register_module_function("goto-end-of-buffer",        (module_fn)goto_end_of_buffer);


#if 0
	eedit_register_module_function("previous-line",             (module_fn)to_previous_screen_line);
	eedit_register_module_function("next-line",                 (module_fn)to_next_screen_line);
	eedit_register_module_function("previous-screen-line",      (module_fn)to_previous_screen_line);
	eedit_register_module_function("next-screen-line",          (module_fn)to_next_screen_line);
	eedit_register_module_function("self-insert",               (module_fn)insert_codepoint);
	eedit_register_module_function("insert-newline",            (module_fn)insert_newline);
	eedit_register_module_function("delete-backward-char",      (module_fn)remove_previous_char);
	eedit_register_module_function("delete-forward-char",       (module_fn)remove_current_char);
	eedit_register_module_function("goto-beginning-of-line",    (module_fn)goto_beginning_of_line);
	eedit_register_module_function("goto-end-of-line",          (module_fn)goto_end_of_line);

	// selection-mode
	eedit_register_module_function("begin-selection",           (module_fn)begin_selection);
	eedit_register_module_function("end-selection",             (module_fn)end_selection);

	eedit_register_module_function("left-word",                 (module_fn)left_word);
	eedit_register_module_function("right-word",                (module_fn)right_word);
	eedit_register_module_function("mouse-wheel-up",            (module_fn)mouse_wheel_up);
	eedit_register_module_function("mouse-wheel-down",          (module_fn)mouse_wheel_down);
#endif
}

////////////////////////////////////////////////////////////////////////////////

} // ! namespace core

} // ! namespace eedit
