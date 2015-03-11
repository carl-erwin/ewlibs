#pragma once

#include <ew/core/device/input/iinput_device_with_buttons.hpp>

/* obsolete */

namespace ew
{
namespace user
{
namespace devices
{

using namespace ew::core::types;

class EW_USER_EXPORT keyboard
	:
	public ew::core::devices::input::input_device_with_buttons
{
public:

	// From SDL
	// IKeyboard::Keys::Key_a

	/* dynamic using a key map ref/ptr */
	enum Keys {
		/* The keyboard syms have been cleverly chosen to map to ASCII */
		Key_UNKNOWN = 0,
		Key_FIRST = 0,
		Key_BACKSPACE = 8,
		Key_TAB = 9,
		Key_CLEAR = 12,
		Key_RETURN = 13,
		Key_PAUSE = 19,
		Key_ESCAPE = 27,
		Key_SPACE = 32,
		Key_EXCLAIM = 33,
		Key_QUOTEDBL = 34,
		Key_HASH = 35,
		Key_DOLLAR = 36,
		Key_AMPERSAND = 38,
		Key_QUOTE = 39,
		Key_LEFTPAREN = 40,
		Key_RIGHTPAREN = 41,
		Key_ASTERISK = 42,
		Key_PLUS = 43,
		Key_COMMA = 44,
		Key_MINUS = 45,
		Key_PERIOD = 46,
		Key_SLASH = 47,
		Key_0 = 48,
		Key_1 = 49,
		Key_2 = 50,
		Key_3 = 51,
		Key_4 = 52,
		Key_5 = 53,
		Key_6 = 54,
		Key_7 = 55,
		Key_8 = 56,
		Key_9 = 57,
		Key_COLON = 58,
		Key_SEMICOLON = 59,
		Key_LESS = 60,
		Key_EQUALS = 61,
		Key_GREATER = 62,
		Key_QUESTION = 63,
		Key_AT = 64,
		/*
		Skip uppercase letters
		*/
		Key_LEFTBRACKET = 91,
		Key_BACKSLASH = 92,
		Key_RIGHTBRACKET = 93,
		Key_CARET = 94,
		Key_UNDERSCORE = 95,
		Key_BACKQUOTE = 96,
		Key_a = 97,
		Key_b = 98,
		Key_c = 99,
		Key_d = 100,
		Key_e = 101,
		Key_f = 102,
		Key_g = 103,
		Key_h = 104,
		Key_i = 105,
		Key_j = 106,
		Key_k = 107,
		Key_l = 108,
		Key_m = 109,
		Key_n = 110,
		Key_o = 111,
		Key_p = 112,
		Key_q = 113,
		Key_r = 114,
		Key_s = 115,
		Key_t = 116,
		Key_u = 117,
		Key_v = 118,
		Key_w = 119,
		Key_x = 120,
		Key_y = 121,
		Key_z = 122,
		Key_DELETE = 127,
		/* End of ASCII mapped keysyms */

		/* International keyboard syms */
		Key_WORLDKey_0 = 160,    /* 0xA0 */
		Key_WORLDKey_1 = 161,
		Key_WORLDKey_2 = 162,
		Key_WORLDKey_3 = 163,
		Key_WORLDKey_4 = 164,
		Key_WORLDKey_5 = 165,
		Key_WORLDKey_6 = 166,
		Key_WORLDKey_7 = 167,
		Key_WORLDKey_8 = 168,
		Key_WORLDKey_9 = 169,
		Key_WORLDKey_10 = 170,
		Key_WORLDKey_11 = 171,
		Key_WORLDKey_12 = 172,
		Key_WORLDKey_13 = 173,
		Key_WORLDKey_14 = 174,
		Key_WORLDKey_15 = 175,
		Key_WORLDKey_16 = 176,
		Key_WORLDKey_17 = 177,
		Key_WORLDKey_18 = 178,
		Key_WORLDKey_19 = 179,
		Key_WORLDKey_20 = 180,
		Key_WORLDKey_21 = 181,
		Key_WORLDKey_22 = 182,
		Key_WORLDKey_23 = 183,
		Key_WORLDKey_24 = 184,
		Key_WORLDKey_25 = 185,
		Key_WORLDKey_26 = 186,
		Key_WORLDKey_27 = 187,
		Key_WORLDKey_28 = 188,
		Key_WORLDKey_29 = 189,
		Key_WORLDKey_30 = 190,
		Key_WORLDKey_31 = 191,
		Key_WORLDKey_32 = 192,
		Key_WORLDKey_33 = 193,
		Key_WORLDKey_34 = 194,
		Key_WORLDKey_35 = 195,
		Key_WORLDKey_36 = 196,
		Key_WORLDKey_37 = 197,
		Key_WORLDKey_38 = 198,
		Key_WORLDKey_39 = 199,
		Key_WORLDKey_40 = 200,
		Key_WORLDKey_41 = 201,
		Key_WORLDKey_42 = 202,
		Key_WORLDKey_43 = 203,
		Key_WORLDKey_44 = 204,
		Key_WORLDKey_45 = 205,
		Key_WORLDKey_46 = 206,
		Key_WORLDKey_47 = 207,
		Key_WORLDKey_48 = 208,
		Key_WORLDKey_49 = 209,
		Key_WORLDKey_50 = 210,
		Key_WORLDKey_51 = 211,
		Key_WORLDKey_52 = 212,
		Key_WORLDKey_53 = 213,
		Key_WORLDKey_54 = 214,
		Key_WORLDKey_55 = 215,
		Key_WORLDKey_56 = 216,
		Key_WORLDKey_57 = 217,
		Key_WORLDKey_58 = 218,
		Key_WORLDKey_59 = 219,
		Key_WORLDKey_60 = 220,
		Key_WORLDKey_61 = 221,
		Key_WORLDKey_62 = 222,
		Key_WORLDKey_63 = 223,
		Key_WORLDKey_64 = 224,
		Key_WORLDKey_65 = 225,
		Key_WORLDKey_66 = 226,
		Key_WORLDKey_67 = 227,
		Key_WORLDKey_68 = 228,
		Key_WORLDKey_69 = 229,
		Key_WORLDKey_70 = 230,
		Key_WORLDKey_71 = 231,
		Key_WORLDKey_72 = 232,
		Key_WORLDKey_73 = 233,
		Key_WORLDKey_74 = 234,
		Key_WORLDKey_75 = 235,
		Key_WORLDKey_76 = 236,
		Key_WORLDKey_77 = 237,
		Key_WORLDKey_78 = 238,
		Key_WORLDKey_79 = 239,
		Key_WORLDKey_80 = 240,
		Key_WORLDKey_81 = 241,
		Key_WORLDKey_82 = 242,
		Key_WORLDKey_83 = 243,
		Key_WORLDKey_84 = 244,
		Key_WORLDKey_85 = 245,
		Key_WORLDKey_86 = 246,
		Key_WORLDKey_87 = 247,
		Key_WORLDKey_88 = 248,
		Key_WORLDKey_89 = 249,
		Key_WORLDKey_90 = 250,
		Key_WORLDKey_91 = 251,
		Key_WORLDKey_92 = 252,
		Key_WORLDKey_93 = 253,
		Key_WORLDKey_94 = 254,
		Key_WORLDKey_95 = 255,    /* 0xFF */

		/* Numeric keypad */
		Key_KP0 = 256,
		Key_KP1 = 257,
		Key_KP2 = 258,
		Key_KP3 = 259,
		Key_KP4 = 260,
		Key_KP5 = 261,
		Key_KP6 = 262,
		Key_KP7 = 263,
		Key_KP8 = 264,
		Key_KP9 = 265,
		Key_KPKey_PERIOD = 266,
		Key_KPKey_DIVIDE = 267,
		Key_KPKey_MULTIPLY = 268,
		Key_KPKey_MINUS = 269,
		Key_KPKey_PLUS = 270,
		Key_KPKey_ENTER = 271,
		Key_KPKey_EQUALS = 272,

		/* Arrows + Home/End pad */
		Key_UP = 273,
		Key_DOWN = 274,
		Key_RIGHT = 275,
		Key_LEFT = 276,
		Key_INSERT = 277,
		Key_HOME = 278,
		Key_END = 279,
		Key_PAGEUP = 280,
		Key_PAGEDOWN = 281,

		/* Function keys */
		Key_F1 = 282,
		Key_F2 = 283,
		Key_F3 = 284,
		Key_F4 = 285,
		Key_F5 = 286,
		Key_F6 = 287,
		Key_F7 = 288,
		Key_F8 = 289,
		Key_F9 = 290,
		Key_F10 = 291,
		Key_F11 = 292,
		Key_F12 = 293,
		Key_F13 = 294,
		Key_F14 = 295,
		Key_F15 = 296,

		/* Key state modifier keys */
		Key_NUMLOCK = 300,
		Key_CAPSLOCK = 301,
		Key_SCROLLOCK = 302,
		Key_RSHIFT = 303,
		Key_LSHIFT = 304,
		Key_RCTRL = 305,
		Key_LCTRL = 306,
		Key_RALT = 307,
		Key_LALT = 308,
		Key_RMETA = 309,
		Key_LMETA = 310,
		Key_LSUPER = 311,    /* Left "Windows" key */
		Key_RSUPER = 312,    /* Right "Windows" key */
		Key_MODE = 313,    /* "Alt Gr" key */
		Key_COMPOSE = 314,    /* Multi-key compose key */

		/* Miscellaneous function keys */
		Key_HELP = 315,
		Key_PRINT = 316,
		Key_SYSREQ = 317,
		Key_BREAK = 318,
		Key_MENU = 319,
		Key_POWER = 320,    /* Power Macintosh power key */
		Key_EURO = 321,    /* Some european keyboards */
		Key_UNDO = 322,    /* Atari keyboard has Undo */

		/* Add any other keys here */
		Key_LAST
	};

	/* */
	keyboard(void);

	keyboard(u32 index);

	virtual u8 * getKeys(void) = 0;

};

}
}
}

