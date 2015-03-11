// lang c++
#include <new>
#include <iostream>

// system
#include <windows.h>

// ew
#include <ew/ew_config.hpp>

#include <ew/core/types/types.hpp>
#include <ew/core/mutex/mutex.hpp>

#include "mutex_private_data.hpp"

namespace ew
{
namespace Core
{
namespace MUTEX
{

using namespace ew::core::types;

mutex::Mutex() : d(new private_data()) { }

mutex::~Mutex()
{
	delete d;
}

boolmutex::isLocked()
{
	if (trylock() == true) {
		unlock();
		return false;
	}
	return true;
}


boolmutex::lock()
{
	DWORD ret = WaitForSingleObject(d->_win32_mutex, INFINITE);
	return (ret == WAIT_OBJECT_0) ?  true : false;
}


boolmutex::trylock()
{
	DWORD ret = WaitForSingleObject(d->_win32_mutex, 1);
	return (ret == WAIT_OBJECT_0) ?  true : false;
}


boolmutex::unlock()
{
	BOOL ret = ReleaseMutex(d->_win32_mutex);
	return (ret == TRUE) ?  true : false;
}



}
}
}
