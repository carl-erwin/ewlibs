#include "../text_layout/text_layout.hpp"

/* TODO: abstract codecs,
   update pipeline_info_t to take a data_type { bytes (octetstream)  , unicode (text), sound, video -> mime/type  }
   rename pipeline_info_t stream_info_t
*/

namespace eedit
{
namespace core
{

struct hex_mode_context_t : public editor_layout_filter_context_t {
    editor_layout_builder_context_t * blayout_ctx = nullptr; // to access the text buffer buffer

    layout_io_t out;

};

bool hex_mode_init(editor_layout_builder_context_t * blayout_ctx, editor_layout_filter_context_t ** out)
{
    hex_mode_context_t * mode_ctx = new hex_mode_context_t;
    *out = mode_ctx;

    mode_ctx->blayout_ctx = blayout_ctx;
    filter_io_init(&mode_ctx->out);
    // TODO: nibble index
    return true;
}

bool hex_mode_filter(editor_layout_builder_context_t * blctx, editor_layout_filter_context_t * ctx,
                     layout_io_vec_t in_vec,
                     layout_io_vec_t out_vec)
{
//    hex_mode_context_t * hex_mode_ctx = static_cast<hex_mode_context_t *>(ctx);

    {
        static char hex_table[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

        if (layout_io_vec_size(in_vec) == 0)
            return true;

        auto & in = *layout_io_vec_first(in_vec);
        uint64_t offset = in.offset;

        layout_io_t out;
        filter_io_init(&out);

        /// update/remove ed_bufer -> raw_buffer
        if (in.end_of_pipe == true) {
            out.cp     = ' ';
            out.end_of_pipe = true;
            out.offset = offset;
        } else {

            uint8_t byte_val = in.byte_value;
            uint8_t msb = (byte_val >> 4);
            uint8_t lsb = (byte_val &  0x0f);

            out.cp     = hex_table[msb];
            out.valid  = true;
            out.offset = offset;
            layout_io_vec_push(out_vec, &out);

            out.cp     = hex_table[lsb];
            out.valid  = true;
            out.offset = offset;
            layout_io_vec_push(out_vec, &out);

            out.cp     = ' ';
            out.valid  = true;
            out.offset = offset;
            layout_io_vec_push(out_vec, &out);

        }
    }
    return true;
}

bool hex_mode_finish(editor_layout_builder_context_t * blctx, editor_layout_filter_context_t * ctx)
{
    hex_mode_context_t * modectx = static_cast<hex_mode_context_t *>(ctx);

    // add destructor ?
    delete modectx;
    return true;
}

editor_layout_filter_t hex_mode = {
    "hex_mode",
    hex_mode_init,
    hex_mode_filter,
    hex_mode_finish
};

} // ! namespace core

} // ! namespace eedit
