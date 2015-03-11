#include "../text_layout.hpp"

/* TODO: abstract codecs,
   update pipeline_info_t to take a data_type { bytes (octetstream)  , unicode (text), sound, video -> mime/type  }
   rename pipeline_info_t stream_info_t
*/

namespace eedit
{
namespace core
{

struct hex_mode_context_t : public filter_context_t {
	build_layout_context_t * blayout_ctx = nullptr; // to access the text buffer buffer
};

bool hex_mode_init(build_layout_context_t * blayout_ctx, filter_context_t ** out)
{
	hex_mode_context_t * mode_ctx = new hex_mode_context_t;
	*out = mode_ctx;

	mode_ctx->blayout_ctx = blayout_ctx;

	// TODO: nibble index
	return true;
}

bool hex_mode_filter(build_layout_context_t * blctx, filter_context_t * ctx,
		     const filter_io_t * const in, const size_t nr_in,
		     filter_io_t * out, const size_t max_out, size_t * nr_out)
{
//    hex_mode_context_t * hex_mode_ctx = static_cast<hex_mode_context_t *>(ctx);

	*nr_out = 0;
	size_t index = 0;
	{
		static char hex_table[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
		u64 offset = in[index].offset;
		/// update/remove ed_bufer -> raw_buffer
		if (in[index].end_of_pipe == true) {
			out[index].cp     = ' ';
			out[index].end_of_pipe = true;
			out[index].offset = offset;
			*nr_out = 1;
		} else {

			u8 byte_val = in[index].byte_value;
			u8 msb = (byte_val >> 4);
			u8 lsb = (byte_val &  0x0f);

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

bool hex_mode_finish(build_layout_context_t * blctx, filter_context_t * ctx)
{
	hex_mode_context_t * modectx = static_cast<hex_mode_context_t *>(ctx);

	// add destructor ?
	delete modectx;
	return true;
}

filter_t hex_mode = {
	"hex_mode",
	hex_mode_init,
	hex_mode_filter,
	hex_mode_finish
};

} // ! namespace core

} // ! namespace eedit
