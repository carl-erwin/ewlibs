#pragma once


// Ew
#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

#include <ew/core/threading/mutex.hpp>

namespace ew
{
namespace core
{
namespace threading
{

class mutex;

class EW_CORE_THREAD_EXPORT condition_variable
{
public:
	condition_variable(mutex * mtx);
	virtual ~condition_variable();

	bool set_mutex(mutex * mtx);
	mutex * get_mutex() const;

	bool wait(); // infinite
	bool timed_wait(size_t nr_milliseconds);
	bool timed_wait_micro(size_t nr_microseconds); // remove this ???
	bool timed_wait_nano(size_t nr_nanoseconds);   // remove this ???
	bool signal();    //
	bool broadcast(); //

private:
	class private_data;
	private_data * d;
};

}
}
}
