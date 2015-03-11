#pragma once


#include <ew/ew_config.hpp>

#include <ew/user/iuser_input.hpp> // user input management

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

using namespace ew::core::DEVICE; /* for IButton */

// -----------------------------------------------------------------------------

using namespace ew::core::DEVICE::INPUT;

class EW_PLATFORM_EXPORT Win32Joystick : public ew::User::DEVICE::IJoystick
{
public:
	Win32Joystick();
	Win32Joystick(u32 index);
	virtual ~Win32Joystick();

	const char * class_name(void) const
	{
		return 0;
	};
	const char * getName() const
	{
		return 0;
	};


	virtual u32  getSystemIndex(void);
	virtual bool is_opened(void);
	virtual bool is_closed(void);

	virtual bool open(void);
	virtual bool close(void);

	//
	virtual u32 getNumberOfButtons(void);
	virtual void getNumberOfButtons(u32 & nrButtons);

	virtual bool isAnyButtonPressed(void);
	virtual bool isAnyButtonPressed(u32 & button);   //
	//
	virtual bool getButtonState(u32 button, enum IButton::State & state); // PRESSED RELEASED

	virtual bool isButtonPressed(u32 button);    //
	virtual bool isButtonRealesed(u32 button);    //

	//
	virtual void getNumberOfAxis(u32 & nrAxis);
	virtual u32  getNumberOfAxis(void);
	virtual bool getAxisValue(u32 axis, s32 & value);   //

	virtual void getNumberOfHats(u32 & nrHats);
	virtual u32  getNumberOfHats(void);

private:

	u32      _systemIndex;  // system index
	bool     _wasOpened; // open/close flag
	u32      _nrButtons;
	u32      _nrAxis;

	// ajouter une methode pour retourner le handle
};

// -----------------------------------------------------------------------------

class EW_PLATFORM_EXPORT Win32_Keyboard : public ew::User::DEVICE::IKeyboard
{
public:
	Win32_Keyboard();
	Win32_Keyboard(u32 index);
	virtual ~Win32_Keyboard();

	const char * class_name(void) const
	{
		return 0;
	};
	const char * getName() const
	{
		return 0;
	};

	virtual u32  getSystemIndex(void);
	virtual bool is_opened(void);
	virtual bool is_closed(void);

	virtual bool open(void);                                  // assume we have a working keyboard
	virtual bool close(void);                                 //

	virtual u32 getNumberOfButtons(void);
	virtual void getNumberOfButtons(u32 & nrButtons);

	virtual bool isAnyButtonPressed(void);                          // WARNING return all keys   NUMMLOCK etc ...
	virtual bool isAnyButtonPressed(u32 & button);                  // WARNING return all keys   NUMMLOCK etc ...

	virtual bool getButtonState(u32 button, enum IButton::State & state);    //

	virtual bool isButtonPressed(u32 button);                               //
	virtual bool isButtonRealesed(u32 button);                              //

	virtual u8 * getKeys(void);

private:
	u32      _systemIndex;  // system index
	bool     _wasOpened; // open/close flag
};

// -----------------------------------------------------------------------------

class EW_PLATFORM_EXPORT Win32_Mouse : public ew::User::DEVICE::IMouse
{
public:
	Win32_Mouse();
	Win32_Mouse(u32 index);
	virtual ~Win32_Mouse();

	const char * class_name(void) const
	{
		return 0;
	};
	const char * getName() const
	{
		return 0;
	};


	virtual u32  getSystemIndex(void);
	virtual bool is_opened(void);
	virtual bool is_closed(void);

	virtual bool open(void);
	virtual bool close(void);
	//
	//
	virtual u32 getNumberOfButtons(void);
	virtual void getNumberOfButtons(u32 & nrButtons);

	virtual bool isAnyButtonPressed(void);                          //
	virtual bool isAnyButtonPressed(u32 & button);                          //

	virtual bool getButtonState(u32 button, enum IButton::State & state);            //
	virtual bool isButtonPressed(u32 button);                               //
	virtual bool isButtonRealesed(u32 button);                              //
	//
	virtual void getNumberOfAxis(u32 & nrAxis);
	virtual u32 getNumberOfAxis(void);
	virtual bool getAxisValue(u32 axis, s32 & value);                       // X = 0 , Y = 1

private:
	u32      _systemIndex;  // system index
	bool     _wasOpened; // open/close flag
	u32      _nrButtons;
	u32      _nrAxis;
};


}
}
}
}
}
}
}


// -----------------------------------------------------------------------------


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

using namespace ew::core::types;
using namespace ew::User::DEVICE;

class EW_PLATFORM_EXPORT Win32_IUserInputInterface : public ew::User::IUserInputInterface
{
public:
	const char * class_name(void) const
	{
		return 0;
	};
	const char * getName() const
	{
		return 0;
	};


	virtual const char ** getimplementationNames(void);

	virtual u32  getNumberOfKeyboardDevices(void);
	virtual u32  getNumberOfMouseDevices(void);
	virtual u32  getNumberOfJoystickDevices(void);

	virtual IKeyboard * getKeyboardDevice(u32 index);
	virtual IMouse * getMouseDevice(u32 index);
	virtual IJoystick * getJoystickDevice(u32 index);

	virtual void        releaseKeyboardDevice(IKeyboard * ikeyboard);
	virtual void        releaseMouseDevice(IMouse * imouse);
	virtual void        releaseJoystickDevice(IJoystick * ijoystick);
};

}
}
}
}
}
