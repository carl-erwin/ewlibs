// under linux and *BSD
// put in Ew_config.h ?
# include <dlfcn.h>

#include <iostream>

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/dll/dll.hpp>

#include <ew/utils/utils.hpp>


#include <ew/console/console.hpp>

namespace ew
{
namespace core
{

using ew::console::cerr;

// -----------------------------------------------------

class dll::private_data
{
public:
	private_data(const char * name)
	{
		_fileName = ew::utils::c_string_dup(name);
	}

	~private_data()
	{
		::delete [] _fileName;
		_fileName = 0;
		_handler = 0;
	}

	char * _fileName = 0;
	void * _handler  = 0;
};

// ------------------------------------------------------------------------------


dll::dll(const char * name)
	:
	d(::new private_data(name))
{
	set_name(name);
}


bool dll::load()
{
	if (d->_handler)
		return true;

	d->_handler = ::dlopen(d->_fileName, RTLD_NOW);
	if (!d->_handler) {
		cerr << "::dlerror() : " << ::dlerror() << "\n";
		ew::Throw(dll_loading_error());

		return false;
	}
	return true;
}

bool dll::unload()
{
	if (d->_handler == 0)
		return true;

	if (::dlclose((void *) d->_handler)) {
		cerr << "::dlerror() : " << ::dlerror() << "\n";
		ew::Throw(dll_unloading_error());

		return false;
	}

	d->_handler = 0;
	return true;
}

dll::~dll()
{
	unload();
	delete d;
}

const char * dll::filename()
{
	return 0;
}

const char * dll::class_name() const
{
	return "ew::core::dll";
}


bool dll::is_loaded(void)
{
	return (d->_handler != 0) ? true : false;
}

void * dll::symbol_by_name(const char * _symbol)
{
	void * sym = ::dlsym((void *) d->_handler, _symbol);
	if (!sym) {
		std::cerr << "cannot found symbol '" << _symbol << "' in '" << name() << "' dll\n";
		ew::Throw(dll_symbol_not_found());
	}

	return (sym);
}

// sub system init
bool init(void)
{
	return true;
}

bool quit(void)
{
	return true;
}

}
} // !
