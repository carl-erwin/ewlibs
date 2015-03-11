#include <iostream>

#include <ew/core/exception/exception.hpp>

#define EW_DEFINE_WRAPPER_FUNC_POINTER // !! do not remove this !!
#include "libGL.hpp"

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

using namespace ew::core::types;
using namespace ew::core::exceptions;


// add mutex ?
static ew::core::dll * _libGL = 0; //

ew::core::dll::symbol _libGL_symbol_table[] = {
#include "libGL_sym_tab_entries.hpp"
	{0 , 0 }
};

bool load_libGL_dll()
{
#ifdef WIN32
	const char * dllNames[] = { "opengl32.dll" };
#else
	const char * dllNames[] = { "libGL.so.1", "libGL.so" };
#endif

	ew::core::disable_exceptions();

	for (unsigned int i = 0; i < sizeof(dllNames) / sizeof(dllNames[0]); ++i) {
		_libGL = ::new ew::core::dll(dllNames[i]);
		if (_libGL->load())
			break;
		::delete _libGL;
		_libGL = 0;
	}

	if (!_libGL) {
		return false;
	}

	for (u32 i = 0;  _libGL_symbol_table[i].name; ++i) {
		// try
		*(_libGL_symbol_table[i].ptr) = _libGL->symbol_by_name(_libGL_symbol_table[i].name);
		// catch
		// extension not available ?
	}

	return true;
}

bool unload_libGL_dll()
{
	if (_libGL) {
		_libGL->unload();
		::delete _libGL;
		_libGL = 0;
	}
	return true;
}


}
}
}
}
}
