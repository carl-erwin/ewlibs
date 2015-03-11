#pragma once

#include <stddef.h>
#include <stdint.h>

#include "editor_types.h"

#include "byte_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * This is a simple api that should be used by the modules/plugins
 *
 * when "text-mode" is enabled
 * the buffer maintain a tuple
 *
 * { codec_id, text_codec_contex_id_t } that represent the active codec
 *
 * in future version it will be possible to switch text_codec on the fly:
 * special stream_info_t { stream_entity_type(u32) = CODEC_CHANGE, editor_codec_type_t(u32), codec_id_t id, codec_ctx_t } ?
 *
 */

enum editor_codec_type_e {
	EDITOR_INVALID_CODEC = 0,
	EDITOR_TEXT_CODEC,
	EDITOR_PICTURE_CODEC,
	EDITOR_VIDEO_CODEC,
	EDITOR_AUDIO_CODEC,
};



// this struct represent a charater whitin the codec
struct codec_io_ctx_s {
	editor_buffer_id_t editor_buffer_id;      // editor_buffer.h
	byte_buffer_id_t   bid;       // buffer id see byte_buffer.h
	codec_id_t         codec_id;
	codec_context_id_t codec_ctx; //
};


// this struct is embeded in all derived codecs type
struct codec_ops_s {
	int                      (*context_create)(codec_context_id_t  * ctx);
	int                      (*context_destroy)(codec_context_id_t * ctx);
	enum editor_codec_type_e (*get_type)();
	int                      (*get_name)(char buffer[], size_t buffer_size);
};


// codec registration
// when a codec is initialized it must call
codec_id_t codec_register(const char * name, enum editor_codec_type_e type, void * codec_ops);
codec_id_t codec_get_by_name(const char * name);

enum editor_codec_type_e codec_get_type(codec_id_t id);
void * codec_get_ops(codec_id_t id);



#ifdef __cplusplus
}
#endif
