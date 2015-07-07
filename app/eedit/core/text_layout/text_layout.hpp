#pragma once

#include <exception>
#include <stdexcept>
#include <stdint.h>

#include "ew/graphics/font/font.hpp"

//#include "../application/application.hpp"

#include "editor_export.h"
#include "editor_buffer.h"
#include "editor_view.h"
#include "editor_codec.h"

#include "editor_screen.h"

// TODO : export as "C" apis


enum editor_stream_type_e {
    editor_stream_type_bytes,
    editor_stream_type_unicode,
    editor_stream_type_codec_change
};

#include "layout_filter_io.h"
#include "layout_filter_io_vec.h"


extern "C" {
    EDITOR_EXPORT
    bool build_screen_layout(struct codec_io_ctx_s * io_ctx, editor_view_id_t sid, const codepoint_info_s * start_cpi, screen_t * out);
}


inline size_t layout_io_size()
{
    return sizeof (struct layout_io_s);
}

inline void filter_io_init(layout_io_t * io)
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
    io->cp_index    = (uint64_t)-1; // be carrefull used const uint64_t invalid_cp_index
    io->split_flag  = 0;
    io->split_count = 0;
};




// TODO :  editor_layout_builder_context_t -> editor_layout_builder_context_s
struct editor_layout_builder_context_s {

    editor_layout_builder_context_s(editor_buffer_id_t editor_buffer_id_, byte_buffer_id_t bid_, uint64_t sid_, const codepoint_info_s * start_cpi_, screen_t * out_);

    // ctx in
    editor_buffer_id_t	editor_buffer_id = 0;
    byte_buffer_id_t	bid  = 0;
    editor_view_id_t	view  = 0;
    codec_id_t              codec_id = 0;
    codec_io_ctx_s	        io_ctx;

    uint64_t                  start_offset = 0;

    ew::graphics::fonts::font * ft = nullptr; // TODO: font family, map<font>

    uint32_t max_width_px;
    uint32_t max_height_px;

    // ctx out
    // TODO : move variables to screen/output device
    uint32_t nr_put;
    uint32_t screen_max_line;
    uint32_t screen_max_column;

    const codepoint_info_s * start_cpi = nullptr; // this cpi is mainly used to compute expansion split count, column count, etc...

    // in/out
    uint64_t maximum_cp;
    screen_t * out;
};

typedef struct editor_layout_builder_context_s editor_layout_builder_context_t;



namespace eedit
{
namespace core
{

void dump_glyp_info(const ew::graphics::fonts::font_glyph_info & glyph_info);



bool get_glyph_info_from_cache(const int32_t cp,
                               ew::graphics::fonts::font_glyph_info & out);


void add_glyph_info_to_cache(const int32_t cp,
                             const ew::graphics::fonts::font_glyph_info & glyph_info);


bool get_codepoint_glyph_info(ew::graphics::fonts::font * ft,
                              const int32_t cp,
                              int32_t * cp_filtered, /* filled on error with 0xfffd */
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

typedef struct editor_layout_builder_context_s layout_context_t; // -> editor_layout_builder_context_t
struct editor_layout_filter_t;           // -> build_layout_mode_t
struct editor_layout_filter_context_t;   // -> build_layout_mode_context_t

// replace bool with enum
typedef bool (*editor_layout_filter_init_function_t)(editor_layout_builder_context_t * ctx, editor_layout_filter_context_t ** to_allocate);

typedef bool (*editor_layout_filter_function_t)(editor_layout_builder_context_t * ctx,
        editor_layout_filter_context_t * filter_ctx,
        layout_io_vec_t in,
        layout_io_vec_t out);

typedef bool (*editor_layout_filter_finish_function_t)(editor_layout_builder_context_t * ctx, editor_layout_filter_context_t *);

struct editor_layout_filter_context_t {};
struct editor_layout_filter_t {
    const char *                           name;
    editor_layout_filter_init_function_t   init;
    editor_layout_filter_function_t        filter;
    editor_layout_filter_finish_function_t finish;
};




/*
  for now 3-4 modes in pipeline:
  raw_bytes(rdr_start) -> unicode -> expansion -> (???) -> filtered cp info accum ? | screen(final)
*/

bool build_layout(editor_layout_builder_context_t & ctx);

} // ! namespace core

} // ! namespace eedit
