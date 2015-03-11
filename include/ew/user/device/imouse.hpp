#pragma once

#include <ew/core/device/input/iinput_device_with_axis.hpp>
#include <ew/core/device/input/iinput_device_with_buttons.hpp>

namespace ew
{
namespace user
{
namespace devices
{

using namespace ew::core::types;

class EW_USER_EXPORT mouse
	:
	public ew::core::devices::input::input_device_with_axis,
	public ew::core::devices::input::input_device_with_buttons
{
public:
	mouse(u32 index);
};

}
}
}
