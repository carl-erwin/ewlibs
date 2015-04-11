#include "../text_layout.hpp"

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
};

bool hex_mode_init(editor_layout_builder_context_t * blayout_ctx, editor_layout_filter_context_t ** out)
{
    hex_mode_context_t * mode_ctx = new hex_mode_context_t;
    *out = mode_ctx;

    mode_ctx->blayout_ctx = blayout_ctx;

    // TODO: nibble index
    return true;
}

bool hex_mode_filter(editor_layout_builder_context_t * blctx, editor_layout_filter_context_t * ctx,
                     const editor_layout_filter_io_t * const in, const size_t nr_in,
                     editor_layout_filter_io_t * out, const size_t max_out, size_t * nr_out)
{
//    hex_mode_context_t * hex_mode_ctx = static_cast<hex_mode_context_t *>(ctx);

    *nr_out = 0;
    size_t index = 0;
    {
        static char hex_table[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
        uint64_t offset = in[index].offset;
        /// update/remove ed_bufer -> raw_buffer
        if (in[index].end_of_pipe == true) {
            out[index].cp     = ' ';
            out[index].end_of_pipe = true;
            out[index].offset = offset;
            *nr_out = 1;
        } else {

            uint8_t byte_val = in[index].byte_value;
            uint8_t msb = (byte_val >> 4);
            uint8_t lsb = (byte_val &  0x0f);

            out[index + 0].cp     = hex_table[msb];
            out[index + 0].valid  = true;
            out[index + 0].offset = offset;

            out[index + 1].cp     = hex_table[lsb];
            out[index + 1].valid  = true;
            out[index + 2].offset = offset;

            out[index + 2].cp     = ' ';
            out[index + 2].valid  = true;
            out[index + 2].offset = offset;

            *nr_out = 3;


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
