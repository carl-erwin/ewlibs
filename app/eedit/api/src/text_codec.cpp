#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "ew/ew_config.hpp"

#include "../include/codec.h"
#include "../include/text_codec.h"


extern "C"
SHOW_SYMBOL int text_codec_read(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt)
{
	text_codec_ops_s * ops = static_cast<text_codec_ops_s *>(codec_get_ops(io_ctx->codec_id));
	if (ops == nullptr)
		return -1;

	return ops->read(io_ctx, iovc, iocnt);
}

extern "C"
SHOW_SYMBOL int text_codec_reverse_read(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt)
{
	text_codec_ops_s * ops = static_cast<text_codec_ops_s *>(codec_get_ops(io_ctx->codec_id));
	if (ops == nullptr)
		return -1;

	return ops->reverse_read(io_ctx, iovc, iocnt);
}

extern "C"
SHOW_SYMBOL int text_codec_write(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt)
{
	text_codec_ops_s * ops = static_cast<text_codec_ops_s *>(codec_get_ops(io_ctx->codec_id));
	if (ops == nullptr)
		return -1;

	return ops->write(io_ctx, iovc, iocnt);
}

extern "C"
SHOW_SYMBOL int text_codec_sync_codepoint(struct codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset)
{
	text_codec_ops_s * ops = static_cast<text_codec_ops_s *>(codec_get_ops(io_ctx->codec_id));
	if (ops == nullptr)
		return -1;

	return ops->sync_codepoint(io_ctx, offset, direction, synced_offset);
}

extern "C"
SHOW_SYMBOL int text_codec_sync_line(struct codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset)
{
	text_codec_ops_s * ops = static_cast<text_codec_ops_s *>(codec_get_ops(io_ctx->codec_id));
	if (ops == nullptr)
		return -1;

	return ops->sync_line(io_ctx, offset, direction, synced_offset);
}



// one codepoint at time
extern "C"
SHOW_SYMBOL int text_codec_encode(codec_id_t codec_id, int32_t codepoint, uint64_t out_size, uint8_t out[], size_t * nb_write)
{
	text_codec_ops_s * ops = static_cast<text_codec_ops_s *>(codec_get_ops(codec_id));
	if (ops == nullptr)
		return -1;

	return ops->encode(codepoint, out_size, out, nb_write);
}

extern "C"
SHOW_SYMBOL int text_codec_decode(codec_id_t codec_id, size_t in_size, uint8_t in[], int32_t * codepoint)
{
	text_codec_ops_s * ops = static_cast<text_codec_ops_s *>(codec_get_ops(codec_id));
	if (ops == nullptr)
		return -1;

	return ops->decode(in_size, in, codepoint);
}
