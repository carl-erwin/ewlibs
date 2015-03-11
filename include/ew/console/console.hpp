#pragma once

#include <ew/core/types/types.hpp>
#include <ew/core/threading/mutex.hpp>

namespace ew
{
namespace console
{

using namespace ew::core::types;

enum type {
	STDIN   = 0,
	STDOUT  = 1,
	STDERR  = 2,
	FILE    = 3,
	DEBUG   = 4
};

// console integer display mode
namespace  integer
{

enum  mode {
	dec,
	hex,
	oct,
	bin
};
}

/* */
enum level {
	CONSOLE_LEVEL0,
	CONSOLE_LEVEL1,
	CONSOLE_LEVEL2,
	CONSOLE_LEVEL3,
	CONSOLE_LEVEL4,
	CONSOLE_LEVEL5,
	CONSOLE_LEVEL6,
	CONSOLE_LEVEL7,
};

/*
  TODO: hide mutex in priv data ?
*/
class EW_CONSOLE_EXPORT console : public ew::core::threading::mutex
{
public:
	explicit console(ew::console::type type);
	//    explicit console( ew::FILESYSTEM::File & file );
	virtual ~console();

	void enable();
	void disable();
	bool is_enabled();
	void toggle();

	bool set_max_level(const ew::console::level lvl = CONSOLE_LEVEL0);

	console & operator<< (const ew::console::level run_lvl);

	console & operator<< (const ew::console::integer::mode new_mode);

	console & operator<< (const char c);
	console & operator<< (const char * s);
	console & operator<< (const void * addr);

	console & operator<< (const s8 v);
	console & operator<< (const u8 v);
	console & operator<< (const s16 v);
	console & operator<< (const u16 v);
	console & operator<< (const s32 v);
	console & operator<< (const u32 v);
	console & operator<< (const s64 v);
	console & operator<< (const u64 v);

	console & operator<< (const f32 v);
	console & operator<< (const double v);

	template <typename T>  bool write(const T val)
	{
		this->operator << (val);
		return true;
	}

	bool write(const char * s, u32 size);

private:
	class private_data;
	class private_data * const d;
};

extern EW_CONSOLE_EXPORT console cerr;
extern EW_CONSOLE_EXPORT console cout;
extern EW_CONSOLE_EXPORT console cin;
extern EW_CONSOLE_EXPORT console dbg;
}
}
