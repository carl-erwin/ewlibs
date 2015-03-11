#pragma once

#include <ew/core/types/types.hpp>

#include <ew/core/device/input/iinput_device_with_buttons.hpp>
#include <ew/core/device/input/iinput_device_with_axis.hpp>
#include <ew/core/device/input/iinput_device_with_hats.hpp>

namespace ew
{
namespace user
{
namespace devices
{

using namespace ew::core::types;

class EW_USER_EXPORT joystick
	:
	public ew::core::devices::input::input_device_with_buttons,
	public ew::core::devices::input::input_device_with_axis,
	public ew::core::devices::input::input_device_with_hats
{
public:
	joystick(u32 index);
	virtual ~joystick();
};

}
}
}
