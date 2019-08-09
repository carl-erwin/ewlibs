#pragma once

#include <stdint.h>

#include "editor_export.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    invalid_input_event,
    keypress,
    button_press,
    button_release,
    wheel_up,
    wheel_down
} editor_input_event_type_e;

// FIXME: align some key with their unicode value
typedef enum  {
    NO_KEY = -1, /* empty event */
    NUL = 0,

    Tab = '\t', // move to unicode ?
    Linefeed,
    Clear,
    Return, // '\n' ?
    Pause,
    ScrollLock,
    SysReq,
    Escape = 127,
    Delete,
    BackSpace,
    Home,
    Left,
    Up,
    Right,
    Down,
    PageUp,
    PageDown,
    End,
    Begin,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    ///
    Keypad_Plus, // FIXME: add other keys
    ///
    UNICODE = 0xFF,
    LastKey,
    MaxKey = LastKey,

} editor_key_e;



typedef enum  {
    no_range,
    simple_range
} editor_input_key_range_e;

typedef enum {
    mod_ctrl   = (1 << 0),
    mod_shift  = (1 << 1),
    mod_altL   = (1 << 2),
    mod_altR   = (1 << 3),
    mod_oskey  = (1 << 4),
} editor_input_event_mod_mask_e;

typedef struct  editor_input_event_s {
    editor_input_event_type_e type = invalid_input_event;
    uint32_t     device_index = 0; // no used yet
    editor_key_e key = NO_KEY;
    bool         is_range = false;
    uint32_t     start_value = 0;
    uint32_t     end_value = 0;
    bool         ctrl = false;
    bool         shift = false;
    bool         altL = false;
    bool         altR = false;
    bool         oskey = false; // "windows key"

    uint32_t button_press_mask  = 0; // button_press mask 32 buttons :-)
    int32_t  x = -1; //! pointer horizontal coordinate at time of event
    int32_t  y = -1; //! pointer vertical coordinate at time of event

} editor_input_event_s;



EDITOR_EXPORT
struct editor_input_event_s *
editor_input_event_new(editor_input_event_type_e type,
                       editor_key_e key, editor_input_key_range_e rt,
                       uint32_t mod_mask,
                       uint32_t key_start_val, uint32_t key_end_val,
                       int32_t x, int32_t y, uint32_t button_mask);

EDITOR_EXPORT
void editor_input_event_delete(struct editor_input_event_s * ev);

EDITOR_EXPORT
void editor_input_event_dump(const struct editor_input_event_s * ev, const char * name);

EDITOR_EXPORT
bool editor_input_event_contains(const editor_input_event_s * a, const editor_input_event_s * b);

EDITOR_EXPORT
bool editor_input_event_is_equal(const editor_input_event_s * a, const editor_input_event_s * b);

EDITOR_EXPORT
bool editor_input_event_is_not_equal(const editor_input_event_s * a, const editor_input_event_s * b);

EDITOR_EXPORT
editor_key_e c_string_to_editor_key_value(const char * c_str);

#ifdef __cplusplus
}
#endif
