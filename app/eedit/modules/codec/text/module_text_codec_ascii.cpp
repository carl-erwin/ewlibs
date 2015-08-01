#include <assert.h>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <alloca.h>

#include "ew/ew_config.hpp"
#include "editor_module.h"
#include "editor_codec.h"

#include <cstdio>

/////////////////////////////
/// codec interface
/////////////////////////////


#include "api/include/byte_buffer.h"
#include "api/include/text_codec.h"



// the codec is loaded/initialized by the core
extern "C"
int ascii_context_create(codec_context_id_t * ctx)
{
    *ctx = 0;
    return 0;
}

extern "C"
int ascii_context_destroy(codec_context_id_t * ctx)
{
    return 0;
}

extern "C"
enum editor_codec_type_e ascii_get_type()
{
    return EDITOR_TEXT_CODEC;
}

extern "C"
int ascii_get_name(char buffer[], size_t buffer_size)
{
    snprintf(buffer, buffer_size, "text/ascii");
    return 0;
}


extern "C"
int ascii_read_forward(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt)
{
    // read up to iocnt codepont
    int64_t offset = iovc->offset;
    size_t buff_size = iocnt;

    std::vector<uint8_t> buff;
    buff.reserve(iocnt);

    // std::cerr << __PRETTY_FUNCTION__ << "@ " << offset << " iocnt = " << iocnt << "\n";


    int i = 0;
    while (i < (int)iocnt) {

        size_t nb_read = 0;
        size_t to_read = std::min<size_t>(iocnt, buff_size);

        int res = byte_buffer_read(io_ctx->bid, offset,  &buff[0],  to_read, &nb_read);
        if (res != 0) {
            /* */
            return -1;
        }

        // decoded ?
        if (nb_read > iocnt) {
            nb_read = iocnt;
        }

        for (size_t j = 0; j < nb_read; j++) {

            iovc[i + j].cp = buff[j] > 127 ? 0xfffd: buff[j];
            iovc[i + j].offset = offset + j;
            iovc[i + j].size   = 1;
        }

        i      += nb_read;
        offset += nb_read;

        // always true
        if (nb_read <= buff_size) {
            // std::cerr << __PRETTY_FUNCTION__ << "nb_read("<< nb_read << ") <= buff_size\n";
            break;
        }
    }

    // std::cerr << __PRETTY_FUNCTION__ << " return total read = " << i << "\n";

    return i;
}


// "decode" up to iocnt codepoint in reverse order

/*
  last_offset(0)[ | | | | ]first_offset(eof)
*/

extern "C"
int ascii_read_backward(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt)
{
    // start of buffer ?
    if (iovc->offset == 0)
        return 0;

    uint64_t first_offset = iovc->offset;
    uint64_t last_offset;
    if (iocnt > first_offset) {
        last_offset = 0;
    } else {
        last_offset = first_offset - iocnt;
    }

    uint8_t buff[16 * 1024]; // codec ctx // move this to codec ctx // and adapt, cache ?

    // stop @ offset - iocv
    struct text_codec_io_s * dest_iovc = &iovc[0];
    while (first_offset != last_offset) {

        size_t max_read = std::min<size_t>(sizeof (buff), first_offset - last_offset);
        size_t nb_read  = 0;
        int res = byte_buffer_read(io_ctx->bid, (first_offset - max_read), buff, max_read, &nb_read);
        if (res != 0) {
            return -1;
        }

        for (size_t j = nb_read; j > 0; ) {
            --j;
            --first_offset;
            dest_iovc->cp     = buff[j];
            dest_iovc->offset = first_offset;
            dest_iovc->size   = 1;
            dest_iovc++;
        }
    }

    return int(dest_iovc - iovc);
}

extern "C"
int ascii_write(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt)
{
    auto start_offset = iovc[0].offset;
    std::vector<uint8_t> vec;

    // encode :-)
    for (size_t i = 0; i < iocnt; i++) {
        auto cp = iovc[i].cp;
        if (cp < 128) {
            vec.push_back(cp);
        } else {
            // cannot encode byte >= 128
        }
    }

    size_t nb_written = 0;
    byte_buffer_write(io_ctx->bid, start_offset, &vec[0], vec.size(), &nb_written);
    int res = nb_written;

    // fix size / offset
    for (size_t j = 0; j < nb_written; j++) {
        iovc[j].offset = start_offset + j;
        iovc[j].size   = 1;
    }

    return res;
}

extern "C"
int ascii_sync_codepoint(struct codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset)
{
    abort();
    return 0;
}


