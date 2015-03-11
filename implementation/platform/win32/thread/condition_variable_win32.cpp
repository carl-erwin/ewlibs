#include <windows.h>

#include <iostream>

#include <ew/core/threading/condition_variable.hpp>

#include <ew/core/mutex/mutex.hpp>
#include <ew/core/mutex/mutex_locker.hpp>

#include "../mutex/mutex_private_data.hpp"

namespace ew
{
namespace Core
{
namespace THREAD
{

using namespace ew::core::MUTEX;

class ConditionVariable::private_data
{
public:
	private_data(Mutex * mutex) : _mutex(mutex)
	{
	}

	~private_data()
	{
	}

	mutex * _mutex;
};


condition_variable::ConditionVariable(Mutex * mutex)
	:
	d(new private_data(mutex))
{
}

condition_variable::~ConditionVariable()
{
	delete d;
}

bool ConditionVariable::setMutex(Mutex * mutex)
{
	d->_mutex = mutex;
	return true;
}

mutex * ConditionVariable::getMutex() const
{
	return d->_mutex;
}

bool ConditionVariable::wait()
{
	return false;
}

bool ConditionVariable::timedWait(u32 nrMilliSeconds)
{
	return false;
}

bool ConditionVariable::timedWaitMicro(u32 nrMicroSeconds)
{
	return false;
}

bool ConditionVariable::timedWaitNano(u32 nrNanoSeconds)
{
	return false;
}

bool ConditionVariable::signal()
{
	return false;
}

bool ConditionVariable::broadcast()
{
	return false;
}

}
}
}
