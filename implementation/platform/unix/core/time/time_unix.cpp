
#include <iostream>
/* according to posix 1003.1-2001 */
#include <sys/select.h>


/* according to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h> // clock_gettime

#include <ew/core/time/time.hpp>
#include <ew/core/dll/dll.hpp>
#include <ew/core/threading/condition_variable.hpp>
#include <ew/core/threading/mutex.hpp>

#include "../syscall/syscall.hpp"

namespace ew
{
namespace core
{
namespace time
{

using namespace ew::core::types;
using namespace ew::core::threading;

using namespace ew::core::syscall::unix_system;

static struct timespec _t0;
static bool _time_was_init = false;

static size_t _t0_milli = 0;
static size_t _t0_micro = 0;
static size_t _t0_nano  = 0;


bool init(void)
{
	sys_clock_gettime(CLOCK_MONOTONIC, &_t0);

	_t0_milli = (size_t)((float)_t0.tv_sec * 1000.0f        + ((float)_t0.tv_nsec / 1000000.0f));
	_t0_micro = (size_t)((float)_t0.tv_sec * 1000000.0f     + ((float)_t0.tv_nsec / 1000.0f));
	_t0_nano  = (size_t)((float)_t0.tv_sec * 1000000000.0f  + ((float)_t0.tv_nsec / 1.0f));

	_time_was_init = true;
	return _time_was_init;
}


bool quit(void)
{
	_time_was_init = false;
	return true;
}

//  -----------------------------------------------

size_t get_seconds_since_startup(void)
{
	struct timespec _t1;

	sys_clock_gettime(CLOCK_MONOTONIC, &_t1);

	return _t1.tv_sec - _t0.tv_sec;
}

size_t get_milliseconds_since_startup(void)
{
	struct timespec _t1;

	sys_clock_gettime(CLOCK_MONOTONIC, &_t1);

	//  return ((_t1.tv_sec - _t0.tv_sec) * 1000) + ((_t1.tv_nsec - _t0.tv_nsec) / 1000000);

	return (size_t)((_t1.tv_sec * 1000.0f) + (_t1.tv_nsec / 1000000.0f)) - _t0_milli;
}

size_t get_microseconds_since_startup(void)
{
	struct timespec _t1;

	sys_clock_gettime(CLOCK_MONOTONIC, &_t1);

	//  return ((_t1.tv_sec - _t0.tv_sec) * 1000000) + ((_t1.tv_nsec - _t0.tv_nsec) / 1000);

	return (size_t)((_t1.tv_sec * 1000000.0f) + (_t1.tv_nsec / 1000.0f)) - _t0_micro;

}


size_t get_nanoseconds_since_startup(void)
{
	struct timespec _t1;

	sys_clock_gettime(CLOCK_MONOTONIC, &_t1);

	//  return ((_t1.tv_sec - _t0.tv_sec) * 1000000) + ((_t1.tv_nsec - _t0.tv_nsec) / 1000);

	return ((_t1.tv_sec * 1000000000) + _t1.tv_nsec) - _t0_nano;
}


size_t get_ticks(void)
{
	return get_milliseconds_since_startup();
}

void sleep(size_t nr_milliseconds)
{
	mutex mtx;
	condition_variable cond(&mtx);

	mtx.lock();
	cond.timed_wait(nr_milliseconds);
	mtx.unlock();
}

void usleep(size_t nr_microseconds)
{
	mutex mtx;
	condition_variable cond(&mtx);

	mtx.lock();
	cond.timed_wait_micro(nr_microseconds);
	mtx.unlock();
}

void nsleep(size_t nr_nanoseconds)
{
	mutex mtx;
	condition_variable cond(&mtx);

	mtx.lock();
	cond.timed_wait_nano(nr_nanoseconds);
	mtx.unlock();
}


}
}
}
