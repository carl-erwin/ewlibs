#include <iostream>

#include <ew/core/exception/exception.hpp>


#ifdef EW_TARGET_SYSTEM_WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#define EW_DEFINE_WRAPPER_FUNC_POINTER
#include "libGLU.hpp"

namespace ew
{
namespace implementation
{
namespace graphics
{
namespace rendering
{
namespace opengl
{

using namespace ew::core::exceptions;
using ew::core::dll;


/*
  TODO: refactor dll loading
  ptr_table = load(symbol_table)
*/

// add mutex ?
static dll * _libGLU = 0;

dll::symbol _libGLU_symbol_table[] = {
#include "libGLU_sym_tab_entries.hpp"
	{0 , 0 }
};

bool load_libGLU_dll()
{
	_libGLU = ::new dll("libGLU.so");

	ew::core::enable_exceptions();
	{
		_libGLU->load();

		for (size_t i = 0;  _libGLU_symbol_table[i].name; ++i) {
			// try
			*(_libGLU_symbol_table[i].ptr) = _libGLU->symbol_by_name(_libGLU_symbol_table[i].name);
			// catch
			// extension not available ?
		}
	}
	ew::core::disable_exceptions();

	return true;
}

bool unload_libGLU_dll()
{
	if (_libGLU)
		_libGLU->unload();
	::delete _libGLU;
	return true;
}


}
}
}
}
}
