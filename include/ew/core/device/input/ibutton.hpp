#pragma once

#include <ew/core/types/types.hpp>

namespace ew
{
namespace core
{
namespace devices
{
namespace input
{

using namespace ew::core::types;

class EW_CORE_EXPORT button   /* : public ew::core::object ? */
{
public:
	button()
		:
		_state(RELEASED),
		_last_time_pressed(0),
		_last_time_released(0) { }

	virtual ~button() { }

	enum state { RELEASED, PRESSED };

	enum state getState()
	{
		return _state;
	}

	u32 getLastTimePressed()
	{
		return _last_time_pressed;
	}

	u32 getLastTimeReleased()
	{
		return _last_time_released;
	}

private:
	button::state _state;
	/* class Time button ??? */
	u32 _last_time_pressed;
	u32 _last_time_released;
};

}
}
}
}
