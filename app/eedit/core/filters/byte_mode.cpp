#include "../log.hpp"
#include "../text_layout.hpp"

#include "../../api/include/byte_buffer.h"

namespace eedit
{
namespace core
{

struct byte_mode_context_t : public filter_context_t {

	build_layout_context_t * blayout_ctx = nullptr; // to access the text buffer buffer, will be removed ?

	u64       cur_offset = 0;
	byte_buffer_id_t bid        = 0;
	u64       total_read = 0;
	size_t    buf_size   = 0;
};

bool byte_mode_buffer_init(build_layout_context_t * blayout_ctx, filter_context_t ** out)
{
	byte_mode_context_t * ctx = new byte_mode_context_t;
	*out = ctx;

	ctx->blayout_ctx = blayout_ctx;
	ctx->bid = blayout_ctx->bid;

	ctx->cur_offset =  blayout_ctx->start_offset;

	ctx->total_read = 0;

	assert(ctx->bid);

	int ret = byte_buffer_size(ctx->bid, &ctx->buf_size);
	if (ret != 0)
		return false;

	app_log << __PRETTY_FUNCTION__ << " cur_offset = " << ctx->cur_offset << "\n";
	app_log << __PRETTY_FUNCTION__ << " buf_size   = " << ctx->buf_size   << "\n";

	return true;
}

bool byte_mode_buffer_filter(build_layout_context_t * blctx, filter_context_t * ctx_, const filter_io_t * const in, const size_t nr_in, filter_io_t * out, const size_t max_out, size_t * nr_out)
{
	byte_mode_context_t * ctx = static_cast<byte_mode_context_t *>(ctx_);


	uint8_t byte_value;
	size_t to_read = 1;
	size_t nb_read = 0;


//    app_log << __PRETTY_FUNCTION__ << " read @ cur_offset = " << ctx->cur_offset << "\n";

	int ret = byte_buffer_read(ctx->bid, ctx->cur_offset,  &byte_value, to_read, &nb_read);
	if (ret == 0) {
		ctx->total_read += nb_read;
	}

	*nr_out = 0;
	size_t index = 0;
	{
		out[index].content_type = editor_stream_type_bytes;
		out[index].offset = ctx->cur_offset;

		if (out[index].offset == ctx->buf_size) {
			byte_value = ' ';
			out[index].end_of_pipe = true;
		}

		out[index].byte_value  = byte_value;


		out[index].valid       = true;
		out[index].is_selected = false;
		*nr_out = 1;

		ctx->cur_offset += nb_read;
	}


	return true;
}

bool byte_mode_buffer_finish(build_layout_context_t * blctx, filter_context_t * ctx_)
{
	byte_mode_context_t * ctx = static_cast<byte_mode_context_t *>(ctx_);

	app_log << __PRETTY_FUNCTION__ << " total_read = " << ctx->total_read << "\n";

	delete ctx;
	return true;
}

filter_t byte_mode = {
	"byte_mode",
	byte_mode_buffer_init,
	byte_mode_buffer_filter,
	byte_mode_buffer_finish
};

} // ! namespace core

} // ! namespace eedit
