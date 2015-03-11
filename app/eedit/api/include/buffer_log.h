#pragma once

/*
 * The buffer log can be used to records all the modifications of a given byte_buffer
 *
 * FIXME:
 *      it will be used by the default text-mode :
 *      undo/redo
 *      undo-until (commit/offset/line)
 *      redo-until (commit/offset/line)
 *
 * TODO:
 *     it will be useful to record {enter,leave}-{line,statement,block,paragraph,file,...} events
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "editor_types.h"
#include "byte_buffer.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef enum buffer_log_operation_e {
	buffer_log_nop = -1,
	buffer_log_insert_op,
	buffer_log_remove_op,
} buffer_log_operation_t;

typedef struct buffer_log_commit_data_s {
	buffer_log_operation_t op;
	uint64_t               offset;
	const uint8_t *        data;
	size_t                 size;
} buffer_log_commit_data_t;




int buffer_log_init(const byte_buffer_id_t, buffer_log_id_t * log);
int buffer_log_reset(buffer_log_id_t log);
int buffer_log_destroy(buffer_log_id_t log);

int buffer_log_dump(buffer_log_id_t id);

int buffer_log_insert(buffer_log_id_t log, uint64_t offset, const uint8_t * data, size_t size, buffer_commit_rev_t * rev);
int buffer_log_remove(buffer_log_id_t log, uint64_t offset, const uint8_t * data, size_t size, buffer_commit_rev_t * rev);

int buffer_log_get_last_commit(buffer_log_id_t    log, buffer_commit_rev_t * rev, buffer_log_commit_data_t * commit_data);
int buffer_log_get_current_commit(buffer_log_id_t log, buffer_commit_rev_t * rev, buffer_log_commit_data_t * commit_data);
int buffer_log_get_commit_info(buffer_log_id_t    log, buffer_commit_rev_t * rev, buffer_log_commit_data_t * commit_data);

int buffer_log_undo_until(buffer_log_id_t log, buffer_commit_rev_t rev);
int buffer_log_redo_until(buffer_log_id_t log, buffer_commit_rev_t rev);

// move head pointer
int buffer_log_undo(buffer_log_id_t log);
int buffer_log_redo(buffer_log_id_t log);

#ifdef __cplusplus
}
#endif