// TODO: improve forward direction
extern "C"
int64_t ascii_sync_line(struct codec_io_ctx_s * io_ctx, const uint64_t near_offset, const int direction, uint64_t * synced_offset)
{
    int64_t cp_count = 0;

    if (direction == 0)
        return -1;

    int32_t  prev_cp  = 0;
    uint64_t read_pos = near_offset;
    if (direction > 0) {

        while (true) {

            struct text_codec_io_s iovc;
            iovc.offset = read_pos;
            int ret = ascii_read_forward(io_ctx, &iovc, 1);
            if (ret < 0)
                return -1;

            if (ret == 0) {
                return cp_count;
            }


            ++cp_count;

            switch (prev_cp) {

            case '\r': {
                if (iovc.cp == '\n') {
                    *synced_offset = iovc.offset + 1;
                    return cp_count;
                }

                *synced_offset = iovc.offset;
                return cp_count;
                break;
            }

            case '\n': {
                if (iovc.cp == '\n') {
                    *synced_offset = iovc.offset;
                    return cp_count;
                }
                break;
            }

            }

            ////
            read_pos = iovc.offset + iovc.size;
            prev_cp  = iovc.cp;

        }

        return cp_count;
    }

    // direction < 0)
    // read_pos = near_offset;

    size_t READ_SIZE = 1024 * 1024;

    std::vector<text_codec_io_s> iovec;
    iovec.reserve(READ_SIZE);

    size_t loop = 0;
    size_t maxio = 1024;
    while (true) {

        ++loop;
        // std::cerr << __PRETTY_FUNCTION__ << " loop " << loop << "\n";


        // fetch
        uint64_t start;

        maxio += 1024 * (1 << loop);
        maxio = maxio > READ_SIZE ? READ_SIZE : maxio;
        // std::cerr << __PRETTY_FUNCTION__ << " maxio " << maxio << "\n";
        // std::cerr << __PRETTY_FUNCTION__ << " read_pos " << read_pos << "\n";

        if (read_pos <= maxio) {
            start = 0;
            maxio = read_pos;
        } else {
            start = read_pos - maxio;
        }

        if (0) {
            // std::cerr << __PRETTY_FUNCTION__ << " start " << start << "\n";
            // std::cerr << __PRETTY_FUNCTION__ << " read_pos " << read_pos << "\n";
            // std::cerr << __PRETTY_FUNCTION__ << " maxio " << maxio << "\n";
        }


        iovec[0].offset = start;
        int ret = ascii_read_forward(io_ctx, &iovec[0], maxio);
        if (ret < 0) {
            // FIXME:
            return -1;
        }

        if (ret == 0) {
            *synced_offset = 0;
            return cp_count;
        }

        // std::cerr << __PRETTY_FUNCTION__ << " ret = " << ret << "\n";

        read_pos -= ret;

        struct text_codec_io_s * iovc;

        for (iovc = &iovec[0] + ret; ; ) {

            --iovc;
            if (iovc < &iovec[0]) {
                break;
            }

            if (0) {
                // std::cerr << __PRETTY_FUNCTION__ << "iovc[" << cp_count << "]->cp = " << iovc->cp << "\n";
                // std::cerr << __PRETTY_FUNCTION__ << "iovc[" << cp_count << "]->offset = " << iovc->offset << "\n";
                // std::cerr << __PRETTY_FUNCTION__ << "iovc[" << cp_count << "]->size  = " << iovc->size << "\n";
                // std::cerr << __PRETTY_FUNCTION__ << " prev_cp  = " << prev_cp << "\n";
            }

            ++cp_count;

            switch (prev_cp) {

            case '\r':
            case '\n': {
                *synced_offset = iovc->offset + 1;
                --cp_count;
                return  cp_count;
                ;
            }
            break;

            }

            ////
            prev_cp  = iovc->cp;
        }


    }

    return 0;
}


// one cod	epoint at time
extern "C"
int ascii_encode(int32_t codepoint, uint64_t out_size, uint8_t out[], size_t * nb_write)
{
    abort();
    return 0;
}

extern "C"
int ascii_decode(size_t in_size, uint8_t in[], int32_t * codepoint)
{
    abort();
    return 0;
}


/////////////////////////////
/// module api
/////////////////////////////


static codec_id_t codec_id = 0;

extern "C"
SHOW_SYMBOL const char * module_name()
{
    return "text/ascii";
}


extern "C"
SHOW_SYMBOL const char * module_version()
{
    return "1.0.0";
}


extern "C"
SHOW_SYMBOL eedit_module_type_e  module_type()
{
    return MODULE_TYPE_CODEC;
}


extern "C"
SHOW_SYMBOL const char * module_depends()
{
    return "";
}



static struct text_codec_ops_s ascii_ops;



extern "C"
SHOW_SYMBOL eedit_module_init_status_e  module_init()
{
    //
    ::memset(&ascii_ops, 0, sizeof (ascii_ops));
    ascii_ops.base_ops.context_create  = ascii_context_create;
    ascii_ops.base_ops.context_destroy = ascii_context_destroy;
    ascii_ops.base_ops.get_name        = ascii_get_name;
    ascii_ops.base_ops.get_type        = ascii_get_type;
    //
    ascii_ops.read_forward         = ascii_read_forward;
    ascii_ops.read_backward = ascii_read_backward;
    ascii_ops.write        = ascii_write;

    ascii_ops.sync_codepoint = ascii_sync_codepoint;
    ascii_ops.sync_line      = ascii_sync_line;

    ascii_ops.encode       = ascii_encode;
    ascii_ops.decode       = ascii_decode;

    codec_id = codec_register(module_name(), EDITOR_TEXT_CODEC, (void *)&ascii_ops);
    if (codec_id != 0) {
        return MODULE_INIT_OK;
    }

    return MODULE_INIT_ERROR;
}

extern "C"
SHOW_SYMBOL int  module_quit()
{
    /* nothing to do */
    return 0;
}
