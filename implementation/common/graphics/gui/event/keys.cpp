#include <iostream>
#include <ctype.h>
#include <cstring>
#include <string>
#include <map>


#include "ew/graphics/gui/event/keys.hpp"


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


std::map<std::string, enum key_value> str_to_keyval_map;
bool was_init = false;

// populate map : move to gui::init ?
bool init_str_keyval_map()
{
	if (was_init == true)
		return true;

	for (int i = NUL; i < MaxKey; i++) {
		char buff[32];

		// TODO: use lambda here
		auto s = c_string(key_value(i));

		int slen = ::strlen(s);
		if (slen == 0)
			continue;

		for (int i = 0; i < slen; i++) {
			buff[i] = ::tolower(s[i]);
		}
		buff[slen] = '\0';

		std::string str(buff);
		str_to_keyval_map.insert(std::pair<std::string, key_value>(str, key_value(i)));
	}

	was_init = true;
	return true;
}

enum key_value c_string_to_key_value(const char * c_str)
{
	init_str_keyval_map();

	enum key_value val = NUL;

	auto end = str_to_keyval_map.end();

	auto str = std::string(c_str);
	auto it = str_to_keyval_map.find(str);

	if (it != end) {
		val = it->second;
	}

	return val;
}


} // ! namespace keys
} // ! namespace events
} // ! namespace gui
} // ! namespace graphics
} // ! namespace ew
