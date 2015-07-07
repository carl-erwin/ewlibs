#include "core/log/log.hpp"

#include "../../api/include/text_codec.h"

#include "ew/codecs/text/unicode/utf8/utf8.hpp"
#include "../text_layout/text_layout.hpp"
#include "../../api/include/text_codec.h"


// RENAME in text decoder

namespace eedit
{
namespace core
{


struct text_decoder_context_t : public editor_layout_filter_context_t {
    // replace by bid ?
    editor_layout_builder_context_t * blayout_ctx = nullptr; // to access the text buffer buffer

    uint64_t next_offset = 0;
    size_t   buffer_size = 0;


    layout_io_t end_of_buffer;

    layout_io_t tmp_io;


    struct text_codec_io_s * iovc = nullptr;

    int iocnt = 16*1024;

    uint32_t split_count;
    uint32_t split_flag;
    uint64_t cur_cp_index; // used for correct codepoint expansion, and column count :-)
};




bool text_decoder_init(editor_layout_builder_context_t * blayout_ctx, editor_layout_filter_context_t ** out)
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

    filter_io_init(&mode_ctx->tmp_io);

    // info
    if (blayout_ctx->start_cpi && blayout_ctx->start_cpi->used) {

        mode_ctx->cur_cp_index = blayout_ctx->start_cpi->cp_index;

//	must add the resync
//	assert(mode_ctx->cur_cp_index != uint64_t(-1));

        if (blayout_ctx->start_cpi->split_count) {
            mode_ctx->split_count   = blayout_ctx->start_cpi->split_count;
            mode_ctx->split_flag    = blayout_ctx->start_cpi->split_flag;
        }
    }




    return true;
}


bool text_decoder_finish(editor_layout_builder_context_t * blctx, editor_layout_filter_context_t * ctx_)
{
    text_decoder_context_t * ctx = static_cast<text_decoder_context_t *>(ctx_);

    // add destructor ?
    delete [] ctx->iovc;
    delete ctx;
    return true;
}


bool text_decoder_filter(editor_layout_builder_context_t * blctx,
                         editor_layout_filter_context_t * ctx_,
                         layout_io_vec_t in_vec,
                         layout_io_vec_t out_vec)
{


//	int get_filter_io(layout_io_t * in, size_t max_in);
//	int put_filter_io(const layout_io_t * const out, const size_t nr_out);

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

    size_t index = 0;
    size_t buf_sz = ctx->buffer_size;

    auto & out = ctx->tmp_io;

    static int debug = 0; // MOVE FLAG PER MODULE + appctl ?
    if (debug) {
        app_log <<__PRETTY_FUNCTION__ << " buffer size  = " << buf_sz << "\n";
        app_log <<__PRETTY_FUNCTION__ << " start offset = " << ctx->next_offset << "\n";
    }



    int ret = text_codec_read_forward(&io_ctx, ctx->iovc, ctx->iocnt);

    if (debug)
        app_log <<__PRETTY_FUNCTION__ << " text_codec_read = " << ret << "\n";

    if (ret <= 0) {
        // error : log, auto eof
        index = 0;
    } else {
        index = ret;
    }

    for (size_t i = 0; i < index; i++) {
        out.end_of_pipe  = false;
        out.content_type = editor_stream_type_unicode;
        out.offset       = ctx->iovc[i].offset;
        out.cp           = ctx->iovc[i].cp;
        out.real_cp      = ctx->iovc[i].cp;
        out.valid        = true;
        out.is_selected  = is_selected;
        assert(ctx->iovc[i].offset <= buf_sz);


        out.cp_index    = ctx->cur_cp_index;
        out.split_flag  = ctx->split_flag;
        out.split_count = ctx->split_count;
        ctx->split_count = 0;
        ctx->split_flag  = 0;

        if ((out.cp == '\r') || (out.cp == '\n')) {
            ctx->cur_cp_index = 0;
        } else {
            ++ctx->cur_cp_index;
        }

        layout_io_vec_push(out_vec, &out);


    }

    if (((int)index < ctx->iocnt)) {


        if (debug)
            app_log <<__PRETTY_FUNCTION__ << " index("<<index<<") < iocnt("<<ctx->iocnt<<"), end-of-buffer reached\n";

        out = ctx->end_of_buffer;
        layout_io_vec_push(out_vec, &out);
        ++index;
    }



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


    return true;
}

editor_layout_filter_t text_decoder = {
    "text_decoder",
    text_decoder_init,
    text_decoder_filter,
    text_decoder_finish
};

} // ! namespace core

} // ! namespace eedit
