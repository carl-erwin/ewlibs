
#include <windows.h>

#include <ew/ew_config.hpp>
#include <ew/core/time/time.hpp>


namespace ew
{
namespace Core
{
namespace TIME
{

bool init()
{
	return true;
}
bool quit()
{
	return true;
}

u32 get_seconds_since_startup(void)
{
	return 0;
}
u32 get_milliseconds_since_startup(void)
{
	return 0;
}
u32 getMicroSecondsSinceStartUp(void)
{
	return 0;
}
u32 get_ticks(void)
{
	return GetTickCount();
}

void sleep(u32 nrMilliseconds)
{
	::Sleep(nrMilliseconds);
}
void usleep(u32 nrMicroSeconds) { }
void nsleep(u32 nrNanoSeconds) {}

}
}
}
