#include <assert.h>

// lang c++
#include <new>
#include <iostream>

// system
#include <pthread.h>

// ew
#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/thread.hpp>

// internal data used by condition variable ?
#include "mutex_private_data.hpp"

/*
  UNIX mutex ( POSIX )
  used by *bsd, linux ...
*/

namespace ew
{
namespace core
{
namespace threading
{

using namespace ew::core::types;
using namespace ew::core::syscall::unix_system;
using namespace ew::core::exceptions;


mutex::mutex(mutex::type t)
	:
	d(::new private_data())
{
	int ret;

	ret = sys_pthread_mutexattr_init(&d->_pth_mutex_attr);
	if (ret != 0) {
		throw object_creation_error();
		return ;
	}

	d->_type = t;

	int mtx_type = PTHREAD_MUTEX_NORMAL;
	switch (d->_type) {
	case  normal_mutex: {
		mtx_type = PTHREAD_MUTEX_NORMAL;
	}
	break ;
	case  recursive_mutex: {
		mtx_type = PTHREAD_MUTEX_RECURSIVE;
	}
	break ;
	case  error_check: {
		mtx_type = PTHREAD_MUTEX_ERRORCHECK;
	}
	break ;
	}

	ret = sys_pthread_mutexattr_settype(&d->_pth_mutex_attr, mtx_type);
	if (ret != 0) {
		throw object_creation_error();
		return ;
	}

	ret = sys_pthread_mutex_init(&(d->_pth_mutex), (const pthread_mutexattr_t *)0);
	if (ret != 0) {
		throw object_creation_error();
		return ;
	}
}

mutex::~mutex()
{
	assert(d);
	int ret = sys_pthread_mutex_destroy(&d->_pth_mutex);
	if (ret != 0) {
		throw object_destruction_error();
	}
	delete d;
}


mutex::type mutex::get_type()
{
	assert(d);
	return d->_type;
}

thread * mutex::is_locked_by_thread()
{
	assert(d);
	return d->_owner;
}

bool mutex::trylock()
{
	assert(d);
	assert(0);
	int ret = sys_pthread_mutex_trylock(&(d->_pth_mutex));
	if (ret != 0) {
		return false;
	}
	d->_owner = ew::core::threading::self();
	return true;
}

bool mutex::lock()
{
	assert(d);
	int ret = sys_pthread_mutex_lock(&(d->_pth_mutex));
// cannot use this assert if we did not create an ew::core::thread ....
//    assert((d->_type == ew::core::threading::mutex::recursive_mutex) || (d->_owner != ew::core::threading::self()));
	assert(ret == 0);
	if (ret != 0) {
		return false;
	}
//    assert((d->_type == ew::core::threading::mutex::recursive_mutex) || (d->_owner == 0));
	d->_owner = ew::core::threading::self();
	return true;
}

bool mutex::unlock()
{
//    assert(d->_owner == ew::core::threading::self());
	int ret = sys_pthread_mutex_unlock(&(d->_pth_mutex));
	assert(ret == 0);
	if (ret != 0) {
		assert(0);
		return false;
	}
	d->_owner = nullptr;
	return true;
}

}
}
}
