#pragma once

#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>


namespace eedit
{

namespace core
{

/* FIXME:
 * implement like a ring buffer with a gap
 * do the slots clearing when we push ...
 * this will avoid locks in get(...)
 */
template <class T>
class event_queue
{
public:

	size_t size() const
	{
		return m_queue.size();
	}

	inline bool push(T msg)
	{
		std::lock_guard<std::mutex> lock(m_queue_mtx);
		m_queue.push(msg);
		m_queue_cond_var.notify_one();
		return true;
	}

	inline size_t wait(size_t wait_time)
	{
		if (m_queue.size())
			return m_queue.size();

		std::unique_lock<std::mutex> lk(m_queue_mtx);
		auto now = std::chrono::system_clock::now();
		m_queue_cond_var.wait_until(lk, now + std::chrono::milliseconds(wait_time));
		return m_queue.size();
	}

	inline bool get(T & msg)
	{
		// FIXME: race condition here if multiple get
		if (m_queue.empty()) {
			return false;
		}

		std::lock_guard<std::mutex> lock(m_queue_mtx);
		msg = m_queue.front();
		m_queue.pop();

		return true;
	}

private:
	std::mutex              m_queue_mtx;
	std::queue<T>           m_queue;
	std::condition_variable m_queue_cond_var;
};

////////////////////////////////////////////////////////////////////////////////


}

}
