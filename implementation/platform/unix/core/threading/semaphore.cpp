#include <pthread.h>

#ifndef _XOPEN_SOURCE
# define _XOPEN_SOURCE 600 // for sem_timedwait
#endif

#include <semaphore.h>
#include <errno.h>
#include <unistd.h>

#include <ew/core/threading/semaphore.hpp>

namespace ew
{
namespace core
{
namespace threading
{

using namespace ew::core::types;

class semaphore::private_data
{
public:
	private_data()
		:
		initOk(false),
		inUse(false)
	{

	}

	sem_t sem;
bool initOk:
	1;
bool inUse:
	1;
};

semaphore::semaphore(size_t value)
	: d(new private_data())
{
	if (sem_init(&d->sem, 0, value) < 0) {
		//         ew::Throw( ObjectCreationError() );
		return ;
	}
	d->inUse = true;
	d->initOk = true;
}

semaphore::~semaphore()
{
	release();
	delete d;
}

bool semaphore::release()
{
	if (d->initOk == false)
		return true;

	int ret = sem_destroy(&d->sem);
	if (ret < 0 && errno == EBUSY)
		return false;
	if (ret < 0)
		return false;

	d->inUse = false;
	return true;
}

u32  semaphore::value()
{
	int sval;
	if (sem_getvalue(&d->sem, &sval) < 0)
		return 0;

	return static_cast<u32>(sval);
}

bool  semaphore::wait()
{
	int ret;
	while (((ret = sem_wait(&d->sem)) == -1) && (errno == EINTR))
		continue ;

	if (ret < 0)
		return false;

	return true;
}

bool  semaphore::try_wait()
{
	int ret;
	while (((ret = sem_trywait(&d->sem)) == -1) && (errno == EINTR))
		continue ;

	if (ret < 0)
		return false;

	return true;
}

bool  semaphore::timed_wait()
{

	return true;
}

bool  semaphore::post()
{
	int ret = sem_post(&d->sem);
	if (ret < 0)
		return false;

	return true;
}

}
}
}
