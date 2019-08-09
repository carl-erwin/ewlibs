#include <cstring>
#include <map>

#include "editor_input_event.h"

#include "core/log/log.hpp"


std::map<std::string, editor_key_e> str_to_keyval_map;
static bool was_init = false;


/* not used yet */
struct editor_input_event_s *
editor_input_event_new(editor_input_event_type_e type,
                       editor_key_e key, editor_input_key_range_e rt,
                       uint32_t mod_mask,
                       uint32_t key_start_val, uint32_t key_end_val,
                       int32_t x, int32_t y, uint32_t button_mask)
{
    auto ev = new editor_input_event_s;

    ev->type = type;

    // filter special keys here
    switch (key) {
    case Tab: {
        ev->key = UNICODE;
        key_start_val = '\t';
        key_end_val   = key_start_val;
    }
    break;
    default: {
        /* TODO: handle other keys */
    } break;
    }

    ev->key = key;
    ev->is_range    = rt == simple_range ? true : false;
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
    ev->button_press_mask = button_mask;

    return ev;
}

void editor_input_event_delete(struct editor_input_event_s * ev)
{
    delete ev;
}

#define SWITCH_CASE_STRINGIFY(X)   case X: return #X ; break

const char * editor_input_event_key_to_string(editor_key_e key)
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



void editor_input_event_dump(const struct editor_input_event_s * ev, const char * name)
{
    app_log(-1, "ev name (%s) ", name);
    app_log(-1, "type(%u) ", ev->type);

    if (ev->ctrl) app_log(-1, "ctrl + ");
    if (ev->altL) app_log(-1, "alt + ");
    if (ev->altR) app_log(-1, "altR + ");
    if (ev->oskey) app_log(-1, "oskey + ");
    if (ev->is_range) {
        app_log(-1, "[ ");
    }

    app_log(-1, "key(%s) ", editor_input_event_key_to_string(ev->key));

    if (ev->is_range) {
        app_log(-1, "start_value(%u) <=> end_value(%u) ]", ev->start_value, ev->end_value);
        app_log(-1, " ]");
    } else {
        app_log(-1, "unicode_value(%u), char('%c')", ev->start_value,(char)ev->start_value);
    }

    if (ev->button_press_mask) {
        app_log(-1, "button_press_mask(%x)", ev->button_press_mask);
    }

    app_log(-1, "\n");
}


bool editor_input_event_contains(const editor_input_event_s * a, const editor_input_event_s * b)
{
    if (a == b) return true;

    if (a->type != b->type)          return false;


    switch (a->type) {
    case keypress: {

        if (a->key         != b->key)        return false;
        if (a->is_range    == false)         return false;
        if (a->start_value >  b->start_value)return false;
        if (a->end_value   <  b->end_value)  return false;
        if (a->ctrl        != b->ctrl)       return false;
        if (a->altL        != b->altL)       return false;
        if (a->altR        != b->altR)       return false;
        if (a->oskey       != b->oskey)      return false;

        return true;

    }
    break;

    case button_press:
    case button_release: {
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
    if (a == b) return true;

    if (a->type != b->type)          return false;

    switch (a->type) {
    case keypress: {

        if (a->key         != b->key)         return false;
        if (a->is_range    != b->is_range)    return false;
        if (a->start_value != b->start_value) return false;

        if ((a->is_range ) && (a->end_value != b->end_value)) return false;

        if (a->ctrl        != b->ctrl)        {
            return false;
        }
        if (a->altL        != b->altL)        {
            return false;
        }
        if (a->altR        != b->altR)        {
            return false;
        }
        if (a->oskey       != b->oskey)       {
            return false;
        }

        return true;
    }
    break;

    case button_press:
    case button_release: {
        // check keyborad ctrl/shift etc ...

        if (a->button_press_mask != b->button_press_mask)
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

    return !editor_input_event_is_equal(a, b);
}


inline const char * c_string(const editor_key_e val)
{
    switch (val) {
    case  NUL:
        return "NUL"; // ?
    case  Tab:
        return "Tab";
    case  Linefeed:
        return "Linefeed";
    case  Clear:
        return "Clear";
    case  Return:
        return "Return";
    case  Pause:
        return "Pause";
    case  ScrollLock:
        return "ScrollLock";
    case  SysReq:
        return "SysReq";
    case  Escape:
        return "Escape";
    case  Delete:
        return "Delete";
    case  BackSpace:
        return "BackSpace";
    case  Home:
        return "Home";
    case  Left:
        return "Left";
    case  Up:
        return "Up";
    case  Right:
        return "Right";
    case  Down:
        return "Down";
    case  PageUp:
        return "PageUp";
    case  PageDown:
        return "PageDown";
    case  End:
        return "End";
    case  Begin:
        return "Begin";
    case  F1:
        return "F1";
    case  F2:
        return "F2";
    case  F3:
        return "F3";
    case  F4:
        return "F4";
    case  F5:
        return "F5";
    case  F6:
        return "F6";
    case  F7:
        return "F7";
    case  F8:
        return "F8";
    case  F9:
        return "F9";
    case  F10:
        return "F10";
    case  F11:
        return "F11";
    case  F12:
        return "F12";

    case  Keypad_Plus:
        return "Keypad_Plus";

    case  UNICODE:
        return "UNICODE";
    default:
        return "";
    }

    return "";
}



bool init_str_keyval_map()
{
    if (was_init == true)
        return true;

    for (int i = NUL; i < MaxKey; i++) {
        char buff[32];

        auto s = c_string(editor_key_e(i));

        int slen = ::strlen(s);
        if (slen == 0)
            continue;

        for (int i = 0; i < slen; i++) {
            buff[i] = ::tolower(s[i]);
        }
        buff[slen] = '\0';

        std::string str(buff);
        str_to_keyval_map.insert(std::pair<std::string, editor_key_e>(str, editor_key_e(i)));
    }

    was_init = true;
    return true;
}


editor_key_e c_string_to_editor_key_value(const char* c_str)
{
    init_str_keyval_map();

    editor_key_e val = NUL;

    auto end = str_to_keyval_map.end();

    auto str = std::string(c_str);
    auto it = str_to_keyval_map.find(str);

    if (it != end) {
        val = it->second;
    }

    return val;
}

