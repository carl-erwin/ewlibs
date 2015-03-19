#include <vector>
#include <algorithm>

#include "mark.h"

#include "../text_layout.hpp"

namespace eedit
{
namespace core
{

struct mark_filter_context_t : public filter_context_t {
	build_layout_context_t * blayout_ctx = nullptr; // to access the text buffer
	std::vector<uint64_t> offset;
	size_t off_index = 0;
	bool skip_pass = true;
};

bool mark_filter_init(build_layout_context_t * blayout_ctx, filter_context_t ** out)
{
	mark_filter_context_t * ctx = new mark_filter_context_t;
	*out = ctx;
	
	ctx->blayout_ctx = blayout_ctx;

	// TODO: TEXT MODE CONTEXT { ebid, view, codec_id(view), codec_ctx(view) } ?

	// setup context
	auto ebid      = blayout_ctx->editor_buffer_id;
	auto view      = blayout_ctx->view;

	// get all marks
	auto buff_nmark     = editor_buffer_number_of_marks (ebid, MOVING_MARK);
	auto buff_nmark_fix = editor_buffer_number_of_marks (ebid, FIXED_MARK);
	auto view_nmark     = editor_view_number_of_marks   (view, MOVING_MARK);
	auto view_nmark_fix = editor_view_number_of_marks   (view, FIXED_MARK);
	
	// accumulate marks : TODO: check  errors
	std::vector<mark_t> marks;
	marks.resize(buff_nmark + view_nmark + buff_nmark_fix + view_nmark_fix);
	size_t last_index = 0;
	last_index += editor_buffer_get_marks(ebid, MOVING_MARK, buff_nmark, &marks[last_index]);
	last_index += editor_buffer_get_marks(ebid, FIXED_MARK,  buff_nmark, &marks[last_index]);
	last_index += editor_view_get_marks(view,   MOVING_MARK, view_nmark, &marks[last_index]);
	last_index += editor_view_get_marks(view,   FIXED_MARK,  view_nmark, &marks[last_index]);

	// NB: sort in increasing offset order
	std::sort(marks.begin(), marks.end(), [](mark_t m1, mark_t m2) { return mark_get_offset(m1) < mark_get_offset(m2); });

	ctx->offset.reserve(marks.size());

	uint64_t last_push = uint64_t(-1);

	for (auto m : marks) {
		auto cur_offset = mark_get_offset(m);
		if (cur_offset != last_push) {
			ctx->offset.push_back(cur_offset);
			last_push = cur_offset;
		}
	}

	// can be merged with previous for loop
	auto & vec = ctx->offset;
	for (auto & entry : vec) {
	      if (entry >= blayout_ctx->start_offset) {
		      ctx->off_index = &entry - &vec[0];
		      ctx->skip_pass = false;
		      break;
	      }
	}

	return true;
}

bool mark_filter_filter(build_layout_context_t * blctx, filter_context_t * ctx_,
		     const filter_io_t * const in, const size_t nr_in,
		     filter_io_t * out, const size_t max_out, size_t * nr_out)
{
	mark_filter_context_t * ctx = static_cast<mark_filter_context_t *>(ctx_);
  // if in[...] is "marked" set mark flag

	if (ctx->skip_pass) {
		for (size_t  i = 0; i < nr_in; i++) {
			out[i] = in[i];
			// look for offset ?
		}
		*nr_out = nr_in;
		return true;
	}

	for (size_t  i = 0; i < nr_in; i++) {
		out[i] = in[i];

		if (ctx->off_index < ctx->offset.size() && (out[i].offset == ctx->offset[ctx->off_index])) {
			out[i].is_selected = true;
			// skip
			while (ctx->off_index < ctx->offset.size() && ctx->offset[ctx->off_index] == out[i].offset)
				ctx->off_index++;
		}
	}

	*nr_out = nr_in;

	return true;
}

bool mark_filter_finish(build_layout_context_t * blctx, filter_context_t * ctx)
{
	delete static_cast<mark_filter_context_t *>(ctx);
	return true;
}

filter_t mark_filter = {
	"mark_filter",
	mark_filter_init,
	mark_filter_filter,
	mark_filter_finish
};

} // ! namespace core

} // ! namespace eedit
