#include <ew/Utils.hpp>

#include <ew/core/exception/exception.hpp>

#include <ew/core/types/types.hpp>

#include <cstdlib>
#include <cstring>

namespace ew
{
namespace core
{

using namespace ew::core::types;

static TLS_DECL bool exceptions_enabled = false;

void enable_exceptions()
{
	exceptions_enabled = true;
}

void disable_exceptions()
{
	exceptions_enabled = false;
}

bool are_exceptions_enabled()
{
	return exceptions_enabled;
}

class exception::private_data
{
public:
	private_data(const char * name)
		:
		_exception_name(ew::utils::c_string_dup(name))
	{
	}

	~private_data()
	{
		::delete [] _exception_name;
	}

	char * _exception_name;

private:
	private_data(const private_data & orig);
	private_data & operator=(const private_data & orig);
};

exception::exception(const char * name)
	:
	d(::new private_data(name))
{

}

exception::exception(const exception & orig)
	:
	d(::new private_data(orig.d->_exception_name))
{

}

exception & exception::operator=(const exception & orig)
{
	if (this != &orig) {
		delete d;
		d = new private_data(orig.d->_exception_name);
	}
	return *this;
}

exception::~exception()
{
	::delete d;
}

const char * exception::get_name() const
{
	return d->_exception_name;
}

const char * exception::get_class_name() const
{
	return "ew::core::exception";
}

const char * exception::what() const
{
	return d->_exception_name;
}

}
}
