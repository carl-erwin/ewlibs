#include <ew/ew.hpp>

#include <ew/Ew_program.hpp>

#include <ew/core/thread/thread.hpp>

// -------------------------------
using namespace ew::core;
using namespace ew::core::types;
using namespace ew::core::time;
using namespace ew::core::THREAD;
using namespace ew::core::MUTEX;
// -------------------------------

void test(const char * msg)
{
	std::cerr << msg << "\n";

	thread_exit(0);
}


int main(int ac, char * av[])
{
	ew::core::time::init();
	ew::core::THREAD::init();

	for (u32 i = 0; i < 100; ++i) {
		thread * thread = new thread((func_t) test, (Arg_t) "Hello world !");
		thread->start();
		thread->join();
		delete thread;
	}
	return (0);
}

