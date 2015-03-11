#pragma once

#include <ew/ew_config.hpp>

#include <ew/core/device/input/ibutton.hpp>
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

class EW_CORE_DEVICE_EXPORT input_device_with_buttons : virtual public input_device
{
public:
	//  input_device_with_buttons(u32 index) {}

	//  virtual ~input_device_with_buttons() {}

	virtual void getNumberOfButtons(u32 & nrButtons) = 0;

	virtual u32 getNumberOfButtons(void) = 0;

	virtual bool isButtonPressed(u32 button) = 0;

	virtual bool isButtonRealesed(u32 button) = 0;

//     virtual bool isAnyButtonPressed( void ) = 0;
//     virtual bool isAnyButtonPressed( u32 & button ) = 0;
//     virtual bool getButtonState( u32 button, IButton::State & state ) = 0;
//     // Pressed / Released


//     virtual bool enableButtonRepetition(u32 & button) = 0;
//     in a derived class ?

};

}
}
}
}
