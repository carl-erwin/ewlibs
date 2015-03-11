#include <cstring>
#include <stdio.h>

#include <ew/console/console.hpp>
#include <ew/core/exception/exception.hpp>

namespace ew
{
namespace console
{

// globals
//EW_CONSOLE_EXPORT
console cin(STDIN);
//EW_CONSOLE_EXPORT
console cout(STDOUT);
//EW_CONSOLE_EXPORT
console cerr(STDERR);

class console::private_data
{
public:
	private_data(ew::console::type type)
		:
		_type(type)
	{

	}


	~private_data()
	{
	}

	ew::console::type _type;
};

console::console(ew::console::type type)
	: d(::new private_data(type))
{

}

console::~console()
{
	delete d;
}


console & console::operator<< (const char c)
{
	return *this;
}

console & console::operator<< (const char * s)
{
	return *this;
}

console & console::operator<< (const s8 v)
{
	return *this;
}

console & console::operator<< (const u8 v)
{
	return *this;
}

console & console::operator<< (const s16 v)
{
	return *this;
}

console & console::operator<< (const u16 v)
{
	return *this;
}

console & console::operator<< (const s32 v)
{
	return *this;
}

console & console::operator<< (const u32 v)
{
	return *this;
}


console & console::operator<< (const s64 v)
{
	return *this;
}

console & console::operator<< (const u64 v)
{
	return *this;
}


console & console::operator<< (const f32 v)
{
	return *this;
}

console & console::operator<< (const double v)
{

	return *this;
}

// ------------------------------------------------

bool console::write(const char * str, u32 len)
{
//  ::write( d->fd, str, len );
	return true;
}

// ---------------------------------------------------



}
}
