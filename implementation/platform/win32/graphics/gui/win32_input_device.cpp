
#include <iostream>

#include "win32_input_device.hpp"


namespace ew
{
namespace implementation
{
namespace Platform
{
namespace Win32
{

namespace User
{
namespace INPUT
{
namespace DEVICE
{

// ---------------------------------------------------------------------------

Win32Joystick::Win32Joystick() : IJoystick()
{
	std::cerr << __FUNCTION__ << std::endl;
}

// ---------------------------------------------------------------------------

Win32Joystick::Win32Joystick(u32 index) : IJoystick(index)
{
	std::cerr << "Win32Joystick::Win32Joystick(" << index << ")"  << std::endl;
}

// ---------------------------------------------------------------------------

Win32Joystick::~Win32Joystick()
{
	std::cerr << __FUNCTION__ << std::endl;

	close();
	_wasOpened = false;
	_nrButtons = 0;
}

// ---------------------------------------------------------------------------
u32  Win32Joystick::getSystemIndex(void)
{
	return _systemIndex;
}

bool Win32Joystick::is_opened(void)
{
	return _wasOpened;
}

bool Win32Joystick::is_closed(void)
{
	return !_wasOpened;
}

bool Win32Joystick::open(void)
{
	return false;
}

bool Win32Joystick::close(void)
{
	return (false);
}

// ---------------------------------------------------------------------------
u32 Win32Joystick::getNumberOfButtons(void)
{
	return 0;
}

void Win32Joystick::getNumberOfButtons(u32 & nrButtons)
{
	nrButtons = 0;
}

bool Win32Joystick::getButtonState(u32 button, enum IButton::State & state)
{
	return false;
}

// ---------------------------------------------------------------------------
void Win32Joystick::getNumberOfAxis(u32 & nrAxis)
{
	nrAxis = 0;
}
u32 Win32Joystick::getNumberOfAxis(void)
{
	return 0;
}

bool Win32Joystick::getAxisValue(u32 axis, s32 & value)
{
	return (false);
}

// ---------------------------------------------------------------------------

bool Win32Joystick::isAnyButtonPressed(void)
{
	u32 button;
	return (Win32Joystick::isAnyButtonPressed(button));
}

// ---------------------------------------------------------------------------

bool Win32Joystick::isAnyButtonPressed(u32 & button)
{
	for (button = 0; button < _nrButtons; ++button) {
		if (isButtonPressed(button) == true)
			return (true);
	}
	return (false);
}


// ---------------------------------------------------------------------------

bool Win32Joystick::isButtonPressed(u32 button)
{
	return (false);
}

// ---------------------------------------------------------------------------

bool Win32Joystick::isButtonRealesed(u32 button)
{
	return (false);
}

void Win32Joystick::getNumberOfHats(u32 & nrHats)
{
	nrHats = 0;
}

u32  Win32Joystick::getNumberOfHats(void)
{
	return 0;
}

// -------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------
Win32_Keyboard::Win32_Keyboard() : IKeyboard()
{

}

Win32_Keyboard::Win32_Keyboard(u32 index)  : IKeyboard(index)
{

}

// ---------------------------------------------------------------------

Win32_Keyboard::~Win32_Keyboard()
{

}

// ---------------------------------------------------------------------
u32  Win32_Keyboard::getSystemIndex(void)
{
	return _systemIndex;
}

bool Win32_Keyboard::is_opened(void)
{
	return _wasOpened;
}

bool Win32_Keyboard::is_closed(void)
{
	return !_wasOpened;
}

bool Win32_Keyboard::open(void)
{
	_wasOpened = true;
	return (true);
}

// ---------------------------------------------------------------------

bool Win32_Keyboard::close(void)
{
	_wasOpened = false;
	return (true);
}

// ---------------------------------------------------------------------

u32 Win32_Keyboard::getNumberOfButtons(void)
{
	return 0;
}

void Win32_Keyboard::getNumberOfButtons(u32 & nrButtons)
{
	nrButtons = 0;
}

bool Win32_Keyboard::isAnyButtonPressed(void)
{
	u32 button;
	return (Win32_Keyboard::isAnyButtonPressed(button));
}

// ---------------------------------------------------------------------

bool Win32_Keyboard::isAnyButtonPressed(u32 & button)
{
	button = 0;
	return false;
}

// ---------------------------------------------------------------------

bool Win32_Keyboard::getButtonState(u32 button, enum IButton::State & state)
{
	return false;
}

// ---------------------------------------------------------------------

bool Win32_Keyboard::isButtonPressed(u32 button)
{
	return false;
}

// ---------------------------------------------------------------------

bool Win32_Keyboard::isButtonRealesed(u32 button)
{
	return false;
}

// ---------------------------------------------------------------------

u8 * Win32_Keyboard::getKeys()
{
	return (u8 *)0;
}

// ---------------------------------------------------------------------------

Win32_Mouse::Win32_Mouse() : IMouse()
{
	_nrButtons = 3;// 5 ?
	_nrAxis = 2; // more ?
}


// ---------------------------------------------------------------------------


Win32_Mouse::Win32_Mouse(u32 index) : IMouse(index)
{
	_nrButtons = 3;// 5 ?
	_nrAxis = 2; // more ?
}

// ---------------------------------------------------------------------------


Win32_Mouse::~Win32_Mouse()
{

}

// ---------------------------------------------------------------------------
u32  Win32_Mouse::getSystemIndex(void)
{
	return _systemIndex;
}

bool Win32_Mouse::is_opened(void)
{
	return _wasOpened;
}

bool Win32_Mouse::is_closed(void)
{
	return !_wasOpened;
}

bool Win32_Mouse::open(void)
{
	_wasOpened = true;
	return (true);
}

// ---------------------------------------------------------------------------

bool Win32_Mouse::close(void)
{
	_wasOpened = false;
	return (true);
}

// ---------------------------------------------------------------------------


bool Win32_Mouse::getButtonState(u32 button, enum IButton::State & state)
{
	return false;
}

// ---------------------------------------------------------------------------
void Win32_Mouse::getNumberOfAxis(u32 & nrAxis)
{
	nrAxis = 0;
}
u32 Win32_Mouse::getNumberOfAxis(void)
{
	return 0;
}

bool Win32_Mouse::getAxisValue(u32 axis, s32 & value)
{
	return false;
}

// ---------------------------------------------
u32 Win32_Mouse::getNumberOfButtons(void)
{
	return 0;
}

void Win32_Mouse::getNumberOfButtons(u32 & nrButtons)
{
	nrButtons = 0;
}

bool Win32_Mouse::isAnyButtonPressed(void)
{
	return false;
}

// ---------------------------------------------

bool Win32_Mouse::isAnyButtonPressed(u32 & button)
{
	return false;
}


// ---------------------------------------------------------------------------

bool Win32_Mouse::isButtonPressed(u32 button)
{
	return false;
}


// ---------------------------------------------------------------------------

bool Win32_Mouse::isButtonRealesed(u32 button)
{
	return false;
}

// ---------------------------------------------------------------------------

}
}
}
}
}
}
} // ! ew::core::Platform::Win32::DEVICE::Input::User


