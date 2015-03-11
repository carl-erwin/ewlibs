#pragma once
#include "ew/ew_config.hpp"

namespace ew
{
namespace graphics
{
namespace gui
{
namespace events
{

namespace keys
{

// todo: key_value -> key_type_value
enum key_value {
	NUL = 0,
	NO_KEY, /* empty event */
	Tab,
	Linefeed,
	Clear,
	Return,
	Pause,
	ScrollLock,
	SysReq,
	Escape,
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
	Keypad_Plus, // add other keys
	///
	UNICODE = 0xFF,
	LastKey,
	MaxKey = LastKey,
};

inline const char * c_string(const enum key_value val)
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

EW_GRAPHICS_EXPORT enum key_value c_string_to_key_value(const char * str);

} // ! namespace keys
} // ! namespace events
} // ! namespace gui
} // ! namespace graphics
} // ! namespace ew
