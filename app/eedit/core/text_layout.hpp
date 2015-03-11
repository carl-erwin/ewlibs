#pragma once

#include <exception>
#include <stdexcept>
#include <stdint.h>

#include "ew/graphics/font/font.hpp"

#include "../application/application.hpp"


#include "editor_buffer.h"
#include "editor_view.h"
#include "codec.h"

#include "screen.h"

// TODO : export as "C" apis


enum editor_stream_type_e {
	editor_stream_type_bytes,
	editor_stream_type_unicode,
	editor_stream_type_codec_change
};


extern "C" {

	struct filter_io_s {

		uint32_t content_type;

		// general info
		uint8_t valid;
		uint8_t end_of_pipe; // skip
		uint8_t quit; // close pipeline
		uint8_t is_selected;

		uint64_t  offset;

		union {
			// content_type == bytes
			struct {
				uint8_t   byte_value;
			};

			// content_type == unicode
			struct {
				int32_t   cp;
				int32_t   real_cp;
				uint64_t  cp_index; // be carefull used const u64 invalid_cp_index
				uint32_t  split_flag;
				uint32_t  split_count;
			};

			// codec_change
			struct {
				codec_id_t         codec_id;
				codec_context_id_t codec_ctx;
			};
		};

		// TODO: add style infos ?
	};

	typedef struct filter_io_s filter_io_t;

}


inline size_t filter_io_size()
{
	return sizeof (struct filter_io_s);
}

inline void filter_io_init(filter_io_t * io)
{

	io->content_type = 0;

	// general info
	io->valid       = false; // ok
	io->end_of_pipe = false; // skip
	io->quit        = false; // close pipeline
	io->is_selected = false;

	io->offset      = (uint64_t)-1;
	io->byte_value  = 0;

	io->cp          = 0xffd;
	io->real_cp     = 0xffd;
	io->cp_index    = (uint64_t)-1; // be carrefull used const u64 invalid_cp_index
	io->split_flag  = 0;
	io->split_count = 0;
};



namespace eedit
{
namespace core
{

// TODO :  build_layout_context_t -> build_layout_context_s
struct build_layout_context_s {

	build_layout_context_s(editor_buffer_id_t editor_buffer_id_, byte_buffer_id_t bid_, uint64_t sid_, const codepoint_info_s * start_cpi_, screen_t * out_);

	// ctx in
	editor_buffer_id_t	editor_buffer_id = 0;
	byte_buffer_id_t	bid  = 0;
	editor_view_id_t	sid  = 0;
	codec_id_t              codec_id = 0;
	codec_io_ctx_s	        io_ctx;

	u64                  start_offset = 0;

	ew::graphics::fonts::font * ft = nullptr; // TODO: font family, map<font>

	u32 max_width_px;
	u32 max_height_px;

	// ctx out
	// TODO : move variables to screen/output device
	u32 nr_put;
	u32 screen_max_line;
	u32 screen_max_column;

	const codepoint_info_s * start_cpi = nullptr; // this cpi is mainly used to compute expansion split count, column count, etc...

	// in/out
	u64 maximum_cp;
	screen_t * out;
};

typedef struct build_layout_context_s build_layout_context_t;

void dump_glyp_info(const ew::graphics::fonts::font_glyph_info & glyph_info);



bool get_glyph_info_from_cache(const s32 cp,
			       ew::graphics::fonts::font_glyph_info & out);


void add_glyph_info_to_cache(const s32 cp,
			     const ew::graphics::fonts::font_glyph_info & glyph_info);


bool get_codepoint_glyph_info(ew::graphics::fonts::font * ft,
			      const s32 cp,
			      s32 * cp_filtered, /* filled on error with 0xfffd */
			      ew::graphics::fonts::font_glyph_info & glyph_info);

/*
  this function checks if cp can be put in the current line
  by checking its insert position(x) and its horizontal advance (returned by the font)
  then fills the avance struct (used to compute the next codepoint posisiton)

  replace return code with enum
  font_error
  no_enough_space
*/
enum layout_status_e {
	layout_ok,
	layout_font_error,
	no_layout_space_left,
};


/*
  TODO: abstact screen as output device/output functor
  must think of "visual-line-mode" implementation
*/


////////////////////////////////////////////////////////////////////////////////
// PIPELINE

/*
  TODO:
  must register modules for pipelining
*/

// build_layout_mode.hpp

typedef struct build_layout_context_s layout_context_t; // -> build_layout_context_t
struct filter_t;           // -> build_layout_mode_t
struct filter_context_t;   // -> build_layout_mode_context_t

// replace bool with enum
typedef bool (*mode_init_fn)(build_layout_context_t * ctx, filter_context_t ** to_allocate);

typedef bool (*mode_filter_fn)(build_layout_context_t * ctx,
			       filter_context_t * filter_ctx,
			       const filter_io_t * const in, const size_t nr_in,
			       filter_io_t * out, const size_t  max_out, size_t * nr_out);

typedef bool (*mode_finish_fn)(build_layout_context_t * ctx, filter_context_t *);

struct filter_context_t {};
struct filter_t {
	const char *   name;
	mode_init_fn   init;
	mode_filter_fn filter;
	mode_finish_fn finish;
};




// TODO:
// to be used by filters
int get_filter_io(layout_context_t * ctx, filter_io_t * iov, int iov_count);
int unget_filter_io(layout_context_t * ctx, filter_io_t * iov, int iov_count);


int put_filter_io(layout_context_t * ctx, filter_io_t * iov, int iov_count);
int unput_filter_io(layout_context_t * ctx, filter_io_t * iov, int iov_count);



/*
  for now 3-4 modes in pipeline:
  raw_bytes(rdr_start) -> unicode -> expansion -> (???) -> filtered cp info accum ? | screen(final)
*/

bool build_layout(build_layout_context_t & ctx);
bool build_screen_layout(struct codec_io_ctx_s * io_ctx, uint64_t sid, const codepoint_info_s * start_cpi, screen_t * out);

} // ! namespace core

} // ! namespace eedit
