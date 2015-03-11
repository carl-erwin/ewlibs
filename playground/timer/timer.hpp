#pragma once

#include "ew/core/Types.hpp"

/*
   TODO : add max sleep for wait
   add stats : when _last return to begin() t1 -t0 = time to process queue
*/

using namespace ew::core::types;

namespace test
{

namespace time
{


class TimerHandler;

class Timer
{
public:
	Timer(u32 freq);

	virtual ~Timer();
	virtual u32  frequency();
	virtual void setFrequency(u32 val);
	virtual u32  isPeriodic();
	virtual void setPeriodic(u32 val);
	virtual u32  expireAt();

public:
	// action
	virtual void timeout() = 0;

private:
	friend class TimerHandler;
	u32 _repeat: 1;
	//u32 _remove:1;
	u32 _freq: 30;
	u32 _timeout;
};

class TimerHandler
{
public:
	TimerHandler();
	virtual ~TimerHandler();
	bool addTimer(Timer * t);
	bool removeTimer(Timer * t);
	u32  wait();

private:
	class Private;
	Private * d;
};


} // ! namespace time

} // ! namespace test

