#pragma once

// lang c++
#include <new>
#include <iostream>

// system
#include <pthread.h>
#include <errno.h>

// ew
#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/exception/exception.hpp>
#include <ew/core/threading/mutex.hpp>

// implementation
#include "../../core/syscall/syscall.hpp"


namespace ew
{
namespace core
{
namespace threading
{

using namespace ew::core::types;

class mutex::private_data
{
public:
	pthread_mutex_t _pth_mutex;
	pthread_mutexattr_t _pth_mutex_attr;
	pthread_mutex_t _internal_lock;

	thread * _owner;
	type _type;

	private_data()
		:
		_owner(0)
	{
	}

	~private_data()
	{
		_owner = 0;
	}

private:
	private_data(const private_data &);
	private_data & operator=(const private_data &);
};

}
}
}