namespace ew
{
namespace implementation
{
namespace Platform
{
namespace Win32
{
namespace User
{

using namespace ew::implementation::Platform::Win32::User::INPUT::DEVICE;

static const char * _sdl_iuser_implementationNames[] = { "Win32", 0 };

const char ** Win32_IUserInputInterface::getimplementationNames(void)
{
	return _sdl_iuser_implementationNames;
}

u32  Win32_IUserInputInterface::getNumberOfKeyboardDevices(void)
{
	std::cerr << __FUNCTION__ << std::endl;
	return (1);
}

// ---------------------------------------------------------------------------

u32  Win32_IUserInputInterface::getNumberOfMouseDevices(void)
{
	std::cerr << __FUNCTION__ << std::endl;
	return (1);
}

// ---------------------------------------------------------------------------

u32  Win32_IUserInputInterface::getNumberOfJoystickDevices(void)
{
	std::cerr << __FUNCTION__ << std::endl;
	return (0);
}

// ---------------------------------------------------------------------------

IKeyboard * Win32_IUserInputInterface::getKeyboardDevice(u32 index)
{
	std::cerr << __FUNCTION__ << std::endl;
	IKeyboard * ikeyboard = new Win32_Keyboard(index);
	return (ikeyboard);

}

// ---------------------------------------------------------------------------

IMouse * Win32_IUserInputInterface::getMouseDevice(u32 index)
{
	std::cerr << __FUNCTION__ << std::endl;
	IMouse * imouse = new Win32_Mouse(index);
	return (imouse);
}

// ---------------------------------------------------------------------------

IJoystick * Win32_IUserInputInterface::getJoystickDevice(u32 index)
{
	std::cerr << __FUNCTION__ << std::endl;
	IJoystick * ijoy = new Win32Joystick(index);
	return (ijoy);
}


// ---------------------------------------------------------------------------

void      Win32_IUserInputInterface::releaseKeyboardDevice(IKeyboard * ikeyboard)
{
	std::cerr << __FUNCTION__ << std::endl;
	delete ikeyboard;
}

// ---------------------------------------------------------------------------

void      Win32_IUserInputInterface::releaseMouseDevice(IMouse * imouse)
{
	std::cerr << __FUNCTION__ << std::endl;
	delete imouse;
}

// ---------------------------------------------------------------------------

void       Win32_IUserInputInterface::releaseJoystickDevice(IJoystick * ijoystick)
{
	std::cerr << __FUNCTION__ << std::endl;
	delete ijoystick;
}

}
}
}
}
} // ! ew::core::Platform::Win32::User
