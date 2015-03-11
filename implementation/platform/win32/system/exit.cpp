#include <ew/system/system.hpp>

#include <windows.h>

namespace ew
{
namespace SYSTEM
{

using namespace ew::core::types;

void exit(s32 status)
{
	::exit(int(status));
}
}
}
