#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ew/console/console.hpp>
#include <ew/core/exception/exception.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/mutex_locker.hpp>

#include <ew/core/program/environment.hpp>

// syscall
#include "../core/syscall/syscall.hpp"

#include <ew/maths/functions.hpp>

namespace ew
{
namespace console
{

TLS_DECL integer::mode integer_mode = integer::dec;

// globals
EW_CONSOLE_EXPORT  console cin(STDIN);
EW_CONSOLE_EXPORT  console cout(STDOUT);
EW_CONSOLE_EXPORT  console cerr(STDERR);
EW_CONSOLE_EXPORT  console dbg(DEBUG);

using namespace ew::core::syscall::unix_system;
using namespace ew::core::threading;

/* global console lock */
mutex      console_mutex;

// TODO: move to CONCURRENT
class  conditional_mutex_locker
{
public:
	conditional_mutex_locker(mutex * mutex, bool use_lock = false)
		: _mutex(mutex),
		  _use_lock(use_lock)
	{
		if (_use_lock)
			_mutex->lock();
	}

	~conditional_mutex_locker()
	{
		if (_use_lock)
			_mutex->unlock();
	}
private:
	mutex * _mutex;
	bool    _use_lock;
};

class console::private_data
{
public:
	private_data(ew::console::type type)
		:
		enable(true),
		fd(-1),
		_type(type),
		max_lvl(ew::console::CONSOLE_LEVEL0),
		run_lvl(ew::console::CONSOLE_LEVEL0)
	{

		/* debug console */
		if (type == DEBUG) {
			enable = false;
			run_lvl = CONSOLE_LEVEL4; // default debug level

			char * dbg_lvl = ew::core::program::getenv("EW_DEBUG_LEVEL");
			if (dbg_lvl) {
				int lvl =  ew::maths::in_range((int)CONSOLE_LEVEL0 , ::atoi(dbg_lvl), (int)CONSOLE_LEVEL7);
				cerr << "found DEBUG_LEVEL " << lvl << "\n";
				if ((int)lvl <= (int)run_lvl) {
					enable = true;
				}
			}
		}
	}

	bool enable;
	int  fd;
	ew::console::type  _type;
	ew::console::level max_lvl;
	ew::console::level run_lvl;

	ssize_t console_sys_write(int fd, const void * buf, size_t count)
	{
		if (!enable || max_lvl < run_lvl)
			return 0;

		// TODO: add mutex use flag
		conditional_mutex_locker con_lock(&console_mutex);
		return sys_write(fd, buf, count);
	}

	//
	template <typename T>
	void  print_bin(int fd, T val, u32 sz)
	{

		if (!enable || (max_lvl < run_lvl))
			return;

		char buffer[sizeof(T) * 8 + 1 + 1];
		char * p = &buffer[0];

		*p++ = 'b';
		for (int i = (sz * 8) - 1; i >= 0; i--) {
			*p++ = '0' + ((val >> i) & 1);
		}
		*p++ = 0;

		console_sys_write(fd, buffer, p - buffer);
	}

	void print_string(const char * str, u32 len)
	{

		if (!enable || (max_lvl < run_lvl))
			return;

		console_sys_write(fd, str, len);
	}

	void print_float(const float v)
	{

		if (!enable || (max_lvl < run_lvl))
			return;

		char buffer[ 31 + 1 ];
		int len = snprintf(buffer, sizeof(buffer) - 1, "%f", v);
		buffer[ len ] = 0;

		console_sys_write(fd, buffer, len);
	}

	void print_double(const double v)
	{

		if (!enable || (max_lvl < run_lvl))
			return;


		char buffer[ 31 + 1 ];
		int len = snprintf(buffer, sizeof(buffer) - 1, "%f", v);
		buffer[ len ] = 0;

		console_sys_write(fd, buffer, len);
	}


