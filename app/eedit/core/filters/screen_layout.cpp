#include "../text_layout.hpp"

//#define DEBUG_SCREEN 1

namespace eedit
{
namespace core
{

enum build_layout_action_e {
	invalid_layout_action = 0,
	do_not_move_pen,
	move_pen_to_next_char,
	move_pen_to_next_line
	// move_pen_to_previous_char,
	// move_pen_to_previous_line,
};

// local struct -> screen mode(s) ?
struct advance {
	u32 x;
	u32 y;
};


////////////////////////////////////////////////////////////////////////////////
// HELPERS

/*
  this function checks if cp can be put in the current line
  by checking its insert position(x) and its horizontal advance (returned by the font)
  then fills the avance struct (used to compute the next codepoint posisiton)

  replace return code with enum
  font_error
  no_enough_space
*/

layout_status_e codepoint_fits(const s32 cp, const u32 max_width,
			       const u32 x, const u32 y, ew::graphics::fonts::font * ft, advance & adv, s32 * cp_filtered)
{
	adv.x = 0;
	adv.y = 0;

	ew::graphics::fonts::font_glyph_info glyph_info;
	bool bret = get_codepoint_glyph_info(ft, cp, cp_filtered, glyph_info);
	if (bret == false) {
		return layout_font_error;
	}

	adv.x = glyph_info.hori_advance;
	adv.y = glyph_info.vert_advance;

	assert(adv.x);
	assert(adv.y);

	if (x + glyph_info.hori_advance >= max_width) {
		// force next loop
		return no_layout_space_left;
	}

	return layout_ok;
}

// FIXME: add interline
// resize the output screen : take space(' ') as reference
bool maximize_screen(const u32 max_width,  const u32 max_height,
		     ew::graphics::fonts::font * ft,
		     screen_t * output_screen)
{

	ew::graphics::fonts::font_glyph_info space_glyph_info;

	s32 cp = ' ';
	get_codepoint_glyph_info(ft, cp, &cp, space_glyph_info);
	u32 tmpc = screen_get_max_width_px(output_screen) / space_glyph_info.hori_advance;
	u32 tmpc2 = screen_get_max_width_px(output_screen) % space_glyph_info.hori_advance;
	if (tmpc2 != 0)
		tmpc += 1; // add 1 column for remaining pixels

	u32 interline = 0;     // TODO: interline/kerning in ui config
	u32 tmpl = (interline  + screen_get_max_height_px(output_screen))  / space_glyph_info.vert_advance;
	u32 tmpl2 = (interline + screen_get_max_height_px(output_screen)) % space_glyph_info.vert_advance;
	if (tmpl2 != 0)
		tmpl += 1;  // add 1 line for remaining pixels

	// resize the screen
	screen_resize(output_screen, tmpl, tmpc);

	return true;
}


/*
  will be moved to mode ?
  slow ?
  todo pass previous cp
*/
void filter_codepoint(const s32 previous_cp, const s32 cur_cp, s32 * fcp, enum build_layout_action_e * action)
{
	const s32 NEW_LINE_SUB = ' '; // 0x21B2;

// TODO: filter cp
	switch (cur_cp) {

	case '\r': {
		// FIXME: check previous cp here ?  pass previous as parameter :-)
		*action = move_pen_to_next_line;
		*fcp = NEW_LINE_SUB; /* */
	}
	break;

	case '\n': {
		if (previous_cp == '\r') {
			*action = move_pen_to_next_line;
		} else {
			*action = move_pen_to_next_line;
		}

		*fcp = NEW_LINE_SUB; /* */
	}
	break;

	// decode error
	case -1: {
		*fcp = 0xfffd;
	}
	break;

	/* escape */
	case 127: {
		*fcp = 0xfffd;
	}
	break;

	case '\t': {
		*fcp = ' ';
	}
	break;

	default: {
		*fcp = cur_cp;

		if (*fcp < ' ')
			*fcp = ' '; // 0xfffd; // ' '; // here ?
	}
	break;
	}
}


/////////////////////////////////////////////////////

struct screen_mode_context_t {
	eedit::core::build_layout_context_t * blayout_ctx = nullptr; // to access the text buffer buffer

	u32 max_width_px;
	u32 max_height_px;
	u32 x;
	u32 y;

