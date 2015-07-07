#include "../text_layout/text_layout.hpp"


namespace eedit
{
namespace core
{

struct unicode_context_t : public editor_layout_filter_context_t {
    editor_layout_builder_context_t * blayout_ctx = nullptr; // to access the text buffer buffer

    uint32_t split_count;
    uint32_t split_flag;
    uint64_t cur_cp_index; // used for correct codepoint expansion, and column count :-)
};

bool unicode_buffer_init(editor_layout_builder_context_t * blayout_ctx, editor_layout_filter_context_t ** out)
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
        assert(mode_ctx->cur_cp_index != uint64_t(-1));

        if (blayout_ctx->start_cpi->split_count) {
            mode_ctx->split_count   = blayout_ctx->start_cpi->split_count;
            mode_ctx->split_flag    = blayout_ctx->start_cpi->split_flag;
        }
    }

    return true;
}

bool unicode_buffer_filter(editor_layout_builder_context_t * blctx,
                           editor_layout_filter_context_t * ctx,
                           layout_io_vec_t in_vec,
                           layout_io_vec_t out_vec)
{
    unicode_context_t * unicode_ctx = static_cast<unicode_context_t *>(ctx);

    if (layout_io_vec_size(in_vec) == 0)
        return false;


    layout_io_t out;

    layout_io_t & in = *layout_io_vec_first(in_vec);


    {
        out        = in;
        out.cp     = out.real_cp;
        out.valid  = true;

        out.cp_index = unicode_ctx->cur_cp_index;
        assert(out.cp_index != uint64_t(-1));
        out.is_selected = false;
        out.split_flag  = unicode_ctx->split_flag;
        out.split_count = unicode_ctx->split_count;
        unicode_ctx->split_count = 0;
        unicode_ctx->split_flag  = 0;

        if ((out.cp == '\r') || (out.cp == '\n')) {
            unicode_ctx->cur_cp_index = 0;
        } else {
            ++unicode_ctx->cur_cp_index;
        }

    }
    return true;
}

bool unicode_buffer_finish(editor_layout_builder_context_t * blctx, editor_layout_filter_context_t * ctx)
{
    unicode_context_t * unictx = static_cast<unicode_context_t *>(ctx);

    delete unictx;
    return true;
}

editor_layout_filter_t unicode_mode = {
    "unicode_mode",
    unicode_buffer_init,
    unicode_buffer_filter,
    unicode_buffer_finish
};

} // ! namespace core

} // ! namespace eedit
