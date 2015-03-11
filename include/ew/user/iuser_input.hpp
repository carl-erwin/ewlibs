#pragma once

#include <ew/ew_config.hpp>

#include <ew/core/object/object.hpp>

#include <ew/core/device/input/iinput_device.hpp>

#include <ew/user/device/ikeyboard.hpp>
#include <ew/user/device/imouse.hpp>
#include <ew/user/device/ijoystick.hpp>


namespace ew
{
namespace user
{

// namespace INPUT ?? -> ew::User::INPUT:: IUserInputInterface
using namespace ew::core::types;
using namespace ew::core::devices::input;
using namespace ew::user::devices;


class EW_USER_EXPORT user_input_interface : public ew::core::object
{
public:
	user_input_interface();
	virtual ~user_input_interface();

	virtual u32 getNumberOfKeyboardDevices(void) = 0;
	virtual u32 getNumberOfMouseDevices(void) = 0;
	virtual u32 getNumberOfJoystickDevices(void) = 0;

	virtual keyboard * getKeyboardDevice(u32 index) = 0;
	virtual mouse * getMouseDevice(u32 index) = 0;
	virtual joystick * getJoystickDevice(u32 index) = 0;

	virtual void releaseKeyboardDevice(keyboard * ikeyboard) = 0;
	virtual void releaseMouseDevice(mouse * imouse) = 0;
	virtual void releaseJoystickDevice(joystick * ijoystick) = 0;
};

}
} // ! ew::User
