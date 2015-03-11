#include <ew/console/console.hpp>
#include <ew/core/exception/exception.hpp>

#define EW_DEFINE_WRAPPER_FUNC_POINTER // !! do not remove this !!
#include "x11_libGLX.hpp"

#include <cstring>

namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::exceptions;
using ew::core::dll;
using ew::console::cerr;

static dll * _libGLX;

ew::core::dll::symbol _libGLX_symbol_table[] = {
#include "x11_libGLX_sym_tab_entries.hpp"
	{0 , 0 }
};

bool load_libGLX_dll()
{
	//TODO: generic loadWrappers(Dll *dll, Symbol [], bool use_execption, char * [] sub_loaders_name )

	const char * dllNames[] = { "libGL.so.1", "libGL.so" };

	ew::core::disable_exceptions();

	for (unsigned int i = 0; i < sizeof(dllNames) / sizeof(dllNames[0]); ++i) {
		_libGLX = ::new dll(dllNames[i]);
		if (_libGLX->load())
			break;
		delete _libGLX;
		_libGLX = 0;
	}

	if (!_libGLX) {
		return false;
	}

	// load subloader first
	const char * subloader_name = "glXGetProcAddress";
	void *(*subloader_ptr)(const char * symname) = 0;
	subloader_ptr = reinterpret_cast<void *( *)(const char *)>(_libGLX->symbol_by_name(subloader_name));

	for (size_t i = 0;  _libGLX_symbol_table[i].name; ++i) {

		// try
		if (subloader_ptr) {
			*(_libGLX_symbol_table[i].ptr) = reinterpret_cast<void *>((*subloader_ptr)(_libGLX_symbol_table[i].name));
		} else {
			*(_libGLX_symbol_table[i].ptr) = _libGLX->symbol_by_name(_libGLX_symbol_table[i].name);
		}
		// catch
		// extension not available ?

		if (*(_libGLX_symbol_table[i].ptr) == 0) {
			cerr << "cannot load '" << _libGLX_symbol_table[i].name << "\n";
		}
	}

	return true;
}

bool unload_libGLX_dll()
{
	::delete _libGLX;
	return true;
}

}
}
}
