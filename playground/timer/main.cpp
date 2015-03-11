

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <list>

#include "timer.hpp"

#include "ew/core/Time.hpp"

namespace test
{

namespace time
{

class TimerTest : public Timer
{
public:
	TimerTest(u32 val, u32 num)
		:
		Timer(val)
	{
		setPeriodic(1);
		_num = num;
		_t_old = ew::core::time::get_ticks();
		_fps = 0;
	}

	virtual ~TimerTest() {}

	virtual void timeout()
	{
		// std::cerr << " in callback timeout... " << "\n";
		++_fps;
		u32 t = ew::core::time::get_ticks();
		static float coef = 3.0f;
		if (t - _t_old >= 1000 * coef) {
			// std::cerr <<  "num = " << _num << ", fps = " << _fps/(coef) << ",  @ time = " << t << "\n";
			_fps = 0;
			_t_old = t;
		}

		// call obj->onTimerEvent(TimerEvent);
	}

public:
	u32 _num;
	u32 _fps;
	u32 _t_old;
};

int main(int ac, char ** av)
{
	int max_timers = 10;
	int timers_freq = 100;
	int periodic = 1;

	if (ac >= 2) {
		max_timers = ::atoi(av[1]);
	}
	if (ac >= 3) {
		timers_freq = ::atoi(av[2]);
	}
	if (ac >= 4) {
		periodic = (::atoi(av[3]) != 0);
	}

	ew::core::time::init();
	std::vector<TimerTest *> tvec;
	TimerHandler timeHandler;

	std::cerr << "adding timers " << max_timers << " @ freq " << timers_freq  << "...";
	for (int i = 0; i < max_timers; i++) {
		TimerTest * t = new TimerTest(1, i);
		t->setFrequency(timers_freq);
		t->setPeriodic(periodic);
		timeHandler.addTimer(t);
		tvec.push_back(t);
	}
	std::cerr << "...ok\n";

	std::cerr << "loop\n";
	u32 count = 0;
	u32 t0 = ew::core::time::get_ticks();
	while (true) {
		count += timeHandler.wait();
		u32 t1 = ew::core::time::get_ticks();
		if (t1 - t0 > 1000) {
			std::cerr << "nr timer per/s = " << count << "\n";
			if (max_timers) {
				std::cerr << "nr timer call per/s = " << count / max_timers << "\n";
			}
			t0 = t1;
			count = 0;
		}
	}

	for (int i = 0; i < 1; i++) {
		delete tvec[i];
	}

	return 0;
}

}
}

int main(int ac, char ** av)
{
	return test::time::main(ac, av);
}
