#include "core/log/log.hpp"
#include "../text_layout/text_layout.hpp"

namespace eedit
{
namespace core
{

struct tab_expansion_context_t : public editor_layout_filter_context_t {
    editor_layout_builder_context_t * blayout_ctx = nullptr; // to access the text buffer buffer
    size_t expansion;
};

bool tab_expansion_init(editor_layout_builder_context_t * blayout_ctx, editor_layout_filter_context_t ** out)
{
    tab_expansion_context_t * mode_ctx = new tab_expansion_context_t;
    *out = mode_ctx;

    mode_ctx->blayout_ctx = blayout_ctx;
    mode_ctx->expansion = 8; // from user config

    return true;
}

//static int32_t tab_first_cp = 0x2192;
static int32_t tab_first_cp = ' '; // 0x2192;

bool tab_expansion_filter(editor_layout_builder_context_t * blctx, editor_layout_filter_context_t * ctx_,
                          layout_io_vec_t in_vec,
                          layout_io_vec_t out_vec)
{
    tab_expansion_context_t * ctx = static_cast<tab_expansion_context_t *>(ctx_);

    size_t expansion = ctx->expansion;


    size_t nr_in = layout_io_vec_size(in_vec);

    for (size_t index = 0; index != nr_in; index++) {

        layout_io_t in;
        layout_io_vec_get(in_vec, &in);

        switch (in.cp) {
        // tab expansion
        case '\t': {

#if 0
            if (0 /* mode_ctx.debug */) {
                app_log << __FUNCTION__ << " offset " << in.offset << ", real_cp = " <<  in.real_cp << ", cp_index = " <<  in.cp_index << " split_count " << in.split_count << " |TABS\n";
            }
#endif

            uint64_t col = in.cp_index;

            if (in.cp_index == uint64_t(-1)) {
                abort();
            }

            uint64_t filled;
            if (in.split_count) {
                col = expansion - in.split_count;
                filled = in.split_count;
            } else {
                col = (in.cp_index % expansion);
                filled = (col % expansion);
            }


            auto filln = expansion - filled;

            // app_log << "filled = " << filled << "\n";
            // app_log << "filln = " << filln << "\n";

            for (size_t n = 0; n < filln; n++) {

                layout_io_t out;
                out.offset = in.offset;

                if (n == 0) {
                    out.cp = tab_first_cp;
                } else {
                    out.cp = ' ';
                    out.cp = tab_first_cp;
                }

                out.is_selected = true; // TO DEBUG

                out.split_flag  = 1;
                out.split_count = 0 + n + filled;
                out.cp_index    = in.cp_index;

                // app_log << " out[" << n << "].cp_index = " <<  out.cp_index << ", split_count = " << out.split_count <<" |TABS\n";

                out.real_cp = '\t';
                out.valid = true;
                out.end_of_pipe = in.end_of_pipe;

                layout_io_vec_push(out_vec, &out);
            }
        }
        break;

        default: {
            layout_io_t out = in; // exact copy
            assert(out.cp_index == in.cp_index);
            out.split_flag  = 0;
            out.split_count = 0;
            out.valid       = true;

            layout_io_vec_push(out_vec, &out);

        }
        break;
        }

    }

    return true;
}

bool tab_expansion_finish(editor_layout_builder_context_t * blctx, editor_layout_filter_context_t * ctx)
{
    tab_expansion_context_t * tabctx = static_cast<tab_expansion_context_t *>(ctx);
    delete tabctx;
    return true;
}

editor_layout_filter_t tab_expansion_mode = {
    "tab_expansion",
    tab_expansion_init,
    tab_expansion_filter,
    tab_expansion_finish
};

} // ! namespace core
} // ! namespace eedit
