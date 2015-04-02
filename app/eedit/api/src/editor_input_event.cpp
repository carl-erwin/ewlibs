#include "editor_input_event.h"

struct editor_input_event_s *
editor_input_event_new(editor_input_event_type_e type,
                       editor_key_e key, editor_input_key_range_e rt,
                       u32 mod_mask,
                       u32 key_start_val, u32 key_end_val,
                       int32_t x, int32_t y, u32 button_mask);
{
    auto ev = new editor_input_event_s;
    ::memset(ev, 0, sizeof (editor_input_event_s));

    type = keypress;

    // filter special keys here
    switch (kt) {
    case Tab: {
        kt = UNICODE;
        key_start_val = '\t';
        key_end_val   = '\t';
    }
    break;
    default: {
        /* TODO: handle other keys */
    } break;
    }

    ev->key = key;
    ev->is_range    = range == simple_range ? true : false;
    ev->start_value = key_start_val;
    ev->end_value   = key_end_val;
    ev->ctrl        = mod_mask & mod_ctrl;
    ev->altL        = mod_mask & mod_altL;
    if (key != UNICODE) {
        ev->altR    = mod_mask & mod_altR;
    }
    ev->oskey       = mod_mask & mod_oskey;

    ev->x = x;
    ev->y = y;
    ev->button_mask = button_mask;

    return ev;
}

void editor_input_event_delete(struct editor_input_event_s * ev)
{
    delete ev;
}

#define SWITCH_CASE_STRINGIFY(X)   case X: return #X ; break

const char * editor_input_event_type_to_string(editor_key_e key)
{
    switch (key) {
    case NUL:
        return "nul";

    case UNICODE:
        return "unicode";

        SWITCH_CASE_STRINGIFY(Tab);
        SWITCH_CASE_STRINGIFY(Linefeed);
        SWITCH_CASE_STRINGIFY(Clear);
        SWITCH_CASE_STRINGIFY(Return);
        SWITCH_CASE_STRINGIFY(Pause);
        SWITCH_CASE_STRINGIFY(ScrollLock);
        SWITCH_CASE_STRINGIFY(SysReq);
        SWITCH_CASE_STRINGIFY(Escape);
        SWITCH_CASE_STRINGIFY(Delete);
        SWITCH_CASE_STRINGIFY(BackSpace);
        SWITCH_CASE_STRINGIFY(Home);
        SWITCH_CASE_STRINGIFY(Left);
        SWITCH_CASE_STRINGIFY(Up);
        SWITCH_CASE_STRINGIFY(Right);
        SWITCH_CASE_STRINGIFY(Down);
        SWITCH_CASE_STRINGIFY(PageUp);
        SWITCH_CASE_STRINGIFY(PageDown);
        SWITCH_CASE_STRINGIFY(End);
        SWITCH_CASE_STRINGIFY(Begin);
        SWITCH_CASE_STRINGIFY(F1);
        SWITCH_CASE_STRINGIFY(F2);
        SWITCH_CASE_STRINGIFY(F3);
        SWITCH_CASE_STRINGIFY(F4);
        SWITCH_CASE_STRINGIFY(F5);
        SWITCH_CASE_STRINGIFY(F6);
        SWITCH_CASE_STRINGIFY(F7);
        SWITCH_CASE_STRINGIFY(F8);
        SWITCH_CASE_STRINGIFY(F9);
        SWITCH_CASE_STRINGIFY(F10);
        SWITCH_CASE_STRINGIFY(F11);
        SWITCH_CASE_STRINGIFY(F12);
        SWITCH_CASE_STRINGIFY(Keypad_Plus);             // add other keys

    default:
        return "unhandled";
    }
    return "";
}



void editor_input_event_dump(struct editor_input_event_s * ev)
{
    app_log << "self(" << ev << ") ";

    if (ev->ctrl) app_log << "ctrl + ";
    if (ev->altL) app_log << "alt + ";
    if (ev->altR) app_log << "altR + ";
    if (ev->oskey) app_log << "oskey + ";
    if (ev->is_range) {
        app_log << "[ ";
    }

    app_log << "type(" << ev->key << ") ";
    app_log << "type_str(" << ev->type_to_string(ev->key) << ") ";

    app_log << "start_value(" << ev->start_value << ") ";
    if (ev->is_range) {
        app_log << "<=> end_value(" << ev->end_value << ") ]";
    }

    if (ev->button_mask) {
        app_log << "button_mask(" << std::hex << ev->button_mask << ")" << std::dec;
    }

}


inline  bool editor_input_event_contains(const editor_input_event_s & a, const editor_input_event_s & b) const
{

    if (*this == b) return true;

    switch (type) {
    case editor_input_event_s::keypress: {

        if (key        != b.key)          return false;
        if (is_range    == false)         return false;
        if (start_value >  b.start_value) return false;
        if (end_value   <  b.end_value)   return false;
        if (ctrl        != b.ctrl)        return false;
        if (altL        != b.altL)        return false;
        if (altR        != b.altR)        return false;
        if (oskey       != b.oskey)       return false;

        return true;

    }
    break;

    case editor_input_event_s::button_press:
    case editor_input_event_s::button_release: {
        return false;
    }
    break;

    default:
        return false;
    }

    return false;
}

bool editor_input_event_is_equal(const editor_input_event_s * a, const editor_input_event_s * b)
{
    if (type != b.type)          return false;

    switch (type) {
    case editor_input_event_s::keypress: {

        if (key         != b.key)        return false;
        if (is_range    != b.is_range)    return false;
        if (start_value != b.start_value) return false;
        if (end_value   != b.end_value)   return false;
        if (ctrl        != b.ctrl)        return false;
        if (altL        != b.altL)        return false;
        if (altR        != b.altR)        return false;
        if (oskey       != b.oskey)       return false;

        return true;
    }
    break;

    case editor_input_event_s::button_press:
    case editor_input_event_s::button_release: {
        // check keyborad ctrl/shift etc ... if so mer the if test

        if (button_mask != b.button_mask)
            return false;

        return true;

    }
    break;

    default:
        return false;
    }
    return false;
}

bool editor_input_event_is_not_equal(const editor_input_event_s * a, const editor_input_event_s * b)
{
    return !this->operator == (b);
}
