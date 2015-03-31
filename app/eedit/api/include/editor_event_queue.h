#pragma once

#include "editor_export.h"

#ifdef __cplusplus
extern "C" {
#endif

struct editor_event_s;
struct editor_event_queue_s;

EDITOR_EXPORT
struct editor_event_queue_s * editor_event_queue_new(void);

EDITOR_EXPORT
void editor_event_queue_delete(struct editor_event_queue_s * q);

EDITOR_EXPORT
size_t editor_event_queue_size(struct editor_event_queue_s * q);

EDITOR_EXPORT
bool editor_event_queue_push(struct editor_event_queue_s * q, struct editor_event_s *ev);

EDITOR_EXPORT
size_t editor_event_queue_wait(struct editor_event_queue_s * q, size_t wait_time);

EDITOR_EXPORT
struct editor_event_s * editor_event_queue_get(struct editor_event_queue_s * q);


#ifdef __cplusplus
}
#endif