	u32 col;
	u32 row;

	u32 max_col;
	u32 max_row;

	u32 cp_count;

	s32 previous_cp;
	ew::graphics::fonts::font_glyph_info space_glyph_info;
};

bool screen_mode_init(build_layout_context_t * blayout_ctx, filter_context_t ** out)
{
	screen_mode_context_t * mode_ctx = new screen_mode_context_t;
	*out = reinterpret_cast<filter_context_t *>(mode_ctx);

	mode_ctx->blayout_ctx = blayout_ctx;

	// FIXME:  can be  done once if no multifont
	s32 cp = ' ';
	bool bret = get_codepoint_glyph_info(mode_ctx->blayout_ctx->ft, cp, &cp, mode_ctx->space_glyph_info);
	if (bret == false) {
		assert(0);
		return false;
	}

	// the pipeline depends on screen size -> screen mode init
	mode_ctx->max_col = mode_ctx->col = 0;
	mode_ctx->max_row = mode_ctx->row = 0;
	mode_ctx->x = 0;
	mode_ctx->y = 0;
	mode_ctx->max_width_px  = blayout_ctx->max_width_px;
	mode_ctx->max_height_px = blayout_ctx->max_height_px;

	mode_ctx->cp_count = 0;
	mode_ctx->previous_cp = 0;

	maximize_screen(blayout_ctx->max_width_px, blayout_ctx->max_height_px, blayout_ctx->ft, blayout_ctx->out);
	return true;
}

bool screen_mode_filter(build_layout_context_t * blctx, filter_context_t * ctx,
			const filter_io_t * const in, size_t nr_in,
			filter_io_t * out, const size_t max_out, size_t * nr_out)
{
	screen_mode_context_t * screen_mode_ctx = reinterpret_cast<screen_mode_context_t *>(ctx);

	advance adv;
	bool do_fetch = true;
	size_t out_index = 0;
	size_t in_index = 0;

	bool quit = false;


	auto & outscr  = screen_mode_ctx->blayout_ctx->out;
	auto & W       = screen_mode_ctx->max_width_px;
	auto & H       = screen_mode_ctx->max_height_px;
	auto & x       = screen_mode_ctx->x;
	auto & y       = screen_mode_ctx->y;
	auto ft        = screen_mode_ctx->blayout_ctx->ft;
	auto & row     = screen_mode_ctx->row;
	auto & col     = screen_mode_ctx->col;
	auto & max_row = screen_mode_ctx->max_row;
	auto & max_col = screen_mode_ctx->max_col;

	while ((in_index != nr_in) && !quit) {

#if 0
		app_log << " x("<<x<<") < W("<<W<<")\n";
		app_log << " y("<<y<<") < H("<<H<<")\n";
#endif
		//
		if (do_fetch == true) {
			out[out_index].end_of_pipe = false;
			out[out_index].quit        = in[in_index].end_of_pipe;
			out[out_index].cp          = in[in_index].cp;
			out[out_index].real_cp     = in[in_index].real_cp;
			out[out_index].offset      = in[in_index].offset;
			out[out_index].cp_index    = in[in_index].cp_index;


			out[out_index].is_selected = in[in_index].is_selected;


			out[out_index].split_flag  = in[in_index].split_flag;
			out[out_index].split_count = in[in_index].split_count;


			screen_mode_ctx->cp_count++; // not really cps

			do_fetch                   = false;
		}

		// find a better place for this test ?
		if (screen_mode_ctx->y + screen_mode_ctx->space_glyph_info.vert_advance >= H) {
			// must have an eof flag / augment pipeline info ?
			quit = true;
			out[out_index].valid = false;
			out[out_index].quit  = true;
			out[out_index].end_of_pipe = true;
			out_index++;
			break;
		}

		enum build_layout_action_e layout_action = move_pen_to_next_char;
		filter_codepoint(screen_mode_ctx->previous_cp, in[in_index].cp, &out[out_index].cp, &layout_action);

		screen_mode_ctx->previous_cp = in[in_index].cp;

		auto layout_error = codepoint_fits(out[out_index].cp, W, x, y, ft, adv, &out[out_index].cp); // TODO return a pen status ?
		switch (layout_error) {
		case layout_font_error: {
			// replace by <?> 0x1FFF ?
			do_fetch = false;
		}
		break;

		case no_layout_space_left: {
			// if codepoint doest no fits move to next line

			x = 0;
			y += screen_mode_ctx->space_glyph_info.vert_advance; // replace by max curline

			max_col = std::max(col, max_col);
			col = 0;
			row++;
			if (y + screen_mode_ctx->space_glyph_info.vert_advance >= H) {
				out[out_index].quit = true;
				quit = true;
				break;
			}
		}
		break;

		case layout_ok: {

			const screen_line_t    * l      = nullptr;
			const codepoint_info_s * ro_cpi = nullptr;
			codepoint_info_s * cpi          = nullptr;

			bool bret = screen_put_cp(outscr, in[in_index].real_cp, out[out_index].cp, row, col, &l, &ro_cpi);  //
			assert(bret == true);
			if (bret == true) {
				/// TODO:
			}
			cpi = const_cast<codepoint_info_s *>(ro_cpi);

			screen_mode_ctx->blayout_ctx->nr_put++;

			assert(cpi != nullptr);
			out[out_index].valid = true; // here ?

			// eedit::core::codepoint_info tmp_cpi

			// outscr->get_codepoint_info(row, col, &l, &cpi);

			cpi->x = x;
			cpi->y = y + screen_mode_ctx->space_glyph_info.vert_advance; // fonts are render bottom -> up
			cpi->w = screen_mode_ctx->space_glyph_info.hori_advance;
			cpi->h = screen_mode_ctx->space_glyph_info.vert_advance;
			cpi->offset      = out[out_index].offset;


			cpi->is_selected = out[out_index].is_selected;

			cpi->split_flag  = out[out_index].split_flag;
			cpi->split_count = out[out_index].split_count;

			cpi->cp_index    = out[out_index].cp_index;
			cpi->used        = true;

#ifdef DEBUG_SCREEN
			app_log << " cpi       = " << cpi << ",  ";
			app_log << " codepoint = " << cpi->codepoint << ",  ";
			app_log << " cpi->x    = " << cpi->x << ",  ";
			app_log << " cpi->y    = " << cpi->y << ",  ";
			app_log << " W         = " << W << ",  ";
			app_log << " H         = " << H << " : ";
			app_log << " row       = " << row << ",  ";
			app_log << " col       = " << col << " : ";
			app_log << " offset    = " << cpi->offset << "\n";
			app_log << " split_flag  = " << cpi->split_flag << "\n";
			app_log << " split_count = " << cpi->split_count << "\n";
#endif
			/* used for clipping */
			// unlikely


			const codepoint_info_s * first_cpinfo;
			const codepoint_info_s * last_cpinfo;

			screen_get_first_and_last_cpinfo(outscr, &first_cpinfo, &last_cpinfo);
			if (first_cpinfo->used == false) {
				codepoint_info_s * t = const_cast<codepoint_info_s*>(first_cpinfo);
				*t = *cpi;
			}

			// costly: remember (index,index) ?
			codepoint_info_s * t = const_cast<codepoint_info_s*>(last_cpinfo);
			*t = *cpi;

			if (layout_action == move_pen_to_next_char) {
				// move to next pixel
				col++;
				x += adv.x;
			}

			// move to next line ?
			if ((layout_action == move_pen_to_next_line) || (screen_mode_ctx->x >= screen_mode_ctx->max_width_px)) {
				x = 0;
				y += screen_mode_ctx->space_glyph_info.vert_advance;

				max_col = std::max(col, max_col);
				col = 0;

				row++;


				if (y + screen_mode_ctx->space_glyph_info.vert_advance >= H) {
					out[out_index].quit = true;
				}
			}

			in_index++;
			out_index++;
			do_fetch = true;

		}
		break;

		}
	}

	max_row = std::max(col, max_col);

	// validate output
	*nr_out = out_index;

	return true;
}

bool screen_mode_finish(build_layout_context_t * blctx, filter_context_t * mode_ctx)
{
	screen_mode_context_t * ctx = reinterpret_cast<screen_mode_context_t *>(mode_ctx);
	delete ctx;
	return true;
}

filter_t screen_mode = {
	"screen_mode",
	screen_mode_init,
	screen_mode_filter,
	screen_mode_finish
};

} // ! namespace core

} // ! namespace eedit
