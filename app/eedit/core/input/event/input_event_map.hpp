#pragma once

#undef NDEBUG
#include <assert.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <set>

#include "ew/graphics/gui/event/event.hpp"
#include "ew/codecs/text/unicode/utf8/utf8.hpp"

#include "core/log.hpp"

//string_tools.h:
#define STRINGIFY(X)   #X
#define SWITCH_CASE_STRINGIFY(X)   case X: return #X ;

/* FIXME: BIG HACK : do some cleanup, use std::regex */


/**
 ** \brief An input event map contains a "path" to a given command to be run.<br/>
 **        The core provides : TODO
 **
 **  There is a  global map table that holds the differents modules map.
 **    example:
 **                 c/c++ map
 **                 sh    map
 **                 txt   map
 **
 **  For each buffer, when a "major-mode" is loaded/enabled, it is responsible of the root map declaration.
 **  other enabled minor-modes , will register their respective function "path" only if there is no conflicts with an already registered one
 **   example of error message 'the sequence '...' conflicts with the already registered function 'module:func_name''
 **
 **  extern 'C' int register_input_event_map 'map_name' 'utf8_string'"
 **  extern 'C' int select_input_event_map 'map_name'
 **  extern 'C' int unregister_input_event_map 'map_name'
 */


/*
TODO:
ctr f -> in search
ctrl + f , left -> crash

should generate parse error

*/

namespace eedit
{

// editor_input_event_map
struct editor_input_event_map;
struct input_map_entry;

inline void dump_keymap_entries_array(std::vector<input_map_entry *> & cur_path);

struct input_action {
	char * fn_name = nullptr;

	~input_action()
	{
		delete [] fn_name;
	}
};


using namespace ew::graphics::gui::events;
using namespace ew::graphics::gui::events::keys;

///////////////////////////



struct input_event_s {

	typedef ew::graphics::gui::events::keys::key_value key_type;

public:

	enum event_type {
		invalid,
		keypress,
		button_press,
		button_release,
	};

	enum range_type {
		no_range,
		simple_range
	};

	enum keyboard_mod_mask {
		mod_ctrl   = (1 << 0),
		mod_shift  = (1 << 1),
		mod_altL   = (1 << 2),
		mod_altR   = (1 << 3),
		mod_oskey  = (1 << 4),
	};

public:
	// keypress event contructor
	input_event_s(key_type kt = NUL, range_type rt = no_range, u32 mod_mask = 0, u32 start_val = 0, u32 end_val = 0);

	// button event contructor
	input_event_s(event_type button_event, int x, int y, u32 button);


public:
	event_type type  = invalid;

	key_type key     = NO_KEY;
	bool is_range    = false;
	u32  start_value = 0;
	u32  end_value   = 0;
	bool ctrl        = false;
	bool shift       = false;
	bool altL        = false;
	bool altR        = false;
	bool oskey       = false;

	u32 button_mask  = 0;
	s32 x            = 0;
	s32 y            = 0;

public:
	bool contains(const input_event_s & b)     const;
	bool operator == (const input_event_s & b) const;
	bool operator != (const input_event_s & b) const;

