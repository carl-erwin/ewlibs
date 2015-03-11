#include <assert.h>

// lang c++ include
// #include <new>
#include <exception>

// system include

//#ifdef __FreeBSD__
// #include <unistd.h>
//#endif

#include <cstdio>

#include <pthread.h>
#include <signal.h>
#include <errno.h>

// Ew includes
#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

#include <ew/core/exception/exception.hpp>

#include <ew/core/threading/thread.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/mutex_locker.hpp>
#include <ew/core/threading/condition_variable.hpp>

#include <ew/utils/utils.hpp>

#include "../syscall/syscall.hpp"

namespace ew
{
namespace core
{
namespace threading
{

using namespace ew::core::syscall::unix_system;

// --------------------------------------------------------------------------------------
static TLS_DECL thread * mySelf = (thread *)1;
// --------------------------------------------------------------------------------------

static bool _threadWasInit = false;

bool init(void)
{
	_threadWasInit = true;
	return true;
}

bool quit(void)
{
	_threadWasInit = false;
	return true;
}

class thread::private_data : public mutex
{
	friend void thread_exit(s32);
public:

	private_data()
		:
		_threadFunc(0),
		_threadArg(0),
		_state(STOPPED),
		_name(0)
	{

	}

	~private_data()
	{
		delete [] _name;
	}

public:
	/* posix struct */
	pthread_t           threadID;

	/* param */
	func_t   _threadFunc;
	arg_t    _threadArg;

	/* status */
	enum state {
		STOPPED = 0,
		STARTING,
		RUNNING,
		QUITTING,
		TERMINATED,
	} _state;

