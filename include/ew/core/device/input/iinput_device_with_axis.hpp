#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/device/input/iinput_device.hpp>

namespace ew
{
namespace core
{
namespace devices
{
namespace input
{

using namespace ew::core::types;

class EW_CORE_DEVICE_EXPORT input_device_with_axis : virtual public input_device
{
public:
	//  input_device_with_axis(u32 index);
	//virtual ~input_device_with_axis();

	virtual void getNumberOfAxis(u32 & nrAxis) = 0;
	virtual u32 getNumberOfAxis(void) = 0;
	virtual bool getAxisValue(u32 axis, s32 & value) = 0;

	//   virtual bool enableAxisRepetition(u32 & axis) = 0; // ?
};

}
}
}
}
