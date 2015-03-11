#pragma once

// lang c++
#include <new>
#include <iostream>

// system
#include <windows.h>

// ew
#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/mutex/mutex.hpp>

#include <ew/core/exception/exception.hpp>

namespace ew
{
namespace Core
{
namespace MUTEX
{

using namespace ew::core::types;

classmutex::private_data {
public:
	HANDLE  _win32_mutex;
	bool    _isInitialised;

	private_data() : _isInitialised(false)
	{
		_win32_mutex = CreateMutex((LPSECURITY_ATTRIBUTES)NULL, FALSE /* bInitialOwner */, (LPCTSTR)0 /* mutex has no name */);
		if (_win32_mutex == NULL) {
			std::cerr << "Can't Create mutex\n";
//          ew::Throw( ew::core::exceptions::ObjectCreationError() );
			return ;
		}
		_isInitialised = true;
	}

	~private_data()
	{
		if (_isInitialised == true) {
			BOOL ret = ReleaseMutex(_win32_mutex);
			if (ret != TRUE)
				return ; // throw
			CloseHandle(_win32_mutex);
		}
		_isInitialised = false;
	}
};


// ----------------------------------------------------------------
}
}
}
