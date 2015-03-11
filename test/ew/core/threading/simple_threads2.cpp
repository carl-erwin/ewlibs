#include <iostream>
#include <ew/Core.hpp>


// -------------------------------

using namespace ew::core;
using namespace ew::core::time;
using namespace ew::core::threading;

namespace ew
{

namespace test
{

void threadEcho(int count)
{
	std::cout << "threadEcho :: " << count << "\n";

	ew::core::threading::thread_exit((s32) 0);
}


class my_run_obj : public ew::core::objects::runable_object   // public ew::core::THREAD::thread
{
public:
	my_run_obj()
	{
		/* : thread::thread() */
	}

	void * operator new(size_t sz)
	{
		std::cerr << " my_thread_function * operator new () " << sz << "\n";
		return (void *) ::new my_run_obj();
	}

	void run()
	{
		static int _nrCalls = 0;
		std::cerr << "my_thread_function::run() > calls " << ++_nrCalls << std::endl;
	}

	const char * getName() const
	{
		return "my_thread_function";
	}

};

int main(int ac, char ** av)
{
	ew::core::time::init();

	ew::core::threading::thread * thread = 0;
	int count = 0;
	while (1) {
		my_run_obj * _mythread = new my_run_obj();
		thread = new ew::core::threading::thread((ew::core::objects::runable_object *)_mythread, (const char *)"");
		// thread = ew::core::threading::thread::allocate();
//    thread = new ew::core::threading:: thread();
		{
			thread->set_function((thread::func_t) threadEcho);
			thread->set_argument(reinterpret_cast<thread::arg_t>(count++));
			thread->start();
			thread->join();
		}
		// ew::core::threading::thread::release( thread );
		delete thread;
		// delete _mythread;
		std::cerr << "\n";
	}


	return (0);
}

} // ! namespace test


} // ! namespace ew

int main(int ac, char ** av)
{
	return ew::test::main(ac, av);
}
