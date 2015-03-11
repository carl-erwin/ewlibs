#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

namespace ew
{
namespace core
{
namespace threading
{

using namespace ew::core::types;

class EW_CORE_THREAD_EXPORT semaphore
{
public:
	semaphore(size_t value);
	virtual ~semaphore();

	u32 value();
	bool wait();
	bool try_wait();
	bool timed_wait();
	bool post();

	bool release(); // remove this ??? if false do not delete

private:
	class private_data;
	class private_data * d;
};

// todo :
// semaphore(const char * name, u32 value); // kernel 2.6 + nptl named semaphore
// create namedsemaphore ?


}
}
}
