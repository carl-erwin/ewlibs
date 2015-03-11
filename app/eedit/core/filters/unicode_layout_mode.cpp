#include "../text_layout.hpp"


namespace eedit
{
namespace core
{

struct unicode_context_t : public filter_context_t {
	build_layout_context_t * blayout_ctx = nullptr; // to access the text buffer buffer

	u32 split_count;
	u32 split_flag;
	u64 cur_cp_index; // used for correct codepoint expansion, and column count :-)
};

bool unicode_buffer_init(build_layout_context_t * blayout_ctx, filter_context_t ** out)
{
	unicode_context_t * mode_ctx = new unicode_context_t;
	*out = mode_ctx;

	mode_ctx->blayout_ctx = blayout_ctx;

	mode_ctx->cur_cp_index      = 0;
	mode_ctx->split_count       = 0;
	mode_ctx->split_flag        = 0;

	// info
	if (blayout_ctx->start_cpi && blayout_ctx->start_cpi) {

		mode_ctx->cur_cp_index = blayout_ctx->start_cpi->cp_index;
		assert(mode_ctx->cur_cp_index != u64(-1));

		if (blayout_ctx->start_cpi->split_count) {
			mode_ctx->split_count   = blayout_ctx->start_cpi->split_count;
			mode_ctx->split_flag    = blayout_ctx->start_cpi->split_flag;
		}
	}

	return true;
}

bool unicode_buffer_filter(build_layout_context_t * blctx,
			   filter_context_t * ctx,
			   const filter_io_t * const in, const size_t nr_in,
			   filter_io_t * out, const size_t max_out, size_t * nr_out)
{
	unicode_context_t * unicode_ctx = static_cast<unicode_context_t *>(ctx);

	*nr_out = 0;
	size_t index = 0;
	{
		*nr_out = 1;
		out[index] = in[index];
		out[index].cp     = out[index].real_cp;
		out[index].valid  = true;

		out[index].cp_index = unicode_ctx->cur_cp_index;
		assert(out[index].cp_index != u64(-1));
		out[index].is_selected = false;
		out[index].split_flag  = unicode_ctx->split_flag;
		out[index].split_count = unicode_ctx->split_count;
		unicode_ctx->split_count = 0;
		unicode_ctx->split_flag  = 0;

		if ((out[index].cp == '\r') || (out[index].cp == '\n')) {
			unicode_ctx->cur_cp_index = 0;
		} else {
			++unicode_ctx->cur_cp_index;
		}

	}
	return true;
}

bool unicode_buffer_finish(build_layout_context_t * blctx, filter_context_t * ctx)
{
	unicode_context_t * unictx = static_cast<unicode_context_t *>(ctx);

	delete unictx;
	return true;
}

filter_t unicode_mode = {
	"unicode_mode",
	unicode_buffer_init,
	unicode_buffer_filter,
	unicode_buffer_finish
};

} // ! namespace core

} // ! namespace eedit
