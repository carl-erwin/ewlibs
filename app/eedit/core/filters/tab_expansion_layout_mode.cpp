#include "../log.hpp"
#include "../text_layout.hpp"

namespace eedit
{
namespace core
{

struct tab_expansion_context_t : public filter_context_t {
	build_layout_context_t * blayout_ctx = nullptr; // to access the text buffer buffer
	size_t expansion;
};

bool tab_expansion_init(build_layout_context_t * blayout_ctx, filter_context_t ** out)
{
	tab_expansion_context_t * mode_ctx = new tab_expansion_context_t;
	*out = mode_ctx;

	mode_ctx->blayout_ctx = blayout_ctx;

	mode_ctx->expansion = 8; // from user config

	return true;
}

//static int32_t tab_first_cp = 0x2192;
static int32_t tab_first_cp = ' '; // 0x2192;

bool tab_expansion_filter(build_layout_context_t * blctx, filter_context_t * ctx_,
			  const filter_io_t * const in, const size_t nr_in,
			  filter_io_t * out, const size_t max_out, size_t * nr_out)
{
	tab_expansion_context_t * ctx = static_cast<tab_expansion_context_t *>(ctx_);

	size_t expansion = ctx->expansion;
	*nr_out = 0;


	for (size_t index = 0; index != nr_in; index++) {

		switch (in[index].cp) {
		// tab expansion
		case '\t': {

			u64 col = in[index].cp_index;

			u64 filled;
			if (in[index].split_count) {
				col = expansion - in[index].split_count;
				filled = in[index].split_count;
			} else {
				col = (in[index].cp_index % expansion);
				filled = (col % expansion);
			}

			if (0 /* mode_ctx.debug */) {
				app_log << __FUNCTION__ << " offset " << in[index].offset << ", real_cp = " <<  in[index].real_cp << ", cp_index = " <<  in[index].cp_index << " |TABS\n";
				app_log << __FUNCTION__ << " col = " << col << "\n";
			}

			auto filln = expansion - filled;

			//app_log << "filled = " << filled << "\n";
			//app_log << "filln = " << filln << "\n";

			for (size_t n = 0; n < filln; n++) {

				out[*nr_out].offset = in[index].offset;

				if (n == 0) {
					out[*nr_out].cp = tab_first_cp;
				} else {
					out[*nr_out].cp = ' ';
					out[*nr_out].cp = tab_first_cp;

				}

				out[*nr_out].is_selected = true; // TO DEBUG

				out[*nr_out].split_flag  = 1;
				out[*nr_out].split_count = 0 + n + filled;
				out[*nr_out].cp_index    = in[index].cp_index;

				// app_log << " out[" << *nr_out << "].cp_index = " <<  out[*nr_out].cp_index << ", split_count = " << out[*nr_out].split_count <<" |TABS\n";


				out[*nr_out].real_cp = '\t';
				out[*nr_out].valid = true;
				out[*nr_out].end_of_pipe = in[index].end_of_pipe;
				(*nr_out)++;
			}
		}
		break;

		default: {
			out[*nr_out] = in[index]; // exact copy
			assert(out[*nr_out].cp_index == in[index].cp_index);
			out[*nr_out].split_flag  = 0;
			out[*nr_out].split_count = 0;
			out[*nr_out].valid       = true;

			(*nr_out)++;
		}
		break;
		}

	}

	return true;
}

bool tab_expansion_finish(build_layout_context_t * blctx, filter_context_t * ctx)
{
	tab_expansion_context_t * tabctx = static_cast<tab_expansion_context_t *>(ctx);
	delete tabctx;
	return true;
}

filter_t tab_expansion_mode = {
	"tab_expansion",
	tab_expansion_init,
	tab_expansion_filter,
	tab_expansion_finish
};

} // ! namespace core
} // ! namespace eedit
