#include <iostream>
#include <cstring>

#include "ew/ew_config.hpp"
#include "./api/include/module.h"
#include "./api/include/codec.h"

#include <cstdio>

/////////////////////////////
/// codec interface
/////////////////////////////


#include "../api/include/byte_buffer.h"
#include "../api/include/text_codec.h"



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
int ascii_read(struct codec_io_ctx_s * io_ctx, struct text_codec_io_s * iovc, size_t iocnt)
{
	// read up to iocnt codepont
	int64_t offset = iovc->offset;
	uint8_t buff[16 * 1024]; // codec ctx // move this to codec ctx // and adapt

	int i = 0;
	while (i < (int)iocnt) {

		size_t nb_read = 0;
		int res = byte_buffer_read(io_ctx->bid, offset,  buff,  sizeof (buff) - i, &nb_read);
		if (res != 0) {
			/* */
			return -1;
		}

		// decoded ?
		if (nb_read > iocnt) {
			nb_read = iocnt;
		}

		for (size_t j = 0; j < nb_read; j++) {


			iovc[i + j].cp = buff[j];
			iovc[i + j].offset = offset + j;
			iovc[i + j].size   = 1;
		}

		i      += nb_read;
		offset += nb_read;

		// always true
		if (nb_read < sizeof (buff)) {
			break;
		}
	}

	return i;
}

extern "C"
int ascii_reverse_read(struct codec_io_ctx_s * b, struct text_codec_io_s * iovc, size_t iocnt)
{

	return 0;
}

extern "C"
int ascii_write(struct codec_io_ctx_s * b, struct text_codec_io_s * iovc, size_t iocnt)
{
	return 0;
}

extern "C"
int ascii_sync_codepoint(struct codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset)
{
	return 0;
}

extern "C"
int ascii_sync_line(struct codec_io_ctx_s * io_ctx, const uint64_t offset, const int direction, uint64_t * synced_offset)
{
	return 0;
}


// one codepoint at time
extern "C"
int ascii_encode(int32_t codepoint, uint64_t out_size, uint8_t out[], size_t * nb_write)
{
	return 0;
}

extern "C"
int ascii_decode(size_t in_size, uint8_t in[], int32_t * codepoint)
{
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
SHOW_SYMBOL const char * module_depends()
{
	return "";
}

extern "C"
SHOW_SYMBOL eedit_module_type_e  module_type()
{
	return MODULE_TYPE_CODEC;
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
	ascii_ops.read         = ascii_read;
	ascii_ops.reverse_read = ascii_reverse_read;
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
