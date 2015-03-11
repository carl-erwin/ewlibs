#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

namespace ew
{
namespace core
{
namespace time
{

using namespace ew::core::types;

// idate.hpp
/*
  all ops on date ? GMT

  IDate
*/

// -----------------------------------------------------------

class EW_CORE_TIME_EXPORT ITimer
{
public:
	ITimer() {}
	virtual ~ITimer() {}

	virtual void start()    = 0; /* start the timer */
	virtual void stop()     = 0;
	virtual u32  getValue() = 0;
	virtual void reset()    = 0;
};

// -----------------------------------------------------------

// ialarm.hpp
class EW_CORE_TIME_EXPORT IAlarm
{
public:
	IAlarm() {}
	virtual ~IAlarm() {}

	bool registerCallback(void (*callback)(void));        /* */

	virtual void start() = 0; /* start the timer */
	virtual void stop() = 0;
	virtual u32 getValue() = 0;
	virtual void reset() = 0;
};


// -----------------------------------------------------------


}
}
} // ! ew::core::time
