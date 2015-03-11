#pragma once

// Ew
#include <ew/ew_config.hpp>
#include <ew/core/object/irunable_object.hpp>
#include <ew/core/types/types.hpp>

namespace ew
{
namespace core
{
namespace threading
{

using namespace ew::core::types;

// sub system init
EW_CORE_THREAD_EXPORT bool init();
EW_CORE_THREAD_EXPORT bool quit();

// interface
class EW_CORE_THREAD_EXPORT thread
{
public:
	/* typedef */
	typedef void * (*func_t)(void *);
	typedef void * arg_t;

public:
	thread(func_t threadfunc, arg_t threadarg, const char * name);
	thread(ew::core::objects::runable_object * irobj, const char * name);
	~thread();

public:
	// iobject
	const char * get_class_name() const; // TODO: remove get_
	const char * get_name() const;

	/* setup : remove/move this elsewhere ? */
	func_t get_function();
	void set_function(func_t);

	arg_t get_argument();
	void set_argument(arg_t);

	/* mgmt */
	bool start();
	bool terminate();
	void join();

	bool is_running() const;

	/* todo: add an enum for the different priority (portable ?) */
	bool set_priority(s32 priority);

	EW_CORE_THREAD_EXPORT  friend void thread_exit(s32 status);

	// disable copy
private:
	thread(const thread &);
	thread & operator=(const thread &);

	// this allow allocation from an other dll on windows
	//   public:
	//     void * operator new ( size_t sz );
	//     void   operator delete ( void * ptr );
private:
	class private_data;
	class private_data * const d;
};

EW_CORE_THREAD_EXPORT void thread_exit(s32 status);
EW_CORE_THREAD_EXPORT bool thread_yield();
EW_CORE_THREAD_EXPORT thread * self();

}
}
}
