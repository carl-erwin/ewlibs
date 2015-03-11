#include <cstdlib>

#include <list>

#include <ew/core/threading/mutex_locker.hpp>
#include <ew/core/threading/thread.hpp>
#include <ew/core/threading/condition_variable.hpp>
#include <ew/Console.hpp>
#include <ew/core/Time.hpp>
#include <ew/Maths.hpp>
#include <ew/System.hpp>
// -------------------------------

using namespace ew::core;
using namespace ew::core::threading;
using namespace ew::core::time;
using namespace ew::core::types;
using namespace ew::maths;
using namespace ew::console;


namespace test
{

namespace simple_thread
{


mutex list_mtx;
std::list<thread *> threadList;
typedef std::list<thread *>::iterator threadListIt;

mutex mtx;
u32 nrthreadRunning = 0;
u32 nrFinished = 0;
u32 nrCreatedthread = 10;

mutex start_destroyer_mtx;
condition_variable start_destroyer_cond(&start_destroyer_mtx);

void threadEchoTick(u32 nr)
{
	{
		mutex_locker lock(list_mtx);
		threadList.push_back(threading::self());
	}


	for (u32 i = 1 ; i <= 5; ++i) {
		{
			mutex_locker lock(cerr);
			cerr << "thread(" << integer::dec << nr <<  "), THREAD::self() = " << integer::hex << (size_t)threading::self() << ", loop = " << integer::dec << i << "\n";
		}

		ew::core::time::sleep(1000);
	}

	{
		mutex_locker lock(mtx);
		--nrthreadRunning;
		++nrFinished;
	}

	mutex_locker lock(cerr);
	cerr << "thread(" << integer::dec << nr << "), THREAD::self() = " << integer::hex << (size_t)threading::self() << ", EXITING \n";
	thread_exit(0);
}

void threadDestroyerFunc()
{
	cerr << "threadDestroyerFunc :: thread destroyer STARTING\n";
	{
		mutex_locker lock(&start_destroyer_mtx);
		start_destroyer_cond.wait();
	}

	cerr << "thread destroyer BEGIN LOOP\n";

	while (1) {
		list_mtx.lock();
		if (threadList.size() == 0) {
			cerr << "---- thread destroyer NO MORE THREADS ---\n";
			list_mtx.unlock();
			break ;
		}
		list_mtx.unlock();

		list_mtx.lock();
		for (threadListIt it = threadList.begin(); threadList.size() && it != threadList.end();) {
			thread * th = (*it);
			if (th == 0) {
				cerr << "th == 0 !!!!!!\n";
				continue;
			}

			th->join();
			delete th;
			threadList.erase(it++);
		}
		list_mtx.unlock();
	}

	cerr << "wait and destroy thread\n";
	cerr << "nr TH running  = " << nrthreadRunning << "\n";
	cerr << "nr TH finished = " << nrFinished << "\n";

	cerr << "thread destroyer EXITING\n";
}


#define MAX_THREADS 1000000
int test1(int ac, char * av[])
{
	nrCreatedthread = 10;
	if (ac >= 2)
		nrCreatedthread = in_range<u32>(1, ::atoi(av[1]), MAX_THREADS);

	ew::core::threading::init();

	cerr << "main : threading::self() = " << (size_t)threading::self() << "\n";

	cerr << "ac = " << ac << "\n";
	cerr << "nrCreatedthread = " << nrCreatedthread << "\n";

	ew::core::time::init();

	// init vars
	nrthreadRunning = 0;
	nrFinished = 0;

	threading::thread * threadDestroyer = new thread((thread::func_t) threadDestroyerFunc , (thread::arg_t)0, "threaddestroyer");
	if (threadDestroyer->start() == false) {
		cerr << "could not start thread destroyer\n";
		ew::system::exit(1);
	}
	cerr << "thread destroyer started\n";

	for (u32 i = 0; i < nrCreatedthread; ++i) {
		cerr << "try to create thread " << i << "\n";
		threading::thread * echo_thread = new thread((thread::func_t) threadEchoTick , reinterpret_cast<thread::arg_t>(i), "threadEchoTick");
		if (echo_thread->start() == false) {
			cerr << "could not start thread " << i << "\n";
			nrCreatedthread = i;
			delete echo_thread;
			break;
		}

		nrthreadRunning++;

		cerr << "create thread " << i << " ok\n";
	}

	cerr << " start_destroyer_cond.signal() ...\n";
	start_destroyer_cond.signal();

	cerr << "try to join destroyer ...\n";
	threadDestroyer->join();
	cerr << " threadDestroyer->join() :: done" << "\n";
	delete threadDestroyer;

	cerr << "test : done..." << "\n";

	ew::core::time::quit();

	return (0);
}


class my_obj : public ew::core::objects::runable_object
{
public:
	virtual void run()
	{
		std::cerr << "my_obj::run() !!!\n";
	}
};

int test2(int ac, char * av[])
{

	ew::core::threading::init();

	my_obj * obj = new my_obj;

	thread * th = new ew::core::threading::thread(obj, "my_obj");

	th->start();
	th->join();

	delete obj;

	delete th;

	return 0;
}


int main(int ac, char ** av)
{
	test1(ac, av);
	test2(ac, av);
	return 0;
}

}

}

int main(int ac, char ** av)
{
	return test::simple_thread::main(ac, av);
}
