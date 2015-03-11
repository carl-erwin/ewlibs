#pragma once

#include <ew/ew_config.hpp>

#include <ew/core/platform/iplatform.hpp>

#include <ew/video/ivideo.hpp>
#include <ew/user/iuser_input.hpp>
#include <ew/audio/iaudio.hpp>

#include <ew/graphics/gui/gui.hpp>

namespace ew
{
namespace implementation
{
namespace Platform
{
namespace Win32
{


using namespace ew::Video;
using namespace ew::User;
using namespace ew::Audio;
using namespace ew::graphics::gui;

class EW_PLATFORM_WIN32_EXPORT Win32_Platform : public ew::core::Platform::IPlatform
{
public:
	Win32_Platform();
	virtual ~Win32_Platform();

	/* platform specific */
	virtual   const char       *      getName(void) const;

	virtual bool                 init(u32 Flags);
	virtual bool                 initSubSystem(u32 Flags);
	virtual bool                 wasInit(u32 Flags);
	virtual bool                 quitSubSystem(u32 Flags);
	virtual s32                  quit(void);
	virtual void                 systemExit(s32 status);


	/* audio interface */
	virtual const char  **  getSupportedAudioInterfaceNames(void);
	virtual IAudioInterface  * getAudioInterface(char * name = "default");
	virtual void    releaseAudioInterface(IAudioInterface * iaudioInterface);

	/* graphical user interface */
	virtual  const char  ** getSupportedGraphicalUserInterface(void);
	virtual  Gui * getGraphicalUserInterface(char * name = "default");
	virtual  void   releaseGraphicalUserInterface(Gui * gui);

};

}
}
}
} // ! ew::implementation::Platform::Win32

/* --------------------------------------------------------------------------------------- */
// factory
/* --------------------------------------------------------------------------------------- */

extern "C"  EW_PLATFORM_WIN32_EXPORT ew::core::Platform::IPlatform  * getPlatformInstance(void);
extern "C"  void  EW_PLATFORM_WIN32_EXPORT releasePlatformInstance(ew::core::Platform::IPlatform * iplatform);

// export for programs
extern "C" EW_PLATFORM_WIN32_EXPORT void initPlatform(void);
extern "C" EW_PLATFORM_WIN32_EXPORT void releasePlatform(void);

/* --------------------------------------------------------------------------------------- */
