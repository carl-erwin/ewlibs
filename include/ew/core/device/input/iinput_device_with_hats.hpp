#pragma once

#include <ew/core/device/input/iinput_device.hpp>

namespace ew
{
namespace core
{
namespace devices
{
namespace input
{

class EW_CORE_DEVICE_EXPORT input_device_with_hats : virtual public input_device
{
public:
	input_device_with_hats(u32 index);
	virtual ~input_device_with_hats();

	virtual void getNumberOfHats(u32 & nrHats) = 0;
	virtual u32 getNumberOfHats(void) = 0;

};

}
}
}
}