	const char * type_to_string(key_type t)    const;
	void dump_event() const;
};

inline input_event_s::input_event_s(key_type kt, range_type rt, u32 mod_mask, u32 start_val, u32 end_val)
{
	type = keypress;

	// filter special keys here
	switch (kt) {
	case Tab: {
		kt = UNICODE;
		start_val = '\t';
		end_val   = '\t';
	}
	break;
	default: {
		/* TODO: handle other keys */
	} break;
	}

	key = kt;
	is_range    = rt == simple_range ? true : false;
	start_value = start_val;
	end_value   = end_val;
	ctrl        = mod_mask & mod_ctrl;
	altL        = mod_mask & mod_altL;
	if (kt != UNICODE) {
		altR        = mod_mask & mod_altR;
	}
	oskey       = mod_mask & mod_oskey;
}

// button event contructor
inline input_event_s::input_event_s(event_type button_event, s32 x_, s32 y_, u32 button)
{
	type = button_event;

	// button1 is first bit
	button_mask = (1 << (button - 1));
	x = x_;
	y = y_;
}



inline void input_event_s::dump_event() const
{
	app_log << "this(" << this << ") ";

	if (ctrl) app_log << "ctrl + ";
	if (altL) app_log << "alt + ";
	if (altR) app_log << "altR + ";
	if (oskey) app_log << "oskey + ";
	if (is_range) {
		app_log << "[ ";
	}

	app_log << "type(" << key << ") ";
	app_log << "type_str(" << type_to_string(key) << ") ";

	app_log << "start_value(" << start_value << ") ";
	if (is_range) {
		app_log << "<=> end_value(" << end_value << ") ]";
	}

	if (button_mask) {
		app_log << "button_mask(" << std::hex << button_mask << ")" << std::dec;
	}

}


inline   const char * input_event_s::type_to_string(key_type t) const
{
	switch (t) {
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


inline  bool input_event_s::contains(const input_event_s & b) const
{

	if (*this == b) return true;

	switch (type) {
	case input_event_s::keypress: {

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

	case input_event_s::button_press:
	case input_event_s::button_release: {
		return false;
	}
	break;

	default:
		return false;
	}

	return false;
}

inline bool input_event_s::operator == (const input_event_s & b) const
{

	if (type != b.type)          return false;

	switch (type) {
	case input_event_s::keypress: {

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

	case input_event_s::button_press:
	case input_event_s::button_release: {
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

inline  bool input_event_s::operator != (const input_event_s & b) const
{
	return !this->operator == (b);
}


///////////////////////////

// keymap/entry.hpp

struct input_map_entry {

	input_event_s ev;

	input_map_entry(input_event_s::key_type kt = NUL,
			input_event_s::range_type rt = input_event_s::no_range,
			u32 mod_mask = 0,
			u32 start_val = 0,
			u32 end_val = 0)
	{

		ev.key        = kt;
		ev.is_range    = rt ==  input_event_s::simple_range ? true : false;
		ev.start_value = start_val;
		ev.end_value   = end_val;
		ev.ctrl        = mod_mask & input_event_s::mod_ctrl;
		ev.altL        = mod_mask & input_event_s::mod_altL;
		ev.altR        = mod_mask & input_event_s::mod_altR;
		ev.oskey       = mod_mask & input_event_s::mod_oskey;
	}

	~input_map_entry()
	{
		delete action;
		for (auto it : entries) {
			delete it;
		}
	}


	std::vector<input_map_entry *> entries;
	input_action * action = nullptr; // entries.size() == 0 if action is defined

	void dump_event()
	{
		return;

		ev.dump_event();
		if (action != nullptr) {
			app_log << " = action(" << (action ? action->fn_name : "") << ")";
		}

	}

	void dump(std::vector<input_map_entry *> & cur_path)
	{
		return;

		cur_path.push_back(this); // on exit pop_back

		if (entries.size() == 0) {
			// leaf dump sequence
			dump_keymap_entries_array(cur_path);
			cur_path.pop_back();
			return;
		}

		auto it = entries.begin();
		while (it != entries.end()) {
			(*it)->dump(cur_path);
			++it;
		}

		cur_path.pop_back();
	}

};

inline void dump_keymap_entries_array(std::vector<input_map_entry *> & cur_path)
{
	return;

	for (auto i = cur_path.begin(); i != cur_path.end();) {
		(*i)->dump_event();
		++i;
		if (i != cur_path.end())
			app_log << "| ";
	}
	app_log << "\n";
}

struct editor_input_event_map {

	const char * name = nullptr;
	bool was_init = false;
	// first level
	std::vector<input_map_entry *> entries;

	void dump()
	{
		return;
		std::vector<input_map_entry *> cur_path;

		auto it = entries.begin();
		while (it != entries.end()) {
			(*it)->dump(cur_path);
			++it;
		}
	}


	~editor_input_event_map()
	{
		for (auto it : entries) {
			delete it;
		}
	}


};

/*
  keymap default
*/
enum input_map_token_type {
	TOK_INVALID = 0,
	TOK_MOUSE_BUTTON,
	TOK_CTRL,
	TOK_ALT,
	TOK_ALT_R,
	TOK_UNICODE,
	TOK_COMMA,
	TOK_EQ,
	TOK_KEYVAL,
	TOK_PAGE_UP,
	TOK_PAGE_DOWN,

};

struct parse_context {
	u8 * p    = nullptr;
	const u8 * pend = nullptr;

	enum key_value tmp_kval;
	u32  button_mask_val;

	editor_input_event_map * current_input_map = nullptr;
	input_map_entry  * current_input_map_entry = nullptr;

	std::vector< input_map_entry * > cur_seq;
public:
	~parse_context()
	{
		delete current_input_map_entry;
	}

	size_t remain()
	{
		// app_log << " parse ctx : remaining bytes '" << (pend - p) << "'\n";
		return (pend - p);
	}

};

inline bool skip_blanks(parse_context * ctx)
{
	while (ctx->p < ctx->pend) {
		u8 c = *ctx->p;
		switch (c) {
		case ' ':
		case '\t': {
			++ctx->p;
			continue;
		}
		break;
		default: {
			return true;
		}
		}
	}
	return true;
}

inline bool skip_non_blanks(parse_context * ctx)
{
	while (ctx->p < ctx->pend) {
		u8 c = *ctx->p;
		switch (c) {
		case ' ':
		case '\t': {
			return true;
		}
		break;

		default: {
			++ctx->p;
			continue;
		}
		}
	}
	return true;
}

// todo: add \r\n support
inline bool skip_line(parse_context * ctx)
{
	u8 last_c = 0;
	while (ctx->p < ctx->pend) {
		u8 c = *ctx->p;

		if (c == '\r') {
			last_c = '\r';
		}

		if (c == '\n') {
			++(ctx->p);
			break;
		}

		if (last_c == '\r') {
			++(ctx->p);
			break;
		}

		last_c = c;
		++(ctx->p);
	}

	return true;
}

inline bool expected_token(parse_context * ctx, const u8 * expected, const size_t expected_len)
{
	if (ctx->remain() < expected_len) {
		app_log << __FUNCTION__ << " no enough bytes\n";
		return false;
	}

	if (::memcmp(ctx->p, expected, expected_len) == 0) {
		ctx->p = ctx->p + expected_len;
		return true;
	}

	app_log << __FUNCTION__ << " '" << expected << "' not found\n";
	return false;
}

inline bool expect_c_string(parse_context * ctx, const char * str, const size_t slen)
{
	return expected_token(ctx, (const u8 *)str, slen);
}

inline bool extract_int(parse_context * ctx, int * val)
{
	if (ctx->remain() == 0) {
		app_log << __FUNCTION__ << " no enough bytes\n";
		return false;
	}

	char * endptr;
	long int v = strtol((char *)ctx->p, &endptr, 10);
	if (endptr == nullptr) {
		return false;
	}
	ctx->p = (u8 *)endptr;

	app_log << __FUNCTION__ << " extracted val = " << v << "\n";
	*val = v;
	return false;
}



inline input_map_token_type get_token_control(parse_context * ctx)
{
	const u8 * expected = (const u8 *)"ctrl";
	size_t expected_len = 4;

	if (expected_token(ctx, expected, expected_len) == false) {
		return TOK_INVALID;
	}

	return TOK_CTRL;
}

inline input_map_token_type get_token_alt(parse_context * ctx)
{
	const u8 * expected = (const u8 *)"alt";
	size_t expected_len = 3;

	if (expected_token(ctx, expected, expected_len) == false) {
		return TOK_INVALID;
	}

	return TOK_ALT;
}

inline input_map_token_type get_token_altR(parse_context * ctx)
{
	const u8 * expected = (const u8 *)"altR";
	size_t expected_len = 4;

	if (expected_token(ctx, expected, expected_len) == false) {
		return TOK_INVALID;
	}

	return TOK_ALT_R;
}


inline input_map_token_type get_token_unicode(parse_context * ctx)
{
	const u8 * expected = (const u8 *)"unicode";
	size_t expected_len = 7;

	if (expected_token(ctx, expected, expected_len) == false) {
		return TOK_INVALID;
	}

	return TOK_UNICODE;
}

inline input_map_token_type get_token_page_up_down(parse_context * ctx)
{
	const u8 * expected1 = (const u8 *)"page-up";
	const u8 * expected2 = (const u8 *)"page-down";

	size_t expected1_len = 7;
	size_t expected2_len = 9;

	if (expected_token(ctx, expected1, expected1_len) == true) {
		return TOK_PAGE_UP;
	}

	if (expected_token(ctx, expected2, expected2_len) == true) {
		return TOK_PAGE_DOWN;
	}

	return TOK_INVALID;
}




inline input_map_token_type get_token_comma(parse_context * ctx)
{
	const u8 * expected = (const u8 *)",";
	size_t expected_len = 1;

	if (expected_token(ctx, expected, expected_len) == false) {
		return TOK_INVALID;
	}

	return TOK_COMMA;
}

inline input_map_token_type get_token_equal(parse_context * ctx)
{
	const u8 * expected = (const u8 *)"=";
	size_t expected_len = 1;

	if (expected_token(ctx, expected, expected_len) == false) {
		return TOK_INVALID;
	}

	return TOK_EQ;
}

inline input_map_token_type get_token_keyname(parse_context * ctx)
{
	// skip blank
	skip_blanks(ctx);
	auto start = ctx->p;
	// get blank
	skip_non_blanks(ctx);

	// extract word
	int len = ctx->p - start;
	u8 * buff = new u8[len + 1]; // move to ctx->accum
	::strncpy((char *)buff, (const char *)start, len);
	buff[len] = '\0';

	// tolower(word)
	auto b = buff;
	while (b < buff + len) {
		*b = ::tolower(*b);
		++b;
	}

	// check keymap str table
	ctx->tmp_kval = c_string_to_key_value((const char *)buff);

	app_log << "found ctx->tmp_kval str '" << buff << "'\n";
	app_log << "found ctx->tmp_kval : " << ctx->tmp_kval << "\n";

	delete [] buff;

	ctx->current_input_map_entry->ev.key = ctx->tmp_kval;
	return TOK_KEYVAL;
}

inline input_map_token_type get_token_mouse(parse_context * ctx)
{
	const u8 * expected = (const u8 *)"mouse-button";
	size_t expected_len = 12;

	if (expected_token(ctx, expected, expected_len) == false) {
		assert(0);
		return TOK_INVALID;
	}

	return TOK_MOUSE_BUTTON;
}






// get_main_tokens
inline input_map_token_type get_token(parse_context * ctx)
{
	input_map_token_type tok = TOK_INVALID;

	while (ctx->p < ctx->pend) {
		switch (*ctx->p) {
		case 'c':
			tok = get_token_control(ctx);
			break;
		case 'a':
			tok = get_token_alt(ctx);
			break;
		case 'u':
			tok = get_token_unicode(ctx);
			break;
		case ',':
			tok = get_token_comma(ctx);
			break;
		case '=':
			tok = get_token_equal(ctx);
			break;

		case 'p':
			tok = get_token_page_up_down(ctx);
			break;

		case 'm':
			tok = get_token_mouse(ctx);
			break;



		default:
			break;
		}

		// found valid token ? -> return
		if (tok != TOK_INVALID) {
			return tok;
		}

		// end of line ?
		if (*ctx->p == '\n') {
			skip_line(ctx);
			return tok;
		}


		// fallback to keyname
		tok = get_token_keyname(ctx);

		// found valid keyname ? -> return
		if (tok != TOK_INVALID) {
			return tok;
		}

		// error: @ line ...
		skip_line(ctx);
		return tok;
	}

	return tok;
}


inline bool parse_control(parse_context * ctx)
{
	skip_blanks(ctx);

	if (expect_c_string(ctx, "+", 1) == false) {
		return false;
	}

	if (ctx->current_input_map_entry->ev.ctrl == true) {
		app_log << "ctrl already defined !\n";
		return false;
	}
	ctx->current_input_map_entry->ev.ctrl = true;

	skip_blanks(ctx);
	return true;
}

inline bool possible_tokens(parse_context * ctx, std::set<s32> & set)
{
	u8 c = *ctx->p;
	auto it = set.find((s32)c);
	if (it != set.end()) {
		// app_log << "*it = '" << (char)(*it) << "'\n";
		return true;
	}

	return false;
}


inline bool decode_utf8_codepoint(parse_context * ctx, u32 * out_value)
{
	auto codec = ew::codecs::text::unicode::utf8::codec();

	int cp;
	ctx->p = ew::codecs::text::get_cp<u8 *>(codec, ctx->p, (u8 *)ctx->pend, &cp);
	if (cp < 0) {
		return false;
	}

	*out_value = cp;
	return true;
}

inline bool parse_unicode_value(parse_context * ctx, u32 * out_value)
{
	std::set<s32> set1( { '\'', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' } );

	bool bret = possible_tokens(ctx, set1);
	if (bret == false) {
		return false;
	}

	switch (*ctx->p) {

	case '\'': {
		++ctx->p;
		decode_utf8_codepoint(ctx, out_value);
		if (expect_c_string(ctx, "'", 1) == false) {
			return false;
		}
	}
	break;

	default: {
		const char * nptr = (const char *)ctx->p;

		char * endptr;
		int base = 10;
		if (::tolower(nptr[1]) == 'x') {
			base = 16;
		}

		*out_value = (u32)strtoll(nptr, &endptr, base);
		ctx->p = (u8 *)endptr;

	}
	break;
	}

	return true;
}


/* "unicode( 0 <=> 0xFFFFFFFF ) => self_insert" */
/* "unicode('€') => self_insert" */
/* "unicode('a' <=> 'Z') => self_insert" */
inline bool parse_unicode(parse_context * ctx)
{
	ctx->current_input_map_entry->ev.key = keys::UNICODE;

	enum steps {
		parse_open_paren,
		parse_first_value,
		check_is_range,
		parse_second_value,
		parse_close_paren,
		have_close_paren,
		parse_ok,
		parse_error
	};

	steps parse_step = parse_open_paren;
	bool  stop_parser = false;


	while ((ctx->p < ctx->pend) && (stop_parser == false)) {
		///
		// app_log << __FUNCTION__ << "\n";
		ctx->remain();

		skip_blanks(ctx);

		input_map_entry kmap_entry;

		switch (parse_step) {

		case parse_open_paren: {
			// app_log << "parse_open_paren\n";
			parse_step = parse_error;
			if (expected_token(ctx, (const u8 *)"(", 1) == true) {
				parse_step = parse_first_value;
			}
		}
		break;

		case parse_first_value: {

			// app_log << "parse_first_value\n";

			parse_step = parse_error;
			if (parse_unicode_value(ctx, &ctx->current_input_map_entry->ev.start_value) == true) {
				// app_log << "first val = " << ctx->current_keymap_entry->start_value << "\n";
				parse_step = check_is_range;
			}

		}
		break;

		case check_is_range: {

			parse_step = parse_error;

			// app_log << "check_is_range\n";

			// check for close parent
			if (*ctx->p == ')') {
				parse_step = parse_close_paren;
				continue;
			}

			if (*ctx->p == '<') {
				parse_step = parse_close_paren;
				// expect sequence '<=>'
				if (expect_c_string(ctx, "<=>", 3) == false) {
					continue;
				}

				ctx->current_input_map_entry->ev.is_range = true;
				parse_step = parse_second_value;
			}

		}
		break;

		case parse_second_value: {
			// app_log << "parse_second_value\n";

			parse_step = parse_error;

			if (parse_unicode_value(ctx, &ctx->current_input_map_entry->ev.end_value) == true) {
				// app_log << "second val = " << ctx->current_keymap_entry->end_value << "\n";
				parse_step = parse_close_paren;
			}

		}
		break;

		case parse_close_paren: {
			// app_log << "parse_close_paren\n";
			parse_step = parse_error;
			if (expected_token(ctx, (const u8 *)")", 1) == true) {
				parse_step = have_close_paren;
			}
		}
		break;

		case have_close_paren: {
			// app_log << "have_close_paren\n";
			parse_step = parse_ok;
		}
		break;

		case parse_ok: {
			// app_log << "parse_ok\n";
			stop_parser = true;
			break;
		}
		break;

		case parse_error:
		default: {
			app_log << "parse_error\n";
			stop_parser = true;
		}
		break;

		}
	}

	if (parse_step == parse_ok) {
		return true;
	}

	return false;
}


inline bool parse_page_up(parse_context * ctx)
{
	ctx->current_input_map_entry->ev.key = keys::PageUp;
	return true;
}

inline bool parse_page_down(parse_context * ctx)
{
	ctx->current_input_map_entry->ev.key = keys::PageDown;
	return true;
}

inline bool parse_mouse_button(parse_context * ctx)
{
	int val = 0;

	extract_int(ctx, &val);

	assert(val > 0);
	ctx->current_input_map_entry->ev.button_mask = (1 << (val - 1));

	if (expect_c_string(ctx, "-", 1) == false) {
		assert(0);
		return false;
	}

	// check_c_string ?
	if (expect_c_string(ctx, "press", 5)) {
		ctx->current_input_map_entry->ev.type = input_event_s::button_press;
		return true;
	}

	if (expect_c_string(ctx, "release", 7)) {
		ctx->current_input_map_entry->ev.type = input_event_s::button_release;
		return true;
	}

	assert(0);
	return false;
}



inline bool parse_alt(parse_context * ctx)
{
	skip_blanks(ctx);

	if (expect_c_string(ctx, "+", 1) == false) {
		return false;
	}

	if (ctx->current_input_map_entry->ev.altL == true) {
		app_log << "alt already defined !\n";
		return false;
	}
	ctx->current_input_map_entry->ev.altL = true;

	skip_blanks(ctx);
	return true;
}

inline bool extract_action_name(parse_context * ctx)
{
	u8 * s = ctx->p;
	while (ctx->p < ctx->pend) {
		char c = *ctx->p;

		// ::fprintf(stderr, "extracted char[%c]\n" , c);

		if (isalnum(c)) {
			ctx->p++;
			continue;
		}

		if (c == '-') {
			ctx->p++;
			continue;
		}

		if (c == '_') {
			ctx->p++;
			continue;
		}

		if (isblank(c)) {
			break;
		}

		// if (c == '\r') {
		//         break;
		// }

		if (c == '\n') {
			break;
		}

		assert(0);
		return false;
	}

	size_t slen = (size_t)(ctx->p - s);

	input_action * action = new input_action;
	action->fn_name = new char [slen + 1];
	::strncpy(action->fn_name, (const char *)s, slen);
	action->fn_name[slen] = 0;

	app_log << "extracted action_name["<< (int)slen<<"] = '" << action->fn_name << "\n";

	ctx->current_input_map_entry->action = action;

	ctx->current_input_map_entry->dump_event();

	return false;
}

inline bool parse_binded_action(parse_context * ctx)
{
	skip_blanks(ctx);
	if (extract_action_name(ctx) == false) {
		return true;
	}
	skip_blanks(ctx);

	if (ctx->remain() == 0) {
		return true;
	}

	if (expect_c_string(ctx, "\n", 1) == false) {
		return false;
	}

	return true;
}

inline bool  push_current_keymap_entry(parse_context * ctx)
{
	ctx->cur_seq.push_back(ctx->current_input_map_entry);

	ctx->current_input_map_entry = nullptr;

	//    app_log << " : push current keymap entry (" << ctx->cur_seq.size() << ")\n";
	//    ctx->current_keymap_entry->dump();

	// alloc next: do not forget parse_context destructor
	ctx->current_input_map_entry = new input_map_entry;
	return true;
}

inline bool find_seq_nodes(int depth, const std::vector<input_map_entry *> & keymap_entries,
			   std::vector<input_map_entry *>::iterator seq_head,
			   const std::vector<input_map_entry *>::iterator & seq_end,
			   std::vector<input_map_entry *>    &    out)
{
	static int debug = 0;

	if (seq_head == seq_end) {
		return false;
	}

	if (debug)
		app_log << " find_seq_nodes  depth = " << depth << "\n";

	// app_log << "lookin for " << "\n";
	// (*seq_head)->dump_event();
	// app_log << "\n";

	bool found = false;

	auto a = *seq_head;

	for (auto km_it = keymap_entries.rbegin(); km_it != keymap_entries.rend(); ++km_it) {
		auto b = *km_it;

		if (debug) {
			// app_log << " ====================\n";
			// app_log << " COMPARE A = \n";
			// a->dump();
			// app_log << " TO B = \n";
			// b->dump();
			// app_log << " ====================\n";
		}

		if ((*a).ev == (*b).ev) {

			if (debug) {
				app_log << "{ ";
				a->dump_event();
				app_log << " == ";
				b->dump_event();
				app_log << " }\n";
			}

			found = true;
			out.push_back(b);

			++seq_head;
			if (seq_head == seq_end) {
				if (debug) {
					app_log << " seq_head == seq_end\n";
				}
				break;
			}

			if (debug) {
				app_log << " seq_head != seq_end\n";
			}

			return find_seq_nodes(depth + 1, b->entries, seq_head, seq_end, out);

		} else {

			if (debug) {
				app_log << "{ ";
				a->dump_event();
				app_log << " != ";
				b->dump_event();
				app_log << " }\n";
			}
		}
	}

	if (debug) {
		app_log << " find_seq_nodes  depth = " << depth << " retcode " << found << "\n";
	}

	return found;
}

inline bool find_sequence(editor_input_event_map * intput_map,
			  std::vector<input_map_entry *> & seq,
			  std::vector<input_map_entry *> & out)
{
	// take the seq head
	auto seq_it = seq.begin();
	auto seq_end = seq.end();

	return find_seq_nodes(0, intput_map->entries, seq_it, seq_end, out);
}

inline bool keymap_insert_sequence(editor_input_event_map * input_map,
				   std::vector<input_map_entry *> & seq,
				   std::vector<input_map_entry *> & out)
{
	std::vector<input_map_entry *> * cur_parent = &input_map->entries;

	size_t depth = 0;

	// release common nodes
	if (out.size()) {

		// same seq just exchange action pointers
		if (out.size() == seq.size()) {
			auto idx = seq.size() - 1;
			std::swap(seq[idx]->action, out[idx]->action);
		}

		for (depth = 0; depth < out.size(); depth++) {
			delete seq[depth];
		}

		cur_parent = &out[depth - 1]->entries;
	}

	// insert remainning nodes
	for (; depth < seq.size(); depth++) {
		cur_parent->push_back(seq[depth]);
		cur_parent = &seq[depth]->entries;
	}

	return true;
}

inline bool add_keys_to_current_input_map(parse_context * ctx)
{
	static int debug = 0;

	std::vector<input_map_entry *> out;


	if (0) {
		app_log << " ==================================================\n";
		app_log << " add_keys_to_current_keymap\n";
		app_log << " ctx->cur_seq.size() = " << ctx->cur_seq.size() << "\n";
		app_log << " == cur seq  ======================================\n";
		dump_keymap_entries_array(ctx->cur_seq);
		app_log << " ==================================================\n";
	}
	bool found = find_sequence(ctx->current_input_map,
				   ctx->cur_seq,
				   out);

	// TODO: remove old sequence
	if (debug) {
		app_log << " find_sequence = " << found << "\n";
		app_log << " out.size()    = " << out.size() << "\n";
		app_log << " ctx->current_keymap->entries.size() = " << ctx->current_input_map->entries.size() << "\n";
		app_log << " ---------------------------------------\n";
	}

	found = keymap_insert_sequence(ctx->current_input_map, ctx->cur_seq, out);

	// MUST reset
	ctx->cur_seq.clear();
	return true;
}

inline bool parse_input_map_string(parse_context & ctx)
{
	while (ctx.p < ctx.pend) {

		skip_blanks(&ctx);

		input_map_token_type token_type = get_token(&ctx); // TODO: fill in get_token()
		ctx.remain();

		// app_log << "token = " << token_type << "\n";
		// app_log << "p ... = '" << ctx.p << "'\n"; // VERY VERBOSE

		switch (token_type) {

		case TOK_CTRL: {
			parse_control(&ctx);
		}
		break;

		case TOK_ALT: {
			parse_alt(&ctx);
		}
		break;

		case TOK_UNICODE: {
			parse_unicode(&ctx);
		}
		break;

		case TOK_PAGE_UP: {
			parse_page_up(&ctx);
		}
		break;

		case TOK_PAGE_DOWN: {
			parse_page_down(&ctx);
		}
		break;

		case TOK_MOUSE_BUTTON: {
			parse_mouse_button(&ctx);
		}
		break;

		case TOK_COMMA: {
			push_current_keymap_entry(&ctx);
		}
		break;

		case TOK_EQ: {
			if (parse_binded_action(&ctx) == false) {
				return false;
			} else {
				push_current_keymap_entry(&ctx);
				// build tree
				if (add_keys_to_current_input_map(&ctx) == false) {
					return false;
				}
			}
		}
		break;

		default:
			// skip line + log
			break;
		}
	}

	return true;
}

inline bool parse_input_map_config(const u8 * start, const u8 * pend, std::map<std::string, editor_input_event_map *> & input_event_table)
{
	app_log << __FUNCTION__ << " begin\n";

	parse_context ctx;

	ctx.current_input_map = new editor_input_event_map;
	ctx.current_input_map->was_init = false;
	ctx.current_input_map_entry = new input_map_entry;

	ctx.p    = (u8 *)start;
	ctx.pend = (u8 *)pend;
	ctx.remain();

	app_log << __FUNCTION__ << " parsing :\n'" << ctx.p << "'\n";

	parse_input_map_string(ctx);

	app_log << " FINAL : ctx.current_keymap->entries.size() = " << ctx.current_input_map->entries.size() << "\n\n";

	ctx.current_input_map->dump();

	input_event_table.insert(std::pair<std::string, editor_input_event_map *>(std::string("default"), ctx.current_input_map));

	app_log << "\n";
	app_log << __FUNCTION__ << " pend\n";

	return true;
}


inline bool eval_input_event(const input_event_s * ev,
			     std::vector<input_map_entry *> * current_level,
			     input_map_entry ** match_found)
{
	for (auto it = current_level->rbegin(); it != current_level->rend(); ++it) {

		input_map_entry * a = *it;

		if ((*a).ev == *ev) {
			*match_found = *it;
			return true;
		}

		if ((*it)->ev.contains(*ev)) {
			*match_found = *it;
			return true;
		}
	}

	*match_found = nullptr;
	return false;
}

inline bool setup_default_input_map(std::map<std::string, editor_input_event_map *> & input_event_table)
{
#if 1
	const u8  * start = (const u8 *)\
			    "unicode(0x00000000 <=> 0xFFFFFFFF ) = self-insert\n"           \
			    "return = insert-newline\n"                                     \
			    "ctrl + unicode('x'), ctrl + unicode('s') = save-buffer\n"      \
			    "ctrl + unicode('x'), ctrl + unicode('c') = quit-editor\n"      \
			    "ctrl + unicode('g') = goto-line\n"                             \
			    "ctrl + unicode('a') = goto-beginning-of-line\n"                \
			    "ctrl + unicode('e') = goto-end-of-line\n"                      \
			    "home                = goto-beginning-of-line\n"              \
			    "end                 = goto-end-of-line\n"                    \
			    "ctrl + unicode('s') = incremental-search-forward\n"            \
			    "backspace           = delete-backward-char\n"                  \
			    "delete              = delete-forward-char\n"                   \
			    "ctrl + unicode('d')  = delete-forward-char\n"                  \
			    "left         = left-char\n"                                     \
			    "right        = right-char\n"                                    \
			    "up           = previous-line\n"                                 \
			    "down         = next-line\n"                                    \
			    "page-up   = page-up\n"                                         \
			    "page-down = page-down\n"                                       \
			    "ctrl + right = right-word\n" \
			    "ctrl + left = left-word\n" \
			    "alt  + unicode('<') = goto-beginning-of-buffer\n"              \
			    "alt  + unicode('>') = goto-end-of-buffer\n"                    \
			    "ctrl + home         = goto-beginning-of-buffer\n"              \
			    "ctrl + end          = goto-end-of-buffer\n"                    \
			    "ctrl + unicode('f') , left  = fold-current-block\n"            \
			    "ctrl + unicode('f') , right = unfold-current-block\n"          \
			    "ctrl + unicode(' ')   =  begin-selection\n"                   \
			    "mouse-button1-press   =  begin-selection\n"                   \
			    "mouse-button1-release =  end-selection\n"                     \
			    "mouse-button2-press   =  mouse-button2-press\n"               \
			    "mouse-button2-release =  mouse-button2-release\n"              \
			    "mouse-button3-press   =  mouse-button3-press\n"               \
			    "mouse-button3-release =  mouse-button3-release\n"              \
			    "mouse-button4-press   =  mouse-wheel-up\n"               \
			    "mouse-button5-press   =  mouse-wheel-down\n"              \
			    "ctrl + unicode('z')   =  undo\n"              \
			    "ctrl + unicode('Z')   =  redo\n"              \
			    "ctrl + unicode('L')   =  dump-buffer-log\n"     \
			    ;
#else
	const u8  * start = (const u8 *)"unicode(0x00000000 <=> 0xFFFFFFFF ) = self-insert\n";
#endif

	const u8  * end = start + strlen((const char *)start);

	bool ret;
	ret = parse_input_map_config((const u8 *)start, (const u8 *)end, input_event_table);
	return ret;
}


inline bool release_input_map(std::map<std::string, editor_input_event_map *> & input_event_table)
{
	for (auto & it : input_event_table) {
		delete it.second;
	}

	return true;
}

}


