#pragma once

/* The editor event interface */

#include "editor_export.h"
#include "editor_types.h"
#include "editor_event_queue.h"
#include "editor_input_event.h"

#include "screen.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum editor_event_type_e {

    EDITOR_EVENT_TYPE_FAMILY_MASK       = 0xFF00,

    INVALID_EDITOR_EVENT                = 0,

    // ui -> core
    EDITOR_RESIZE_EVENT                 = 0x0100, //
    EDITOR_KEYBOARD_EVENT               = 0x0200, // on press events

    EDITOR_POINTER_BUTTON_EVENT_FAMILY  = 0x0300,
    EDITOR_POINTER_BUTTON_PRESS_EVENT   = EDITOR_POINTER_BUTTON_EVENT_FAMILY  | 0x1,
    EDITOR_POINTER_BUTTON_RELEASE_EVENT = EDITOR_POINTER_BUTTON_EVENT_FAMILY  | 0x2,
    EDITOR_POINTER_WHEEL_UP             = EDITOR_POINTER_BUTTON_EVENT_FAMILY  | 0x4,
    EDITOR_POINTER_WHEEL_DOWN           = EDITOR_POINTER_BUTTON_EVENT_FAMILY  | 0x8,

    EDITOR_POINTER_MOTION_EVENT         = 0x0600,

    EDITOR_RPC_CALL_EVENT               = 0x0700,

    // ... -> core
    EDITOR_BUILD_LAYOUT_EVENT           = 0x0800,

    // core -> ui
    EDITOR_LAYOUT_NOTIFICATION_EVENT    = 0x0900,
    EDITOR_RPC_ANSWER_EVENT             = 0x0A00,

    // ui <-> core (bidirectionnal)
    EDITOR_APPLICATION_EVENT_FAMILY     = 0x0B00,
    EDITOR_QUIT_APPLICATION_DEFAULT     = EDITOR_APPLICATION_EVENT_FAMILY | 0x1,
    EDITOR_QUIT_APPLICATION_FORCED      = EDITOR_APPLICATION_EVENT_FAMILY | 0x2,

    // system -> core
    EDITOR_SYSTEM_EVENT                 = 0x0F00,  // halt, reboot, suspend, resume ?
} editor_event_type_e;


typedef enum editor_actor_kind_e {
    EDITOR_ACTOR_INVALID_KIND,
    EDITOR_ACTOR_SYSTEM,
    EDITOR_ACTOR_CORE,
    EDITOR_ACTOR_UI
} editor_actor_kind_e;






///
struct editor_event_address_s {
    editor_actor_kind_e kind;     // EDITOR_ACTOR_INVALID_KIND;
    editor_event_queue_s * queue; // = nullptr;
};

struct editor_event_s {
    editor_event_type_e type; // INVALID_EDITOR_EVENT;

    uint32_t            id;   // used in eventual answer

    editor_event_address_s       src;
    editor_event_address_s       dst;

    editor_buffer_id_t  editor_buffer_id;
    byte_buffer_id_t    byte_buffer_id;
    editor_view_id_t    view_id;

    screen_dimension_t             screen_dim; // layout/input

    union {
        struct {
            screen_t * screen;
        } layout;

        struct {
            editor_input_event_s ev;
        } input;

        struct {
            int     ac;
            char ** av;
        } rpc;
    };
};

EDITOR_EXPORT
struct editor_event_s * editor_event_alloc();

EDITOR_EXPORT
struct editor_event_s * editor_layout_event_new(editor_event_type_e type);

EDITOR_EXPORT
struct editor_event_s * editor_rpc_call_new(int call_ac, const char ** call_av);

EDITOR_EXPORT
struct editor_event_s * editor_rpc_answer_new(struct editor_event_s * request, int answer_ac, const char ** anwser_av);

EDITOR_EXPORT
void editor_event_free(struct editor_event_s * ev); // CHECK PTR


#ifdef __cplusplus
} //
#endif