	void print_integer(s64 v, u32 sz)
	{

		if (!enable || (max_lvl < run_lvl))
			return;

		const char * fmt = "";
		switch (integer_mode) {

		case integer::dec:
#if __WORDSIZE == 64
			fmt = "%ld";
#else
			fmt = "%lld";
#endif
			break;
		case integer::hex:
			fmt = "0x%x";
			break;
		case integer::oct:
			fmt = "%o";
			break;
		case integer::bin:
			print_bin(fd, v, sz);
			return;
		}

		char buffer[ 31 + 1 ];
		int len = snprintf(buffer, sizeof(buffer) - 1, fmt, v);
		buffer[ len ] = 0;

		console_sys_write(fd, buffer, len);
		// catch
	}

	void print_integer(u64 v, u32 sz)
	{

		if (!enable || (max_lvl < run_lvl))
			return;

		const char * fmt = "";
		switch (integer_mode) {
		case integer::dec:
#if __WORDSIZE == 64
			fmt = "%lu";
#else
			fmt = "%llu";
#endif
			break;
		case integer::hex:
			fmt = "0x%x";
			break;
		case integer::oct:
			fmt = "%o";
			break;
		case integer::bin:
			print_bin(fd, v, sz);
			return;
		}

		char buffer[ 31 + 1 ];
		int len = snprintf(buffer, sizeof(buffer) - 1, fmt, v);
		buffer[ len ] = 0;

		console_sys_write(fd, buffer, len);
		// catch
	}

};

console::console(ew::console::type type)
	: d(::new private_data(type))
{
	switch (type) {
	case  STDIN:
		d->fd = 0;
		break;
	case  STDOUT:
		d->fd = 1;
		break;
	case  STDERR:
	case  DEBUG:
		d->fd = 2;
		break;

	default:
		throw ew::core::exceptions::invalid_parameter();
	}
}

console::~console()
{
	delete d;
}

void console::enable()
{
	d->enable = true;
}

void console::disable()
{
	d->enable = false;
}

bool console::is_enabled()
{
	return d->enable;
}

void console::toggle()
{
	if (is_enabled())
		disable();
	else
		enable();
}

bool console::set_max_level(const ew::console::level lvl)
{
	d->max_lvl = lvl;
	return true;
}

console & console::operator<< (const ew::console::level run_lvl)
{
	d->run_lvl = run_lvl;
	return *this;
}

console & console::operator<< (const integer::mode new_mode)
{
	integer_mode = new_mode;
	return *this;
}

console & console::operator<< (const char c)
{
	d->console_sys_write(d->fd, &c, 1);
	return *this;
}

console & console::operator<< (const char * s)
{
	if (s) {
		d->console_sys_write(d->fd, s, strlen(s));
	}
	return *this;
}

console & console::operator<< (const void * ptr)
{
	d->print_integer((u64)ptr, sizeof(ptr));
	return *this;
}


console & console::operator<< (const s8 v)
{
	d->print_integer((u64)v, sizeof(v));
	return *this;
}

console & console::operator<< (const u8 v)
{
	d->print_integer((u64)v, sizeof(v));
	return *this;
}

console & console::operator<< (const s16 v)
{
	d->print_integer((s64)v, sizeof(v));
	return *this;
}

console & console::operator<< (const u16 v)
{
	d->print_integer((u64)v, sizeof(v));
	return *this;
}

console & console::operator<< (const s32 v)
{
	d->print_integer((s64)v, sizeof(v));
	return *this;
}

console & console::operator<< (const u32 v)
{
	d->print_integer((u64)v, sizeof(v));
	return *this;
}

console & console::operator<< (const s64 v)
{
	d->print_integer(v, sizeof(v));
	return *this;
}

console & console::operator<< (const u64 v)
{
	d->print_integer(v, sizeof(v));
	return *this;
}

console & console::operator<< (const f32 v)
{
	d->print_float(v);
	return *this;
}

console & console::operator<< (const double v)
{
	d->print_double(v);
	return *this;
}

// ------------------------------------------------

bool console::write(const char * str, u32 len)
{
	d->print_string(str, len);
	return true;
}

}
}
