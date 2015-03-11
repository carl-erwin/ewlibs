#include <sys/time.h>
#include <time.h>

#include <pthread.h>

#include <iostream>

#include <ew/core/threading/thread.hpp>
#include <ew/core/threading/condition_variable.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/mutex_locker.hpp>

#include "../syscall/syscall.hpp"

#include "mutex_private_data.hpp"

#include <errno.h>

namespace ew
{
namespace core
{
namespace threading
{

using namespace ew::core::syscall::unix_system;

class condition_variable::private_data
{
public:
	private_data(mutex * mutex) : _mutex(mutex) {}

	~private_data()
	{

	}

	mutex * _mutex;
	pthread_cond_t _cond;
	// add pthread_cond_attr_t to use MONOTONIC CLOCKID
};



condition_variable::condition_variable(mutex * mutex)
	:
	d(new private_data(mutex))
{
	if (pthread_cond_init(&d->_cond, NULL) != 0) {
		throw exceptions::object_creation_error();
	}
}

condition_variable::~condition_variable()
{
	if (pthread_cond_destroy(&d->_cond) != 0) {
		// EBUSY :
		// throw Object Early Destruction
	}
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

// TODO: we must check if timeout ?
// TODO: restart if EGAIN or EINTR
#define EW_DO_TIMEWAIT()                                                \
    do {                                                                  \
        thread * current = ew::core::threading::self();                     \
        int ret = sys_pthread_cond_timedwait(&d->_cond,                     \
                                             &d->_mutex->d->_pth_mutex, &ti); \
        d->_mutex->d->_owner = current;                                     \
        return (ret == 0);                                                  \
    } while (0)


bool condition_variable::wait()
{
	thread * current = ew::core::threading::self();
	d->_mutex->d->_owner = 0;
	int ret  = sys_pthread_cond_wait(&d->_cond, &d->_mutex->d->_pth_mutex);
	d->_mutex->d->_owner = current;
	return (ret == 0);
}

// not safe against time jump ?
bool condition_variable::timed_wait(size_t nrMilliSeconds)
{
	struct timespec ti;
	struct timespec ti_now;

	sys_clock_gettime(CLOCK_REALTIME, &ti_now);

	ti.tv_nsec  = ti_now.tv_nsec + (nrMilliSeconds % 1000) * 1000000;
	ti.tv_sec   = ti_now.tv_sec  + (nrMilliSeconds / 1000) + (ti.tv_nsec / 1000000000);
	ti.tv_nsec %= 1000000000;

	EW_DO_TIMEWAIT();
}

bool condition_variable::timed_wait_micro(size_t nrMicroSeconds)
{

	struct timespec ti;

	struct timespec ti_now;

	sys_clock_gettime(CLOCK_REALTIME, &ti_now);

	ti.tv_nsec = ti_now.tv_nsec + (nrMicroSeconds % 1000000) * 1000;
	ti.tv_sec  = ti_now.tv_sec  + (nrMicroSeconds / 1000000) + (ti.tv_nsec / 1000000000);
	ti.tv_nsec %= 1000000000;

	EW_DO_TIMEWAIT();
}

bool condition_variable::timed_wait_nano(size_t nrNanoSeconds)
{

	struct timespec ti;

	struct timespec ti_now;

	sys_clock_gettime(CLOCK_REALTIME, &ti_now);

	ti.tv_nsec =  ti_now.tv_nsec + (nrNanoSeconds % 1000000000);
	ti.tv_sec  =  ti_now.tv_sec  + (nrNanoSeconds / 1000000000) + (ti.tv_nsec / 1000000000);
	ti.tv_nsec %= 1000000000;

	EW_DO_TIMEWAIT();
}


bool condition_variable::signal()
{
	if (sys_pthread_cond_signal(&d->_cond) != 0) {
		return false;
	}

	return true;
}

bool condition_variable::broadcast()
{
	if (sys_pthread_cond_broadcast(&d->_cond) != 0) {
		return false;
	}

	return true;
}

}
}
}
