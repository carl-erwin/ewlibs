#include <cassert>
#include <vector>
#include <list>

#include "ew/core/threading/condition_variable.hpp"
#include "ew/core/Time.hpp"
#include "ew/maths/functions.hpp" // form min

#include "timer.hpp"


using namespace ew::core::time;
using namespace ew::core::threading;

namespace test
{
namespace time
{


Timer::Timer(u32 freq)
{
	_freq = freq;
	_repeat = 0;
}

Timer::~Timer()
{

}

u32  Timer::frequency()
{
	return _freq;
}

void Timer::setFrequency(u32 val)
{
	_freq = val;
}

u32  Timer::isPeriodic()
{
	return _repeat;
}

void Timer::setPeriodic(u32 val)
{
	_repeat = val;
}

u32  Timer::expireAt()
{
	return _timeout;
}


///

class TimerHandler::Private
{
public:

	Private()
	{
		timeout_cond = new condition_variable(&timeout_mutex);
		_sleep = 1000; // TODO

		periodict_timers_1ms.is_periodic = true;
		periodict_timers_1ms.is_circular = true;

		periodict_timers_10ms.is_periodic = true;
		periodict_timers_10ms.is_circular = false;

		simple_timers.is_periodic = false;
		simple_timers.is_circular = false;
	}

	~Private()
	{
		delete timeout_cond;
	}

	template <class Container>
	struct Queue {
		Queue()
		{
			_last = _timers.begin();
			is_periodic = false;
			is_circular = false;
		}
		Container _timers;
		typename Container::iterator _last;
		bool is_periodic;
		bool is_circular;
	};

	Queue< std::vector<Timer *> > periodict_timers_1ms;
	Queue< std::vector<Timer *> > periodict_timers_10ms;
	Queue< std::vector<Timer *> > periodict_timers_100ms;
	Queue< std::list<Timer *> > simple_timers;
	mutex timeout_mutex;
	condition_variable  * timeout_cond;
	u32 _sleep;

	template <class Container>
	bool refresh_iterator(Container & container, typename Container::iterator & it, int pos)
	{
		return true;
	}

	template <class Container>
	void pop_head(Container & container) { }

	template <class Container>
	bool addTimer(Queue<Container> & queue, Timer * t);

	bool addTimer(Timer * t);

