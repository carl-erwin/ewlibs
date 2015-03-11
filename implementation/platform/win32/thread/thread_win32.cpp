// -*- mode: c++; tab-width:2 -*-
// ex: ts=2

// system include(s)
#include <iostream>
#include <windows.h>

// ew include(s)
#include <ew/ew_config.hpp>
#include <ew/core/thread/thread.hpp>


// forward declaration, constants definitions
namespace ew
{
namespace Core
{
namespace THREAD
{

static void  runWrapper(thread * vthread);


bool init()
{
	return true;
}
bool quit()
{
	return true;
}
// --------------------------------------------------------------------------------
__declspec(thread) thread * mySelf = (thread *)1;

class thread::private_data
{
public:
	private_data() :
		_threadHandle(0),
		_threadId(0),
		_threadFunc(0),
		_threadArg(0),
		_initialised_ok(true),
		_started_ok(false)
	{
	}

	inline bool lock()
	{
		return true;
	}
	inline bool unlock()
	{
		return true;
	}

	/* win32 types */
	HANDLE _threadHandle;
	DWORD _threadId;

	/* */
	THREAD::func_t _threadFunc;
	THREAD::Arg_t _threadArg;

	/* status */
	bool _started_ok;
	bool _initialised_ok;

public:

	static void  threadFunctionWrapper(thread * vthread);
	static void  runWrapper(thread * vthread);
};

// --------------------------------------------------------------------------------
thread * self()
{
	return mySelf;
}

// --------------------------------------------------------------------------------

thread::thread(THREAD::func_t threadFunc, THREAD::Arg_t threadArg)
	: d(new private_data())
{
	d->_threadFunc = threadFunc;
	if (d->_threadFunc == 0)
		d->_threadFunc = reinterpret_cast<THREAD::func_t>(thread::private_data::runWrapper);

	d->_threadArg = threadArg;
	if (d->_threadArg == 0)
		d->_threadArg = this;
}

// --------------------------------------------------------------------------------

thread::~thread()
{
	if (data == 0)
		return ;

	this->terminate();

	delete d;
}

// --------------------------------------------------------------------------------
const char * thread::class_name() const
{
	// we must compute a name based on tid
	return "ew::core::THREAD::thread";
}

const char * thread::getName() const
{
	// we must compute a name based on tid
	return "unnamed thread";
}

void   thread_exit(s32 val)
{
	::Exitthread((DWORD)val);
}

// --------------------------------------------------------------------------------

void  thread::private_data::runWrapper(thread * vthread)
{
	if (vthread)
		vthread->run();
}

void  thread::private_data::threadFunctionWrapper(thread * vthread)
{
	// TLS for self()
	mySelf = vthread;

	// fprintf(stderr, "static void   threadWrapper(thread * vthread)\n");

	// set cancel state/type here

	// fprintf(stderr, "static void   threadWrapper(thread * vthread) : set isStarted\n");
	// deadlock vthread->d->lock();
	vthread->d->_started_ok = true;
	// deadlock vthread->d->unlock();

	if ((THREAD::Arg_t)vthread->d->_threadArg != (THREAD::Arg_t)vthread)
		vthread->d->_threadFunc(vthread->d->_threadArg);
	else
		vthread->d->_threadFunc(0);   // ?????

	// fprintf(stderr ,"static void   threadWrapper(thread * vthread) : func finished\n");
	// vthread->d->lock();
	// fprintf(stderr ,"static void   threadWrapper(thread * vthread) : clear isStarted\n");
	// vthread->d->unlock();
	// fprintf(stderr ,"static void   threadWrapper(thread * vthread) : THREAD EXIT\n");
	// vthread->d->_started_ok = false;
	// vthread->d->_has_exited = true;
	thread_exit(0);
}

// --------------------------------------------------------------------------------

void  thread::set_function(THREAD::func_t threadFunc)
{
	d->_threadFunc = threadFunc;
}

// --------------------------------------------------------------------------------


THREAD::func_t  thread::getFunc(void)
{
	return d->_threadFunc;
}

// --------------------------------------------------------------------------------


void    thread::set_argument(THREAD::Arg_t arg)
{
	d->_threadArg = arg;
}

// --------------------------------------------------------------------------------

THREAD::Arg_t   thread::getArg(void)
{
	return d->_threadArg;
}

// --------------------------------------------------------------------------------

void   thread::run(void)
{
	for (;;) {
		std::cerr << "void  thread::run()" << "\n";
	}
}

// --------------------------------------------------------------------------------

bool   thread::start()
{

	d->lock();

	if (d->_initialised_ok == false) {
		// throw invalid thread
		d->unlock();
		return false;
	}

	if (d->_started_ok == true) {
		// exception double execution ??
		d->unlock();
		return false;
	}

	THREAD::func_t wrapper = (THREAD::func_t)thread::private_data::runWrapper;
	if (d->_threadFunc != (THREAD::func_t)thread::private_data::runWrapper)
		wrapper = (THREAD::func_t)thread::private_data::threadFunctionWrapper;

	THREAD::Arg_t wrapper_arg = (THREAD::Arg_t)this;

	d->_threadHandle =
		Createthread(
			LPSECURITY_ATTRIBUTES(NULL), /* the returned handle cannot be inherited (see msdn) */
			SIZE_T(0),   /* stack size : use system default */
			LPTHREAD_START_ROUTINE(wrapper),  /* */
			LPVOID(wrapper_arg), /* */
			DWORD(0), /* the thread runs immediately after creation. */
			LPDWORD(&d->_threadId) /* */
		);

	if (d->_threadHandle == 0) {
		std::cerr << "error :: d->_threadHandle == 0" << std::endl;
		// no ressources
		// DWORD GetLastError(void);
		return false;
	}

	d->_started_ok = true;

	return true;
}

// --------------------------------------------------------------------------------

bool  thread::isRunning()
{
	return d->_started_ok;
}

// --------------------------------------------------------------------------------

bool  thread::terminate()
{
	if (data == 0)
		return false;

	if (d->_started_ok == false)
		return false;

	if (Terminatethread(d->_threadHandle, FALSE) == FALSE)
		return false;

	d->_started_ok = false;

	return true;
}

// --------------------------------------------------------------------------------

void  thread::join()
{
	if (d->_started_ok == false)
		return ;

	u32 _status;

	WaitForSingleObject(d->_threadHandle, INFINITE);
	GetExitCodethread(d->_threadHandle, LPDWORD(&_status));
	CloseHandle(d->_threadHandle);
}

// --------------------------------------------------------------------------------

}
}
}
