#include <windows.h>

#include <iostream>

#include <ew/core/threading/condition_variable.hpp>

#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/mutex_locker.hpp>

#include "./mutex_private_data.hpp"

namespace ew
{
namespace core
{
namespace threading
{

class condition_variable::private_data
{
public:
	private_data(mutex * mutex) : _mutex(mutex)
	{
	}

	~private_data()
	{
	}

	mutex * _mutex;
};


condition_variable::condition_variable(mutex * mutex)
	:
	d(new private_data(mutex))
{
}

condition_variable::~condition_variable()
{
	delete d;
}

bool condition_variable::set_mutex(mutex * mutex)
{
	d->_mutex = mutex;
	return true;
}

mutex * condition_variable::get_mutex() const
{
	return d->_mutex;
}

bool condition_variable::wait()
{
	return false;
}

bool condition_variable::timed_wait(size_t nrMilliSeconds)
{
	return false;
}

bool condition_variable::timed_wait_micro(size_t nrMicroSeconds)
{
	return false;
}

bool condition_variable::timed_wait_nano(size_t nrNanoSeconds)
{
	return false;
}

bool condition_variable::signal()
{
	return false;
}

bool condition_variable::broadcast()
{
	return false;
}

}
}
}
