#include "common.hpp"

namespace ew
{
namespace graphics
{
namespace gui
{

/* keyboard handling: x11/x11_keyboard.cpp */

enum events::keys::key_value  display::private_data::X11::xkeysym_to_event_key(const KeySym __keysym)
{
	std::cerr << __FUNCTION__ << " : keysym = '" << XKeysymToString(__keysym) << "'\n";
	switch (__keysym) {
	case XK_Tab:
		return events::keys::Tab;
	case XK_Linefeed:
		return events::keys::Linefeed;
	case XK_Clear:
		return events::keys::Clear;
	case XK_Return:
		return events::keys::Return;
	case XK_Pause:
		return events::keys::Pause;
	case XK_Scroll_Lock:
		return events::keys::ScrollLock;
	case XK_Sys_Req:
		return events::keys::SysReq;
	case XK_Escape:
		return events::keys::Escape;
	case XK_Delete:
		return events::keys::Delete;
	case XK_BackSpace:
		return events::keys::BackSpace;
	case XK_Home:
		return events::keys::Home;
	case XK_Left:
		return events::keys::Left;
	case XK_Up:
		return events::keys::Up;
	case XK_Right:
		return events::keys::Right;
	case XK_Down:
		return events::keys::Down;
	case XK_Page_Up:
		return events::keys::PageUp;
	case XK_Page_Down:
		return events::keys::PageDown;
	case XK_End:
		return events::keys::End;
	case XK_Begin:
		return events::keys::Begin;

	// keypad
	case XK_KP_Space:
	case XK_KP_Tab:
	case XK_KP_Enter:
	case XK_KP_F1:
	case XK_KP_F2:
	case XK_KP_F3:
	case XK_KP_F4:
	case XK_KP_Home:
	case XK_KP_Left:
	case XK_KP_Up:
	case XK_KP_Right:
	case XK_KP_Down:
	//case XK_KP_Prior:
	case XK_KP_Page_Up:
	//case XK_KP_Next:
	case XK_KP_Page_Down:
	case XK_KP_End:
	case XK_KP_Begin:
	case XK_KP_Insert:
	case XK_KP_Delete:
	case XK_KP_Equal:
	case XK_KP_Multiply:
	case XK_KP_Add:
		return events::keys::Keypad_Plus;

	case XK_KP_Separator:
	case XK_KP_Subtract:
	case XK_KP_Decimal:
	case XK_KP_Divide:
	// keypad
	case XK_KP_0:
	case XK_KP_1:
	case XK_KP_2:
	case XK_KP_3:
	case XK_KP_4:
	case XK_KP_5:
	case XK_KP_6:
	case XK_KP_7:
	case XK_KP_8:
	case XK_KP_9:
	case XK_F1:
		return events::keys::F1;
	case XK_F2:
		return events::keys::F2;
	case XK_F3:
		return events::keys::F3;
	case XK_F4:
		return events::keys::F4;
	case XK_F5:
		return events::keys::F5;
	case XK_F6:
		return events::keys::F6;
	case XK_F7:
		return events::keys::F7;
	case XK_F8:
		return events::keys::F8;
	case XK_F9:
		return events::keys::F9;
	case XK_F10:
		return events::keys::F10;
	case XK_F11:
		return events::keys::F11;
	//case XK_L1:
	case XK_F12:
		return events::keys::F12;
	//case XK_L2:
	case XK_F13:
	//case XK_L3:
	case XK_F14:
	//case XK_L4:
	case XK_F15:
	//case XK_L5:
	case XK_F16:
	//case XK_L6:
	case XK_F17:
	//case XK_L7:
	case XK_F18:
	//case XK_L8:
	case XK_F19:
	//case XK_L9:
	case XK_F20:
	//case XK_L10:
	case XK_F21:
	//case XK_R1:
	case XK_F22:
	//case XK_R2:
	case XK_F23:
	//case XK_R3:
	case XK_F24:
	//case XK_R4:
	case XK_F25:
	//case XK_R5:
	case XK_F26:
	//case XK_R6:
	case XK_F27:
	//case XK_R7:
	case XK_F28:
	//case XK_R8:
	case XK_F29:
	//case XK_R9:
	case XK_F30:
	//case XK_R10:
	case XK_F31:
	//case XK_R11:
	case XK_F32:
	//case XK_R12:
	case XK_F33:
	//case XK_R13:
	case XK_F34:
	//case XK_R14:
	case XK_F35:
	//case XK_R15:
	case XK_Shift_L:
	case XK_Shift_R:
	case XK_Control_L:
	case XK_Control_R:
	case XK_Caps_Lock:
	case XK_Shift_Lock:
	case XK_Meta_L:
	case XK_Meta_R:
	case XK_Alt_L:
	case XK_Alt_R:
	case XK_Super_L:
	case XK_Super_R:
	case XK_Hyper_L:
	case XK_Hyper_R:
	default:
		// TODO: unknown:
		// dbg << __FUNCTION__ << "unhandled keysym : '" << XKeysymToString(keysym) << "'\n";
		return events::keys::NUL;
	}
}

/* fill ev->key : UP, DOWN , LEFT, RIGHT, F1..F12 , ctrl , alt , mod(1...n), tab enter , escape etc..
   fill ev->unicode :
   set flag have_unicode | have_key
*/
bool display::private_data::X11::x11key_to_unicode(ew::graphics::gui::display * guiDpy, const ::XEvent * xevent, events::keyboard_event * ev)
{
	ev->key = keys::NUL;

	// widget->d->X11::getInputContext()
	window * win = (window *)ev->widget;

	Status status;
	KeySym xkeysym;
	char   keybuf[64] = { 0 };
	char * keydata = keybuf;
	int    count = 0;

	/*
	  TODO : For proper input handling
	  check status
	  do not check keyssym on release events
	*/
	if (!win->d->_x11_InputContext) {
		// other input method
		return false;
	}

	XKeyPressedEvent * xkPressEvent = (XKeyPressedEvent *)&xevent->xkey;

	//#define DEBUG_KEYPRESS_EVENT 1
#ifdef DEBUG_KEYPRESS_EVENT
	cerr << "--------------------------\n";
	cerr << " xkPressEvent->state & ControlMask = " << std::hex << (xkPressEvent->state & ControlMask) << "\n";
	cerr << " xkPressEvent->state & Mod1Mask    = " << std::hex << (xkPressEvent->state & Mod1Mask) << "\n";
	cerr << " xkPressEvent->state & Mod5Mask    = " << std::hex << (xkPressEvent->state & Mod5Mask) << "\n";
	cerr << " xkPressEvent->state & ShiftMask   = " << std::hex << (xkPressEvent->state & ShiftMask) << "\n";
	cerr << " xkPressEvent->state & LockMask   = " << std::hex << (xkPressEvent->state & LockMask) << "\n";
	cerr << " xkPressEvent->state & Button1Mask   = " << std::hex << (xkPressEvent->state & Button1Mask) << "\n";
	cerr << " xkPressEvent->state & Button2Mask   = " << std::hex << (xkPressEvent->state & Button2Mask) << "\n";
	cerr << " xkPressEvent->state & Button3Mask   = " << std::hex << (xkPressEvent->state & Button3Mask) << "\n";
	cerr << " xkPressEvent->state & Button4Mask   = " << std::hex << (xkPressEvent->state & Button4Mask) << "\n";
	cerr << " xkPressEvent->state & Button5Mask   = " << std::hex << (xkPressEvent->state & Button5Mask) << "\n";


#endif

	// extract flags
	ev->ctrl = (xkPressEvent->state & ControlMask);
	ev->altL = (xkPressEvent->state & Mod1Mask);
	ev->altR = (xkPressEvent->state & Mod5Mask);
	ev->shift = (xkPressEvent->state & ShiftMask);
	ev->capslock = (xkPressEvent->state & LockMask);
	ev->button1 = (xkPressEvent->state & Button1Mask);
	ev->button2 = (xkPressEvent->state & Button2Mask);
	ev->button3 = (xkPressEvent->state & Button3Mask);
	ev->button4 = (xkPressEvent->state & Button4Mask);
	ev->button5 = (xkPressEvent->state & Button5Mask);
	// remove control from mask
	(xkPressEvent->state &= ~ControlMask);

	using std::cerr;

#ifdef DEBUG_KEYPRESS_EVENT
	cerr << "--------------------------\n";
	cerr << "ev->ctrl    = " << ev->ctrl << "\n";
	cerr << "ev->shift    = " << ev->shift << "\n";
	cerr << "ev->capslock = " << ev->capslock << "\n";

	cerr << "ev->button1 = " << ev->button1 << "\n";
	cerr << "ev->button2 = " << ev->button2 << "\n";
	cerr << "ev->button3 = " << ev->button3 << "\n";
	cerr << "ev->button4 = " << ev->button4 << "\n";
	cerr << "ev->button5 = " << ev->button5 << "\n";
	cerr << std::dec;
#endif


	count = Xutf8LookupString(win->d->_x11_InputContext,
				  xkPressEvent, keydata, sizeof(keybuf),
				  &xkeysym, &status);

#if 1
	cerr << "xkeysym = dec(" << xkeysym << ")\n";
	cerr << "xkeysym = hex(0x" << std::hex << xkeysym << ")\n";
	cerr << std::dec;
#endif

	// dbg << CONSOLE_LEVEL5 << " xevent->xkey.keycode = " << xevent->xkey.keycode << "\n";
	// dbg << CONSOLE_LEVEL5 << " utf8 count " << count << "\n";
	// dbg << CONSOLE_LEVEL5 << " xkeysym " << (u64)xkeysym << "\n";

	bool have_keysym = false;
	bool have_string = false;

	switch (status) {
	case XBufferOverflow:
		cerr << "XBufferOverflow\n";
		break;
	case XLookupNone:
		cerr << "XLookupNone\n";
		break;
	case XLookupChars:
		cerr << "XLookupNone\n";
		break;
	case XLookupKeySym:
		cerr << "XLookupKeySym\n";
		have_keysym = true;
		break;
	case XLookupBoth:
		cerr << "XLookupBoth\n";
		have_keysym = true;
		have_string = true;
		break;
	default:
		cerr << "???\n";
	};

	if (count && have_string) {
		u8 * p   = (u8 *)keydata;
		u8 * end = (u8 *)keydata + sizeof(keybuf);

		cerr << "keydata ["<< keydata << "]\n";

		s32 unicode = 0;
		p = ew::codecs::text::get_cp<decltype(p)>(ew::codecs::text::unicode::utf8::codec(), p, end, &unicode);

		if (unicode < 0x20) {
			goto force_keysym;
		}
		if (unicode == 0x7f) {
			goto force_keysym;
		}

		ev->key = events::keys::UNICODE;
		ev->unicode = unicode;
#if 0
		cerr << "ev->unicode = " << ev->unicode << "\n";
#endif
		return true;
	}

force_keysym:
#if 0
	cerr << " have_keysym = " << have_keysym << "\n";
#endif
	if (have_keysym) {
		ev->key = xkeysym_to_event_key(xkeysym);
	}
	return true;
}


}
}
}
