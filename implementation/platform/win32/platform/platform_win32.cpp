/*
**
*/


#include <iostream>

#include <ew/ew_config.hpp>


#include <ew/graphics/gui/gui.hpp>

#include "implementation/platform/win32/platform/platform_win32.hpp"


#include "implementation/platform/win32/graphics/gui/win32_input_device.hpp"
#include "implementation/platform/win32/graphics/gui/win32_video_device.hpp"

namespace ew
{
namespace implementation
{
namespace Platform
{
namespace Win32
{

using namespace ew::core; // for EW_TIMER EW_VIDEO ...
using namespace ew::User;

Win32_Platform::Win32_Platform()
{
	if (ew::core::Platform::getPlatformInstance()) {
		// error
		std::cerr << "Error : a platform dll is already loaded" << "\n";
		systemExit(0);
	}

	std::cerr << "Win32 Platform dll loaded" << "\n";

	ew::core::Platform::setPlatformInstance(this);
}

Win32_Platform::~Win32_Platform()
{

}

const char * Win32_Platform::getName(void) const
{
	return ("Win32");
}



bool   Win32_Platform::init(u32 Flags)
{
	std::cerr << "[ Win32_Platform :: init ]" << std::endl;


	return false;
	return true;
}

bool   Win32_Platform::initSubSystem(u32 Flags)
{
	std::cerr << "[ Win32_Platform :: initSubSystem ]" << std::endl;

	return false;
	return true;
}

bool   Win32_Platform::wasInit(u32 Flags)
{
	std::cerr << "[ Win32_Platform :: wasInit ]" << std::endl;
	return false;
	return true;
}

bool   Win32_Platform::quitSubSystem(u32 Flags)
{
	std::cerr << "[ Win32_Platform :: quitSubSystem ]" << std::endl;

	Flags = 0;
	return (false);
}

s32   Win32_Platform::quit(void)
{
	std::cerr << "[ Win32_Platform :: quit ]" << std::endl;

	return (0);
}

void   Win32_Platform::systemExit(s32 status)
{
	::exit((int)status);
}

// -------------------------------------------------------------------------------------------------


//using namespace ew::User;
//using namespace ew::implementation::Platform::Win32::User;

//static const char * _iuser_inputNames[] = {"Win32", 0}; // DirectInput ...

//const char **             Win32_Platform::get_supported_user_input_interface_names(void)
//{
//  return _iuser_inputNames;
//}

//IUserInputInterface *             Win32_Platform::get_user_input_interface(char * iname)
//{
//  std::cerr << __FUNCTION__ << std::endl;

//  IUserInputInterface * iuser_input = new Win32_IUserInputInterface;

//  return (iuser_input);
//}


//void                 Win32_Platform::release_user_input_interface(IUserInputInterface * iuser_input)
//{
//  delete iuser_input;
//}

// -------------------------------------------------------------------------------------------------
/* video interface */

//using namespace ew::Video;
//using namespace ew::core::Platform::Win32::Video;

//static const char * _ivideoNames[] = {"Win32", 0}; // Win32.Direct3D, Win32.OpenGL

//const char ** Win32_Platform::get_supported_video_interface_names(void)
//{
//  return _ivideoNames;
//}

//IVideoInterface * Win32_Platform::get_video_interface(char * iname)
//{
//  IVideoInterface * ivideo = new Win32_IVideo;
//  return (ivideo);
//}

//void Win32_Platform::release_video_interface(IVideoInterface * ivideo)
//{
//  delete ivideo;
//}

// -------------------------------------------------------------------------------------------------
/* audio interface */
const char ** Win32_Platform::getSupportedAudioInterfaceNames(void)
{
	return 0;
}

IAudioInterface * Win32_Platform::getAudioInterface(char *)
{
	return 0;
}

void Win32_Platform::releaseAudioInterface(IAudioInterface * iaudio)
{

}

// -------------------------------------------------------------------------------------------------

//  using namespace ew::implementation::Platform::Win32::Graphics::gui;

/* graphical user interface */
const char  **  Win32_Platform::getSupportedGraphicalUserInterface(void)
{
	// wgl / directX
	return 0;
}

Gui  * Win32_Platform::getGraphicalUserInterface(char * name)
{
	//
	Gui * gui = 0;
	return gui;
}

void    Win32_Platform::releaseGraphicalUserInterface(Gui * gui)
{
	delete gui;
}


}
}
}
} // ! ew::implementation::Platform::Win32


// ----------------------------------------------------------------------------------------
// factory
// ----------------------------------------------------------------------------------------

static ew::implementation::Platform::Win32::Win32_Platform * win32_platform = 0;

using namespace ew::core::Platform;
using namespace ew::implementation::Platform::Win32;

extern "C"  IPlatform * getPlatformInstance(void)
{
	std::cerr << __FUNCTION__ << std::endl;

	Win32_Platform * _platform = new Win32_Platform();
	return _platform;
}


extern "C"  void  releasePlatformInstance(IPlatform * iplatform)
{
	std::cerr << __FUNCTION__ << std::endl;

	Win32_Platform * _platform = static_cast<Win32_Platform *>(iplatform);
	delete _platform;
}

// -------------------------------------------------------------------------------------

// Init
extern "C"  void initPlatform(void)
{
	win32_platform = new ew::implementation::Platform::Win32::Win32_Platform();

	// This is done in constructor
	// ew::core::Platform::setPlatformInstance(win32_platform);
	std::cerr << "[ Win32_Platform :: Platform initialized ]" << "\n";
}

// Quit
extern "C"  void releasePlatform(void)
{
	delete win32_platform;
	std::cerr << "[ Win32_Platform :: Platform released ]" << "\n";
}


