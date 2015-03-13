#include <chrono>
#include <iostream>
#include <mutex>
#include <condition_variable>


/* according to posix 1003.1-2001 */
#include <sys/select.h>


/* according to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h> // clock_gettime

#include <ew/core/time/time.hpp>
#include <ew/core/dll/dll.hpp>


#include "../syscall/syscall.hpp"

namespace ew
{
namespace core
{
namespace time
{

using namespace ew::core::types;


static bool _time_was_init = false;

std::chrono::steady_clock::time_point _start;

bool init(void)
{
	_start = std::chrono::steady_clock::now();
	_time_was_init = true;
	return _time_was_init;
}


bool quit(void)
{
	_time_was_init = false;
	return true;
}

//  -----------------------------------------------


  size_t get_seconds_since_startup()
  {
        std::chrono::steady_clock::time_point _end = std::chrono::steady_clock::now();
    return  std::chrono::duration_cast<std::chrono::seconds>(_end - _start).count();
  }

    size_t get_milliseconds_since_startup()
  {
        std::chrono::steady_clock::time_point _end = std::chrono::steady_clock::now();
    return  std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
  }

    size_t get_nanoseconds_since_startup()
  {
        std::chrono::steady_clock::time_point _end = std::chrono::steady_clock::now();
    return  std::chrono::duration_cast<std::chrono::nanoseconds>(_end - _start).count();
  }


  

size_t get_ticks(void)
{
    std::chrono::steady_clock::time_point _end = std::chrono::steady_clock::now();
    return  std::chrono::duration_cast<std::chrono::microseconds>(_end - _start).count();
}

void sleep(size_t nr_milliseconds)
{
        std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);
	
	std::condition_variable cond;

	auto now = std::chrono::system_clock::now();
	cond.wait_until(lock, now + std::chrono::milliseconds(nr_milliseconds));
}

void usleep(size_t nr_microseconds)
{
        std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);
	
	std::condition_variable cond;
	auto now = std::chrono::system_clock::now();
	cond.wait_until(lock, now + std::chrono::microseconds(nr_microseconds));
}

void nsleep(size_t nr_nanoseconds)
{
        std::mutex mtx;
	std::unique_lock<std::mutex> lock(mtx);

	std::condition_variable cond;
	auto now = std::chrono::system_clock::now();
	cond.wait_until(lock, now + std::chrono::nanoseconds(nr_nanoseconds));
}


}
}
}
