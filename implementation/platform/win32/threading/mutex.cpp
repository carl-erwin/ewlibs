// lang c++
#include <new>
#include <iostream>

// system
#include <windows.h>

// ew
#include <ew/ew_config.hpp>

#include <ew/core/types/types.hpp>
#include <ew/core/threading/mutex.hpp>

#include "mutex_private_data.hpp"

namespace ew
{
namespace core
{
namespace threading
{

using namespace ew::core::types;

mutex::mutex(mutex::type t) : d(new private_data()) { }

mutex::~mutex()
{
	delete d;
}

bool mutex::lock()
{
	DWORD ret = WaitForSingleObject(d->_win32_mutex, INFINITE);
	return (ret == WAIT_OBJECT_0) ?  true : false;
}


bool mutex::trylock()
{
	DWORD ret = WaitForSingleObject(d->_win32_mutex, 1);
	return (ret == WAIT_OBJECT_0) ?  true : false;
}


bool mutex::unlock()
{
	BOOL ret = ReleaseMutex(d->_win32_mutex);
	return (ret == TRUE) ?  true : false;
}



}
}
}
