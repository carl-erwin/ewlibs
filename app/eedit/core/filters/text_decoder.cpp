#include "../log.hpp"

#include "../../api/include/text_codec.h"

#include "ew/codecs/text/unicode/utf8/utf8.hpp"
#include "../text_layout.hpp"
#include "../../api/include/text_codec.h"


// RENAME in text decoder

namespace eedit
{
namespace core
{


struct text_decoder_context_t : public filter_context_t {
	// replace by bid ?
	build_layout_context_t * blayout_ctx = nullptr; // to access the text buffer buffer

	uint64_t next_offset = 0;
	size_t   buffer_size = 0;


	filter_io_t end_of_buffer;

	struct text_codec_io_s * iovc = nullptr;

	int iocnt = 16*1024;

	u32 split_count;
	u32 split_flag;
	u64 cur_cp_index; // used for correct codepoint expansion, and column count :-)
};




bool text_decoder_init(build_layout_context_t * blayout_ctx, filter_context_t ** out)
{
	text_decoder_context_t * mode_ctx = new text_decoder_context_t;
	*out = mode_ctx;

	mode_ctx->blayout_ctx = blayout_ctx;
	mode_ctx->next_offset = blayout_ctx->start_offset;
	byte_buffer_size(blayout_ctx->bid, &mode_ctx->buffer_size);

// prepare eof
	mode_ctx->end_of_buffer.is_selected  = true;
	mode_ctx->end_of_buffer.end_of_pipe  = true;
	mode_ctx->end_of_buffer.content_type = editor_stream_type_unicode;
	mode_ctx->end_of_buffer.offset       = mode_ctx->buffer_size;
	mode_ctx->end_of_buffer.cp           = ' ';
	mode_ctx->end_of_buffer.real_cp      = ' ';
	mode_ctx->end_of_buffer.valid        = true;

	mode_ctx->iocnt = 1024;
	mode_ctx->iovc  = new text_codec_io_s[mode_ctx->iocnt];

	mode_ctx->split_count  = 0;
	mode_ctx->split_flag   = 0;
	mode_ctx->cur_cp_index = 0;


	// info
	if (blayout_ctx->start_cpi && blayout_ctx->start_cpi->used) {

		mode_ctx->cur_cp_index = blayout_ctx->start_cpi->cp_index;

//	must add the resync
//	assert(mode_ctx->cur_cp_index != u64(-1));

		if (blayout_ctx->start_cpi->split_count) {
			mode_ctx->split_count   = blayout_ctx->start_cpi->split_count;
			mode_ctx->split_flag    = blayout_ctx->start_cpi->split_flag;
		}
	}




	return true;
}


bool text_decoder_finish(build_layout_context_t * blctx, filter_context_t * ctx_)
{
	text_decoder_context_t * ctx = static_cast<text_decoder_context_t *>(ctx_);

	// add destructor ?
	delete [] ctx->iovc;
	delete ctx;
	return true;
}


bool text_decoder_filter(build_layout_context_t * blctx,
			 filter_context_t * ctx_,
			 const filter_io_t * const in, const size_t nr_in,
			 filter_io_t * out, const size_t max_out, size_t * nr_out)
{


//	int get_filter_io(filter_io_t * in, size_t max_in);
//	int put_filter_io(const filter_io_t * const out, const size_t nr_out);

	text_decoder_context_t * ctx = static_cast<text_decoder_context_t *>(ctx_);

	editor_buffer_id_t editor_buffer_id      = ctx->blayout_ctx->editor_buffer_id;
	byte_buffer_id_t   bid       = ctx->blayout_ctx->bid;
	codec_id_t         codec_id  = ctx->blayout_ctx->codec_id;
	codec_context_id_t codec_ctx = 0;

	struct codec_io_ctx_s io_ctx {
		editor_buffer_id, bid, codec_id, codec_ctx
	};



	ctx->iovc[0].offset = ctx->next_offset;

	bool is_selected = false;

	*nr_out = 0;
	size_t index = 0;
	size_t buf_sz = ctx->buffer_size;


	static int debug = 0; // MOVE FLAG PER MODULE + appctl ?
	if (debug) {
		app_log <<__PRETTY_FUNCTION__ << " buffer size  = " << buf_sz << "\n";
		app_log <<__PRETTY_FUNCTION__ << " start offset = " << ctx->next_offset << "\n";
	}

	int ret = text_codec_read(&io_ctx, ctx->iovc, ctx->iocnt);

	if (debug)
		app_log <<__PRETTY_FUNCTION__ << " text_codec_read = " << ret << "\n";

	if (ret <= 0) {
		// error : log, auto eof
		index = 0;
	} else {
		index = ret;
	}

	for (size_t i = 0; i < index; i++) {
		out[i].end_of_pipe  = false;
		out[i].content_type = editor_stream_type_unicode;
		out[i].offset       = ctx->iovc[i].offset;
		out[i].cp           = ctx->iovc[i].cp;
		out[i].real_cp      = ctx->iovc[i].cp;
		out[i].valid        = true;
		out[i].is_selected  = is_selected;
		assert(ctx->iovc[i].offset <= buf_sz);


		out[i].cp_index    = ctx->cur_cp_index;
		out[i].split_flag  = ctx->split_flag;
		out[i].split_count = ctx->split_count;
		ctx->split_count = 0;
		ctx->split_flag  = 0;

		if ((out[i].cp == '\r') || (out[i].cp == '\n')) {
			ctx->cur_cp_index = 0;
		} else {
			++ctx->cur_cp_index;
		}


	}

	if (((int)index < ctx->iocnt)) {


		if (debug)
			app_log <<__PRETTY_FUNCTION__ << " index("<<index<<") < iocnt("<<ctx->iocnt<<"), end-of-buffer reached\n";

		out[index] = ctx->end_of_buffer;
		++index;
	}

	*nr_out = index;


	// prepare next offset
	if (index) {
		ctx->next_offset = ctx->iovc[index - 1].offset + ctx->iovc[index - 1].size;
	} else {
		//
		ctx->next_offset = ctx->buffer_size; // STOP
	}


	if (debug) {
		app_log <<__PRETTY_FUNCTION__ << " next start offset = " << ctx->next_offset << "\n";
	}

	if (debug) {
		app_log <<__PRETTY_FUNCTION__ << " nr_out = " << *nr_out  << "\n";
	}



	return true;
}

filter_t text_decoder = {
	"text_decoder",
	text_decoder_init,
	text_decoder_filter,
	text_decoder_finish
};

} // ! namespace core

} // ! namespace eedit
