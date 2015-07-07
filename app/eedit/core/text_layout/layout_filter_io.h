#pragma once

#include <stdint.h>

#include "editor_codec.h"
#include "codepoint_info.h"

extern "C" {

    struct layout_io_s {

        uint32_t content_type;

        // general info
        uint8_t valid;
        uint8_t end_of_pipe; // skip
        uint8_t quit; // close pipeline
        uint8_t is_selected;

        uint64_t  offset;

        union {
            // content_type == bytes
            struct {
                uint8_t   byte_value;
            };

            // content_type == unicode
            struct {
                int32_t   cp;
                int32_t   real_cp;
                uint64_t  cp_index; // be carefull used const uint64_t invalid_cp_index
                uint32_t  split_flag;
                uint32_t  split_count;
            };

            // codec_change
            struct {
                codec_id_t         codec_id;
                codec_context_id_t codec_ctx;
            };
        };

        // TODO: add style infos ?
    };

    typedef struct layout_io_s layout_io_t;

}
