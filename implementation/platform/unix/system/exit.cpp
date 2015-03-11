#include <stdlib.h>

#include <ew/core/types/types.hpp>
#include <ew/system/system.hpp>

namespace ew
{
namespace system
{

void exit(ssize_t status)
{
	::exit(static_cast<int>(status));
}

}
}
