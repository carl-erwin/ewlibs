#pragma once

#include <stddef.h>
#include <stdint.h>

#include "byte_buffer.h"
#include "codec.h"

#ifdef __cplusplus
extern "C" {
#endif



// this struct represent a character whitin the codec
struct text_codec_io_s {
	uint64_t  offset; // write only ?
	int32_t   cp;
	uint32_t  size;
};


// MUST be passed in codec_register()
struct text_codec_ops_s {
	struct codec_ops_s base_ops;

	// text specific ops
	int (*read)(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);
	int (*reverse_read)(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);
	int (*write)(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);
	// one codepoint at time
	int (*encode)(int32_t codepoint, uint64_t out_size, uint8_t out[], uint64_t * nb_write);
	int (*decode)(uint64_t in_size, uint8_t in[], int32_t * codepoint);

	int (*sync_codepoint)(struct codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset);
	int (*sync_line)(struct codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset);
};


// these helpers should be used by modes
// returns the number of decoded iovc entries
int text_codec_read(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);
// returns the number of decoded iovc entries
int text_codec_reverse_read(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);
// returns the number of encode iovc entries
int text_codec_write(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);

int text_codec_sync_codepoint(struct codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset);
int text_codec_sync_line(struct codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset);


// one codepoint at time
int text_codec_encode(codec_id_t codec, int32_t codepoint, uint64_t out_size, uint8_t out[], size_t * nb_write);
int text_codec_decode(codec_id_t codec, size_t in_size, uint8_t in[], int32_t * codepoint);



#ifdef __cplusplus
}
#endif
