#include <vector>
#include <map>

#include "text_layout.hpp"

#include "ew/core/time/time.hpp"
#include "log.hpp"

namespace eedit
{
namespace core
{


build_layout_context_s::build_layout_context_s(
	editor_buffer_id_t editor_buffer_id_,
	byte_buffer_id_t bid_,
	uint64_t sid_,
	const codepoint_info_s * start_cpi_, screen_t * out_)
	:
	editor_buffer_id(editor_buffer_id_),
	bid(bid_),
	sid(sid_),
	start_cpi(start_cpi_),
	out(out_)
{
	//        out_->dump(__PRETTY_FUNCTION__);
	u32 L = screen_get_max_number_of_lines(out);
	assert(L);
	u32 C = screen_get_max_number_of_columns(out);
	assert(C);

	screen_reset(out);

	auto view = sid;
	if (view == 0) {
		// no view configured yet
		return;
	}

	ft = (ew::graphics::fonts::font *)editor_view_get_font(view);  // the font used to build the layout: FIXME: break opengl dep, add bold/italic etc...
	assert(ft);

	codec_id = editor_view_get_codec_id(view);
	io_ctx = codec_io_ctx_s {editor_buffer_id, bid, codec_id, 0};

	size_t buffer_sz = 0;
	byte_buffer_size(bid, &buffer_sz);

	maximum_cp  = 1 + std::min<u64>( buffer_sz, (L) * (C)); // FIXME: interline

	max_width_px  = screen_get_max_width_px(out);
	max_height_px = screen_get_max_height_px(out);

	nr_put = 0;

	if (start_cpi) {
		start_offset = start_cpi->offset;
	} else {
		// nothing
		start_offset = 0;
	}

	// update view renderer marks
	editor_view_set_start_offset(view, start_offset ); // HERE
}


// TODO: layout_mode/glyph.hhp

void dump_glyp_info(const ew::graphics::fonts::font_glyph_info & glyph_info)
{
	app_log << " glyph_info.width           " << glyph_info.width << "\n";
	app_log << " glyph_info.height          " << glyph_info.height << "\n";
	app_log << " glyph_info.hori_bearing_x  " << glyph_info.hori_bearing_x << "\n";
	app_log << " glyph_info.hori_bearing_y  " << glyph_info.hori_bearing_y << "\n";
	app_log << " glyph_info.hori_advance    " << glyph_info.hori_advance << "\n";
	app_log << " glyph_info.vert_bearing_x  " << glyph_info.vert_bearing_x << "\n";
	app_log << " glyph_info.vert_bearing_y  " << glyph_info.vert_bearing_y << "\n";
	app_log << " glyph_info.vert_advance    " << glyph_info.vert_advance << "\n";
}


// extern or buffer->mod_context["mode_name"]
// put in screen ?
//

static std::map<s32, ew::graphics::fonts::font_glyph_info> glyph_info_cache;

bool get_glyph_info_from_cache(const s32 cp,
			       ew::graphics::fonts::font_glyph_info & out)
{
	auto it = glyph_info_cache.find(cp);
	if (it == glyph_info_cache.end()) {
		return false;
	}

	out = it->second;
	return true;
}

inline void add_glyph_info_to_cache(const s32 cp,
				    const ew::graphics::fonts::font_glyph_info & glyph_info)
{
	glyph_info_cache[cp] = glyph_info;
}

bool get_codepoint_glyph_info(ew::graphics::fonts::font * ft, const s32 cp, s32 * cp_filtered, ew::graphics::fonts::font_glyph_info & glyph_info)
{
	// isvisible ? char expension plugins...
	s32 vcp = cp;
	bool bret = get_glyph_info_from_cache(vcp, glyph_info);
	if (bret == false) {
		bret = ft->get_codepoint_glyph_info(vcp, glyph_info);
		if (bret == false) {
			app_log << "cannot get coddepoint("<<vcp<<") info, use substitute 0xfffd\n";
			vcp = 0xfffd;
		}
		*cp_filtered = vcp;
		add_glyph_info_to_cache(vcp, glyph_info);
		return true;
	}

	return true;
}


/*
  TODO: abstact screen as output device/output functor
  must think of "visual-line-mode" implementation
*/


/////////////////////////////
// PIPELINE
/////////////////////////////


extern filter_t byte_mode;          // take  buffer id   -> emit byte
extern filter_t text_decoder;       // takes bytes       -> emit code points
extern filter_t unicode_mode;       // takes code points -> count them -> emit codepoints
extern filter_t tab_expansion_mode; // takes codepoints  -> emit codepoints
extern filter_t hex_mode;           // takes bytes       -> emit codepoints

extern filter_t screen_mode;        // takes codepoints  -> emit screen


bool build_layout(build_layout_context_t & ctx)
{
	// move this per view ...
	std::vector<filter_t *> mode_list;
	std::vector<filter_context_t *> mode_ctx;

	/* TODO:
	   prepare per virtual screen module list

	   a given layout mode will be split in 5 parts:

	   mode_init(&mode)-> global init
	   mode_ctx_init(&mode_ctx_ptr) -> create per view context
	   mode_ctx_reset(mode_ctx_ptr) -> called at each beginning of layout pass
	   mode_ctx_release(mode_ctx_ptr)
	   mode_quit(mode);

	   // we will handle module dependencies/conflicts later
	   // we will add major "mode" later

	 */
	/* setup pipeline order */
	/*
	  will compute the correct start rdr offset
	  based on screen size
	  and will start to emit codepoints only when the value is
	*/
	// mode_list.push_back(&real_start_of_line_cache); /* NEW TODO: */

#if 0
// MERGE
	mode_list.push_back(&byte_mode); // TODO: use buffer api to get bytes
	mode_list.push_back(&hex_mode);  // TODO: use buffer api to get bytes
#else
	mode_list.push_back(&text_decoder);
	mode_list.push_back(&tab_expansion_mode);
#endif

	mode_list.push_back(&screen_mode); // LAST LAYOUT FILTER  is the screen

	// setup pipeline
	for (auto it = mode_list.begin(); it != mode_list.end(); ++it) {
		// for each mode call mod->init(ctx, mod->ctx);
		filter_context_t * tmp_ctx = nullptr;
		(*it)->init(&ctx, &tmp_ctx); // rename in reset_filter();
		mode_ctx.push_back(tmp_ctx);
	}

	// run pipeline
	const size_t MAX_OUT = 1024;
	filter_io_t cp_info_1[MAX_OUT * 8];
	size_t nr_in  = 0;
	filter_io_t cp_info_2[MAX_OUT * 8];
	size_t nr_out = 0;

	// TODO: rename filter_io_t -> editor_layout_io_t
	filter_io_t * pin  = cp_info_1;
	filter_io_t * pout = cp_info_2;

	size_t filter_io_sz = filter_io_size(); // @NOTE@ can be cached
	filter_io_t * default_cp = (filter_io_t *)alloca(filter_io_sz);
	filter_io_init(default_cp);

	for (size_t i = 0; i < MAX_OUT; i++) {
		std::copy(default_cp, default_cp + 1, &cp_info_1[i]);
		std::copy(default_cp, default_cp + 1, &cp_info_2[i]);
	}

	// TODO: mesure module elapsed time
	/// we will create real pipe/sockets here in future version
	// to allow parallel computation


	static int DEBUG_PIPELINE = 0;


	if (DEBUG_PIPELINE)
		app_log << "\n\n\n -------- LAYOUT MODE LOOP START -------- \n";

	bool end_of_pipe = false;

	while (end_of_pipe == false) {

		if (DEBUG_PIPELINE)
			app_log << " end_of_pipe("<<end_of_pipe<<") \n";

		nr_in  = 0;
		for (size_t i = 0; i < mode_list.size(); i++) {

			filter_context_t * tmp_ctx = mode_ctx[i];
			nr_out = 0;
			if (mode_list[i]->filter != nullptr) {

				assert(nr_out <= MAX_OUT);
				/*
				bool ret = mode_list[i]->filter(layout_ctx, tmp_ctx);
				filters call
				int n = get_filter_io(layout_ctx, iovec, io_cnt);   // blocking/notifications
				int n = put_filter_io(layout_ctx, iovec, io_cnt);   // non blocking
				int n = get_filter_io_count(layout_ctx);            // non blocking
				int n = unget_filter_io(layout_ctx, iovec, io_cnt); // blocking/notifications
				int n = unput_filter_io(layout_ctx, iovec, io_cnt); // non blocking
				this will enable potential threading
				*/

				bool ret = mode_list[i]->filter(&ctx, tmp_ctx, pin, nr_in, pout, MAX_OUT, &nr_out);

				if (DEBUG_PIPELINE)
					app_log << "\n" << "mode_list(" << mode_list[i]->name <<")[" << i << "] , nr_in(" << nr_in << ") nr_out(" << nr_out << ")\n";

				assert(nr_out <= 8 * MAX_OUT);
				if (nr_out == 0) {

					// count nr restart , max process time ?

					if (DEBUG_PIPELINE)
						app_log << " ... RESTART\n\n";
					break;
				}

				if (DEBUG_PIPELINE)
					app_log<< " pout["<<nr_out - 1<< "].end_of_pipe == " << pout[nr_out - 1].end_of_pipe << "\n";

				if (pout[nr_out - 1].end_of_pipe == true) {
					if (DEBUG_PIPELINE)
						app_log << " -------- LAYOUT end_of_pipe detected -------- \n";

					end_of_pipe = true;
				}

				if (ret == true) {
					std::swap(pin, pout);
					std::swap(nr_in, nr_out);
				}
			}
		}

		// TODO: improve end of pipe detection
		if (nr_in) {

			if (pin[nr_in - 1].quit == true) {
				if (DEBUG_PIPELINE)
					app_log << " -------- LAYOUT quit detected -------- \n";

				break;
			}
		}
	}

	if (DEBUG_PIPELINE)
		app_log << " -------- LAYOUT MODE LOOP END -------- \n\n\n";

	// slow :-) , on purpose
	// close pipeline
	// for each mode call mod->finish(ctx, mod->ctx);
	for (size_t i = 0; i < mode_list.size(); i++) {
		filter_context_t * tmp_ctx = mode_ctx[i];
		if (mode_list[i]->finish != nullptr) {
			mode_list[i]->finish(&ctx, tmp_ctx);
		}
	}


	return true;
}


bool build_screen_layout(struct codec_io_ctx_s * io_ctx, uint64_t sid, const codepoint_info_s * start_cpi, screen_t * out)
{
	assert(!start_cpi || start_cpi->used);

	assert(sid);

	build_layout_context_t blctx(io_ctx->editor_buffer_id, io_ctx->bid, sid, start_cpi, out);

	u32 t0 = ew::core::time::get_ticks();


	build_layout(blctx);                         // TODO: the layout code does not need to know the the screen

	// u64 rdr_end_off   = (u64)-1; // ed_buffer->rdr_end(); // FROM SID
#if 0
	u64 rdr_begin_off = ed_buffer->rdr_begin()->offset();
	u64 cursor_off    = ed_buffer->cursor_it()->offset();

	app_log << " rdr_begin_off = " << rdr_begin_off << "\n";
	app_log << " rdr_end_off   = " << rdr_end_off   << "\n";
	app_log << " cursor_off    = " << cursor_off    << "\n";
#endif

	const codepoint_info_s * first_cpi;
	const codepoint_info_s * last_cpi;

	screen_get_first_cpinfo(out, &first_cpi);
	screen_get_last_cpinfo(out, &last_cpi);

	// rdr_end_off = 0;

	// FIXME: does not work if src.w=0 or src.h=0

	if (first_cpi->used && last_cpi->used) {

#if 0
		app_log << "first_cp offset = " << first_cpi.offset  << "\n";
		app_log << "last cp offset = " << last_cpi.offset  << "\n";
		app_log << "first_cp splitcount = " << first_cpi.split_count  << "\n";
		app_log << "last cp splitcount  = " << last_cpi.split_count   << "\n";
		app_log << "first_cp index = " << first_cpi.cp_index  << "\n";
		app_log << "last cp index  = " << last_cpi.cp_index   << "\n";
		app_log << "buffer size = " << ed_buffer->raw_buffer()->end().offset()   << "\n";
#endif
		// rdr_end_off = last_cpi->offset;
	} else {
		app_log << "first_cpi.used "  << first_cpi->used << "\n";
		app_log << "last_cpi.used " << last_cpi->used << "\n";
	}

	// ed_buffer->rdr_end() = rdr_end_off;

	u32 t1 = ew::core::time::get_ticks();
	if (0) {
		app_log << " time to build layout = " << (t1 - t0) << " ms, nr put " << blctx.nr_put << "\n";
	}


	// FIXME:  add curor mod
	// screen_set_mark_offset(out);
	//out->mark_offset = ed_buffer->cursor_it();

	size_t sz;
	byte_buffer_size(io_ctx->bid, &sz);
	screen_set_buffer_size(out, sz);

	return true;
}

////////////////////////////////////////////////////////////////////////////////

} // ! namespace core

} // ! namespace eedit
