#include <iostream>

#include <ew/core/exception/exception.hpp>

#define EW_DEFINE_WRAPPER_FUNC_POINTER
#include "x11_libX11.hpp"

namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::exceptions;
using ew::core::dll;

static dll * _libX11 = 0;

dll::symbol x11_dll_symbol_table[] = {
#include "libX11_sym_tab_entries.hpp"
	{0 , 0 }
};

bool load_libX11_dll()
{
	_libX11 = ::new dll("libX11.so");

	// TODO : pushExceptionState // ctor
	ew::core::enable_exceptions();
	{
		_libX11->load();

		for (size_t i = 0;  x11_dll_symbol_table[i].name; ++i) {
			// try
			*(x11_dll_symbol_table[i].ptr) = _libX11->symbol_by_name(x11_dll_symbol_table[i].name);
			// catch
			// extension not available ?
		}
	}
	ew::core::disable_exceptions();
	// TODO : popExceptionState

	return true;
}


bool unload_libX11_dll()
{
	::delete _libX11;
	_libX11 = 0;
	return true;
}


}
}
}