	char     *    _name;
	//
	static void threadFunctionWrapper(thread * thread);
	static void * threadRunableObjectWrapper(thread::arg_t threadArg);
};


// --------------------------------------------------------------------------------------
thread * self()
{
	return mySelf;
}

// --------------------------------------------------------------------------------------

void thread_exit(s32 status)
{
	thread * selfTh = self();
	if (selfTh != (thread *) 1) {
		// do cleanup here, update state
		// selfTh->d->lock();
		selfTh->d->_state = thread::private_data::TERMINATED;

		// selfTh->d->unlock();
		sys_pthread_exit(reinterpret_cast<void *>(status));
	} else {
		// We are in main thread
		// self()->set_name("main thread");
	}
}

// --------------------------------------------------------------------------------------

bool thread_yield(void)
{
	int ret = sys_sched_yield();
	return (ret == 0) ? true : false;
}

// --------------------------------------------------------------------------------------

/* static */
void thread::private_data:: threadFunctionWrapper(thread * thread)
{
	// set TLS for self()
	mySelf = thread;

	assert(mySelf->d != 0);

	// set cancel state/type here
	// pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
	// if (thread->isAttached())
	// pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );

	mySelf->d->_state = RUNNING;
	mySelf->d->_threadFunc(mySelf->d->_threadArg);

	// ??
	mySelf->d->_state = QUITTING;

	thread_exit(0);
}

void * thread::private_data:: threadRunableObjectWrapper(thread::arg_t threadArg)
{
	ew::core::objects::runable_object * irobj;

	irobj = static_cast<ew::core::objects::runable_object *>(threadArg);
	irobj->run();
	return NULL;
}

// --------------------------------------------------------------------------------------
// ctor(s)
// --------------------------------------------------------------------------------------

thread::thread(thread::func_t threadFunc, thread::arg_t threadArg , const char * name)
	:
	d(::new private_data())
{
	assert(d != 0);
	d->_threadFunc = threadFunc;
	d->_threadArg = threadArg;
	d->_name = ew::utils::c_string_dup(name);
}

thread::thread(ew::core::objects::runable_object * irobj, const char * name)
	:
	d(::new private_data())
{
	assert(d != 0);
	d->_threadFunc =  thread::private_data::threadRunableObjectWrapper;
	d->_threadArg = irobj;
	d->_name = ew::utils::c_string_dup(name);
}


// --------------------------------------------------------------------------------------

thread::~thread()
{
	if (this->terminate() == false) {
		// throw
	}
	::delete d;
}

// --------------------------------------------------------------------------------------

const char * thread::get_class_name() const
{
	return "ew::core::threading::thread";
}

const char * thread::get_name() const
{
	/* TODO: must have a fake thread instance for the main thread
	   with its status etc set properly
	*/
	if (this == (thread *)1)
		return "main";
	return d->_name;
}

// --------------------------------------------------------------------------------------

void thread::set_function(thread::func_t threadFunc)
{
	mutex_locker locker(d);
	d->_threadFunc = threadFunc;
}

// --------------------------------------------------------------------------------------

thread::func_t thread::get_function(void)
{
	mutex_locker locker(d);
	return d->_threadFunc;
}

// --------------------------------------------------------------------------------------

void thread::set_argument(thread::arg_t arg)
{
	mutex_locker locker(d);
	d->_threadArg = arg;
}

// --------------------------------------------------------------------------------------

thread::arg_t thread::get_argument(void)
{
	mutex_locker locker(d);
	return d->_threadArg;
}

// --------------------------------------------------------------------------------------

bool thread::start()
{
	mutex_locker locker(d);

	if (d->_state == thread::private_data::RUNNING)     /* must throw ? */
		return false;

	d->_state = thread::private_data::STARTING;

	//   pthread_attr_t threadAttr;
	//   if ( pthread_attr_init( &( threadAttr ) ) != 0 )
	//   {
	//     perror( "ew::core::threading::thread::start()" );
	//     return false;
	//   }
	int ret = sys_pthread_create(
			  (pthread_t *) &d->threadID,
			  (pthread_attr_t *) 0 /* & threadAttr */,
			  (func_t) private_data::threadFunctionWrapper,
			  (arg_t)this);

	if (ret != 0) {   /* must throw : create error */
		perror("ew::core::threading:: thread::start() :: pthread_create:: ko");
		return false;
	}

	return true;
}

// --------------------------------------------------------------------------------------

bool thread::is_running() const
{
	mutex_locker locker(d);
	return (d->_state == thread::private_data::RUNNING);
}

// --------------------------------------------------------------------------------------

bool thread::terminate()
{
	mutex_locker locker(d);

	if (d->_state != thread::private_data::RUNNING)
		return false;

	if (sys_pthread_cancel((d->threadID)) != (int) 0) {
		perror("ew::core::thread:: thread::terminate() :: pthread_cancel");
		return false;
	}
	d->_state = thread::private_data::TERMINATED;

	return true;
}

// --------------------------------------------------------------------------------------

/*
 *
 * return bool ?
 *
 * todo : check the return status of pthread_join
 */

void thread::join()
{
	mutex_locker locker(d);

#ifndef __FreeBSD__
	while (d->_state == thread::private_data::STARTING)
		::usleep(1);
#endif

	// what if stopped ?
	switch (d->_state) {

	case thread::private_data::RUNNING: {
		// TODO: store thread_return val in d->_retval
		void * thread_return;
		int ret = sys_pthread_join((d->threadID), (void **)&thread_return);
		if (ret == 0) {
			d->_state = thread::private_data::TERMINATED;
		}
		// else default error : throw
		break;
	}

	case thread::private_data::STARTING:
	case thread::private_data::STOPPED:
	case thread::private_data::QUITTING:
	case thread::private_data::TERMINATED:
		break;
	}
}

bool thread::set_priority(s32 priority)
{
	struct sched_param param;

	param.sched_priority = priority;

	/* only supported policy, others will result in ENOTSUP */
	int policy = SCHED_OTHER;
	policy  = SCHED_FIFO;

	int ret = sys_pthread_setschedparam(d->threadID, policy, &param);
	if (ret == 0) {
		return true;
	}

	return false;
}


}
}
}
