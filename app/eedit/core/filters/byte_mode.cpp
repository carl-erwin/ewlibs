#include "core/log/log.hpp"

#include "../text_layout/text_layout.hpp"

#include "../../api/include/byte_buffer.h"

namespace eedit
{
namespace core
{

struct byte_mode_context_t : public editor_layout_filter_context_t {

    editor_layout_builder_context_t * blayout_ctx = nullptr; // to access the text buffer buffer, will be removed ?

    uint64_t       cur_offset = 0;
    byte_buffer_id_t bid        = 0;
    uint64_t       total_read = 0;
    size_t    buf_size   = 0;

    layout_io_t out;


};

bool byte_mode_buffer_init(editor_layout_builder_context_t * blayout_ctx, editor_layout_filter_context_t ** out)
{
    byte_mode_context_t * ctx = new byte_mode_context_t;
    *out = ctx;

    ctx->blayout_ctx = blayout_ctx;
    ctx->bid = blayout_ctx->bid;

    ctx->cur_offset =  blayout_ctx->start_offset;

    ctx->total_read = 0;

    assert(ctx->bid);

    filter_io_init(&ctx->out);


    int ret = byte_buffer_size(ctx->bid, &ctx->buf_size);
    if (ret != 0)
        return false;

    return true;
}

bool byte_mode_buffer_filter(editor_layout_builder_context_t * blctx, editor_layout_filter_context_t * ctx_,
                             layout_io_vec_t in_vec,
                             layout_io_vec_t out_vec)
{
    byte_mode_context_t * ctx = static_cast<byte_mode_context_t *>(ctx_);


    uint8_t byte_value;
    size_t to_read = 1;
    size_t nb_read = 0;

    int ret = byte_buffer_read(ctx->bid, ctx->cur_offset,  &byte_value, to_read, &nb_read);
    if (ret == 0) {
        ctx->total_read += nb_read;
    }


    layout_io_t & out = ctx->out;


    {
        out.content_type = editor_stream_type_bytes;
        out.offset = ctx->cur_offset;

        if (out.offset == ctx->buf_size) {
            byte_value = ' ';
            out.end_of_pipe = true;
        }

        out.byte_value  = byte_value;


        out.valid       = true;
        out.is_selected = false;

        layout_io_vec_push(out_vec, &out);

        ctx->cur_offset += nb_read;
    }


    return true;
}

bool byte_mode_buffer_finish(editor_layout_builder_context_t * blctx, editor_layout_filter_context_t * ctx_)
{
    byte_mode_context_t * ctx = static_cast<byte_mode_context_t *>(ctx_);

    delete ctx;
    return true;
}

editor_layout_filter_t byte_mode = {
    "byte_mode",
    byte_mode_buffer_init,
    byte_mode_buffer_filter,
    byte_mode_buffer_finish
};

} // ! namespace core

} // ! namespace eedit
