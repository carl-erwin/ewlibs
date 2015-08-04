#pragma once

#include <stddef.h>
#include <stdint.h>

#include "editor_export.h"
#include "byte_buffer.h"
#include "editor_codec.h"

#ifdef __cplusplus
extern "C" {
#endif


struct text_codec_io_ctx_s {
    struct codec_io_ctx_s parent_ctx; // parent ctx

    // user defined pointer to speedup reads/writes
    uint8_t * cache;
    size_t    cache_size;
};

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
    int (*read_forward)(struct text_codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);
    int (*read_backward)(struct text_codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);
    int (*write)(struct text_codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);

    // write_backward ?

    // one codepoint at time
    int (*encode)(int32_t codepoint, uint64_t out_size, uint8_t out[], uint64_t * nb_write);
    int (*decode)(uint64_t in_size, uint8_t in[], int32_t * codepoint);

    int     (*sync_codepoint)(struct text_codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset);
    int64_t (*sync_line)(struct text_codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset); // returns numbers of decoded codepoints
};


// these helpers should be used by modes
// returns the number of decoded iovc entries
int text_codec_read(struct text_codec_io_ctx_s * io_ctx, int direction, struct text_codec_io_s * iovc, size_t iocnt);
int text_codec_read_forward(struct text_codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);
// returns the number of decoded iovc entries
int text_codec_read_backward(struct text_codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);
// returns the number of encode iovc entries
int text_codec_write(struct text_codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt);

int text_codec_sync_codepoint(struct text_codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset);

// return the number of codepoint decoded to read *synced_offset
// < 0 on error
int64_t text_codec_sync_line(struct text_codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset);


// one codepoint at time
int text_codec_encode(codec_id_t codec, int32_t codepoint, uint64_t out_size, uint8_t out[], size_t * nb_write);
int text_codec_decode(codec_id_t codec, size_t in_size, uint8_t in[], int32_t * codepoint);



#ifdef __cplusplus
}
#endif
