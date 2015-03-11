#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>

#include <ew/core/Exception.hpp>


namespace ew
{
namespace core
{
namespace objects
{

namespace stream
{

using namespace ew::core::types; // for (s)size_t, {u,s}{8,16,32,64}

typedef size_t open_mode;
namespace mode
{
static const size_t read      = (1 << 0);
static const size_t write     = (1 << 1);
static const size_t read_only  = read;
static const size_t write_only = write;
static const size_t read_write = read | write;
static const size_t append    = (1 << 3);
static const size_t execute   = (1 << 4);
static const size_t create    = (1 << 5);   /* ???? */
static const size_t listener  = (1 << 6);   /* ???? */
static const size_t invalid_mode = (size_t) - 1;
}

typedef size_t share_mode;
namespace sharing
{
static const size_t invisible = (0 << 0);
static const size_t no_share  = (0 << 7);   /* ???? */
static const size_t shared    = (1 << 7);   /* ???? */
}


// move to PositionableObject
enum location {
	beginning,
	current,
	end
};

enum direction {
	forward,
	backward,
	automatic_direction
};

} // ! namespace STREAM


// TODO:: remove the I in class name
class EW_CORE_OBJECT_EXPORT stream_object
{
public:
	virtual ~stream_object()
	{

	}

	const char * class_name() const
	{
		return "ew::core::objects::stream_object";
	};

	virtual bool open(stream::open_mode mode) = 0;
	virtual bool reopen(stream::open_mode mode) = 0;
	virtual stream::open_mode get_open_mode() = 0;

	virtual bool close() = 0;

	virtual bool is_opened() = 0;
	virtual bool is_closed() = 0;

	virtual bool set_position(u64 to, stream::location from, stream::direction direction) = 0;

	virtual u64 get_position(void) = 0;

public:

	class exceptions
	{
	public:
		class invalid_stream_direction;
		class position_overflow_exception;
		class position_underflow_exception;
	};
};


class EW_CORE_EXCEPTION_EXPORT stream_object::exceptions::invalid_stream_direction : public ew::core::exception
{
public:
	invalid_stream_direction() : exception("Invalid Stream Direction") { }
};

class EW_CORE_EXCEPTION_EXPORT stream_object::exceptions::position_underflow_exception : public ew::core::exception
{
public:
	position_underflow_exception() : exception("Position Underflow Exception") { }
};

class EW_CORE_EXCEPTION_EXPORT stream_object::exceptions::position_overflow_exception : public ew::core::exception
{
public:
	position_overflow_exception() : exception("Position Overflow Exception") { }
};


}
}
}
