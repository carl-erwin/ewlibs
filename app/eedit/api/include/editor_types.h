#pragma once

#include <stdint.h>

/*
 * editor objects are hidden behind uint64_t handles
 * the value 0 is used for invalid objects/no context
 */

typedef uint64_t byte_buffer_id_t;          // handle to low level "raw" buffer
typedef uint64_t editor_buffer_id_t;           // handle to internal representation of an editor bufer (byte_buffer + views + modes + ...)
typedef uint64_t editor_view_id_t;             // TODO: add doc

typedef uint64_t buffer_log_id_t;           // TODO: add doc
typedef uint64_t buffer_commit_rev_t;       // TODO: add doc

typedef uint64_t codec_id_t;                // TODO: add doc
typedef uint64_t codec_context_id_t;        // TODO: add doc

typedef uint64_t editor_font_t;             // just a hack to mask pointer type




#define INVALID_BYTE_BUFFER_ID   ((byte_buffer_id_t)0)
#define INVALID_EDITOR_BUFFER_ID ((editor_buffer_id_t)0)
#define INVALID_EDITOR_VIEW_ID   ((editor_view_id_t)0)
#define INVALID_CODEC_ID         ((codec_id_t)0)
#define INVALID_CODEC_CONTEXT_ID ((codec_context_id_t)0)

