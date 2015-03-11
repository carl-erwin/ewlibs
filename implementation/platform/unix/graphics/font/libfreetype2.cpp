#include <iostream>

#include <ew/core/exception/exception.hpp>

#define EW_DEFINE_WRAPPER_FUNC_POINTER // !! do not remove this !!
#include "libfreetype2.hpp"

namespace ew
{
namespace graphics
{
namespace fonts
{

using namespace ew::core::exceptions;
using ew::core::dll;

static dll * _libfreetype2;

ew::core::dll::symbol _libfreetype2_symbol_table[] = {
#include "libfreetype2_sym_tab_entries.hpp"
	{0 , 0 }
};

bool load_libfreetype2_dll()
{
	const char * dllNames[] = { "libfreetype.so.6" };

	ew::core::disable_exceptions();

	for (unsigned int i = 0; i < sizeof(dllNames) / sizeof(dllNames[0]); ++i) {
		_libfreetype2 = ::new dll(dllNames[i]);
		if (_libfreetype2->load())
			break;
		delete _libfreetype2;
		_libfreetype2 = 0;
	}

	if (!_libfreetype2) {
		return false;
	}

	for (size_t i = 0;  _libfreetype2_symbol_table[i].name; ++i) {
		// try
		*(_libfreetype2_symbol_table[i].ptr) = _libfreetype2->symbol_by_name(_libfreetype2_symbol_table[i].name);
		// catch
		// extension not available ?
	}

	return true;
}

bool unload_libfreetype2_dll()
{
	::delete _libfreetype2;
	return true;
}

}
}
}