	template <class Container>
	u32 processQueue(Queue<Container> & queue)
	{
		//    std::cerr << __FUNCTION__ << " BEGIN\n";

		u32 count = 0;
		Timer * t = 0;

		u32 t0 = ew::core::time::get_ticks();
		u32 t1;
		while (true) {

			if (!queue._timers.size()
			    || (queue._last == queue._timers.end())) {
				t1 = ew::core::time::get_ticks();
				break;
			}

			t = *(queue._last);

			// std::cerr << "queue.size  = " << queue._timers.size() << "\n";
			// std::cerr << "queue.is_periodic = " << queue.is_periodic << "\n";
			// std::cerr << "queue.is_circular = " << queue.is_circular << "\n";

			u32 exp = t->_timeout; // expireAt();
			if (exp <= t0) {

				// remove from queue
				if (!queue.is_periodic || !queue.is_circular) {
					// std::cerr << "erase timer...\n";
					pop_head(queue._timers);
				}

				t->timeout();
				t0 = exp;
				++count;

				if (queue.is_periodic) {

					if (queue.is_circular) {

						// t0 = ew::core::time::get_ticks();
						t->_timeout = t0 + t->frequency();
						// std::cerr << "recompute this timer->timeout to " << t->_timeout << "\n";
						if (!_sleep)
							_sleep = t->_timeout - t0;
						else
							_sleep = ew::maths::min<u32>(_sleep, t->_timeout - t0);

						++queue._last;
						if (queue._last == queue._timers.end()) {
							queue._last = queue._timers.begin();
						}

					} else {
						addTimer(t);
					}
				}

			} else {

				t1 = ew::core::time::get_ticks();
				if (exp >= t1) {
					if (!_sleep)
						_sleep =  exp - t1;
					else
						_sleep = ew::maths::min<u32>(_sleep, exp - t1);

				} else {
					_sleep = 0;
				}

				break;
			}
		}

		// std::cerr << __FUNCTION__ << " t1 - t0 = " << (t1 - t0) << "\n";
		// std::cerr << __FUNCTION__ << " END\n";
		return count;
	}
};


TimerHandler::TimerHandler()
	:
	d(new Private)
{

}

TimerHandler::~TimerHandler()
{
	delete d;
}


template < >
bool TimerHandler::Private::refresh_iterator(std::vector<Timer *> & container, std::vector<Timer *>::iterator & it, int pos)
{
	it =  container.begin() + pos;
	return true;
}

template < >
void TimerHandler::Private::pop_head(std::vector<Timer *> & vec)
{

}

template < >
void TimerHandler::Private::pop_head(std::list<Timer *> & lst)
{
	lst.pop_front();
}

/*
  Have arrays of

  timeout <= 1 ms
  timeout <= 10 ms
  timeout <= 100 ms
  timeout <= 1000 ms
  timeout <= 10000  ms

  + for each array an index for the last processed items

  insert t in queue based on repeat flag an freq

  each repeated timer is inserted in an array once
  for timers <= 1 ms
  just walk through the array an call each Timer::timeout()
*/

template <class Container>
bool TimerHandler::Private::addTimer(Queue<Container> & queue, Timer * t)
{
	u32 t0 = ew::core::time::get_ticks();
	t->_timeout = t0 + t->frequency();

	Timer * tn;

	if (queue.is_periodic && queue.is_circular) {

		if (queue._timers.size() == 0)
			queue._last = queue._timers.begin();
		int pos = std::distance(queue._timers.begin(), queue._last);
		queue._timers.push_back(t);
		refresh_iterator(queue._timers, queue._last, pos);

	} else if (queue._timers.size()) {

		// insert sorted
		// lock list
		{
			// TODO: save last pushed_back item
			typename Container::iterator it_last = queue._timers.end();
			--it_last;

			tn = *it_last;
			u32 last_timeout = tn->_timeout;

			// std::cerr << "tn->_timeout >= last_timeout   ?\n";
			// std::cerr << tn->_timeout << " >= " << last_timeout << "?\n";

			if (tn->_timeout >= last_timeout) {
				queue._timers.push_back(t);
			} else {

				typename  Container::iterator it =  queue._timers.begin();
				typename  Container::iterator it_end = queue._timers.end();
				while (it != it_end) {
					tn = *it;
					if (tn->_timeout >  t->_timeout) {
						// insert before this one
						break;
					}
					++it;
				}
				queue._timers.insert(it, t);
			}

			queue._last = queue._timers.begin();
		}
		// unlock list

	} else {
		queue._timers.push_back(t);
		queue._last = queue._timers.begin();
	}

	assert(queue._timers.size());

	typename Container::iterator it = queue._timers.begin();
	tn = *it;
	u32 exp = tn->expireAt();
	t0 = ew::core::time::get_ticks(); // sync

	if (t0 <= exp) {
		_sleep = exp - t0;
	} else {
		_sleep = 0;
	}

	// std::cerr << __FUNCTION__ << " AFTER _sleep = " << _sleep << "\n";
	return true;
}

bool TimerHandler::Private::addTimer(Timer * t)
{
	if (t->isPeriodic()) {
		if (t->frequency() <= 1) {
			addTimer(periodict_timers_1ms, t);
		} else if (t->frequency() <= 10) {
			addTimer(periodict_timers_10ms, t);
		}
#if 0
		else if (t->frequency() <= 100) {
			addTimer(periodict_timers_100ms, t);
		}
#else
		addTimer(periodict_timers_10ms, t);
#endif


	} else {
		addTimer(simple_timers, t);
	}

	return true;
}

bool TimerHandler::addTimer(Timer * t)
{
	return d->addTimer(t);
}

bool TimerHandler::removeTimer(Timer * t)
{
	return false;
}

// TODO: rename in process queue(is periodic)
u32 TimerHandler::wait()
{
	if (d->_sleep >= 1) {
		// std::cerr << "-------------------------;\n";
		// std::cerr << __FUNCTION__ << " will sleep " << _sleep << " millseconds\n";
		d->timeout_cond->timed_wait(d->_sleep);
	}

	// t0
	u32 count = 0;
	count += d->processQueue(d->periodict_timers_1ms);
	count += d->processQueue(d->periodict_timers_10ms);
	count += d->processQueue(d->simple_timers);
	return count;
}

} // ! namespace time

} // ! namespace test
