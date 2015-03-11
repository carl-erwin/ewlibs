#pragma once

// ew
#include <ew/ew_config.hpp>


namespace ew
{
namespace core
{
namespace threading
{

class condition_variable;
class thread;

/** \classmutex
 *   \brief this is the basic locking struct
 */

class EW_CORE_MUTEX_EXPORT mutex
{
private:
	mutex(const mutex &);
	mutex & operator= (const mutex &);

public:
	enum type {
		normal_mutex,
		recursive_mutex,
		error_check,
	};

public:
	mutex(mutex::type t = normal_mutex);
	~mutex();

	mutex::type get_type();

	bool lock();
	bool trylock();
	bool unlock();

	thread * is_locked_by_thread();

private:
	class private_data;
	class private_data * const d;

	friend class ew::core::threading::condition_variable;
};

}
}
}
