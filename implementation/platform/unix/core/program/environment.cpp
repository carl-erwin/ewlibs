#include <stdlib.h>
#include <iostream>
#include <ew/core/program/environment.hpp>

namespace ew
{
namespace core
{
namespace program
{

char * getenv(const char * var)
{
	char * value = ::getenv(var);
	return value;
}

bool setenv(char * var, char * value)
{
	return  ::setenv(var, value, 0 /* !overwrite*/) == 0;
}

bool unsetenv(char * var)
{
	return ::unsetenv(var) == 0;
}

}
}
}
