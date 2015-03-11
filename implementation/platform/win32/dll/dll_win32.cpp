// system include(s)
#include <iostream>
#include <windows.h>

// ew include(s)
#include <ew/ew_config.hpp>

#include <ew/core/dll/dll.hpp>


namespace ew
{
namespace core
{

class dll::private_data
{
public:
	private_data(const char * name)
		:
		_fileName(0),
		_handler(0)
	{
		size_t len = strlen(name);
		_fileName = new char[len + 1];
		strncpy(_fileName, name, len);
		_fileName[len] = 0;
	}

	~private_data()
	{
		delete [] _fileName;
		_fileName = 0;
		::FreeLibrary(_handler);
	}

public:
	char  * _fileName;
	HMODULE  _handler;

private:
	private_data(const private_data &);
	private_data & operator=(const private_data &);
};

// Ctors/Dtor

dll::dll(const char * name)
	:
	d(::new private_data(name))
{
}

const char * dll::class_name() const
{
	return "dLL";
}

dll::~dll()
{
	::delete d;
}

bool dll::load()
{
	d->_handler = ::LoadLibrary(d->_fileName);

	if (!d->_handler) {
		std::cerr << "dll error()" << std::endl;
		return false;
	}

	return true;
}

bool dll::unload()
{
	::FreeLibrary(d->_handler);
	// add test
	return true;
}

const char * dll::name() const
{
	return d->_fileName;
}

bool  dll::is_loaded(void)
{
	return (d->_handler != 0) ? true : false;
}

void  * dll::symbol_by_name(const char * symbol)
{
	void * sym = 0;

	sym = ::GetProcAddress((HMODULE)d->_handler, symbol);
	if (!sym)
		std::cerr << "dll error()" << std::endl;

	return (sym);
}

}
} //
