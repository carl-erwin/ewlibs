#pragma once

#include <new>
#include <cstring>
#include <ew/core/types/types.hpp>

namespace ew
{
namespace utils
{

using namespace ew::core::types;

/**  return a "new" allocated copy of str
     will return nullptr new fails
 */
static inline  char * c_string_dup(const char * str)
{
	if (str == nullptr) {
		return nullptr;
	}

	try {
		size_t len = ::strlen(str);
		char * dup = ::new char [ len + 1 ];
		::memcpy(dup, str, len);
		dup[len] = 0;
		return dup;
	}

	catch (std::bad_alloc & e) {
		return nullptr;
	}

	return nullptr;
}

}
}
