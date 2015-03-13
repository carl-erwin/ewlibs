#include <iostream>
#include <mutex>


#include <ew/ew_config.hpp>
#include <ew/graphics/gui/gui.hpp>





#include "x11_libX11.hpp"
#include "x11_libGLX.hpp"

namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::objects;

// should handle multiple gui objects ?
static bool   wasInit = false;
  static std::mutex  gui_mtx;

bool  init()
{
  std::lock_guard<std::mutex> mTxLock(gui_mtx);

	if (wasInit == true)
		return false;

	load_libX11_dll();
	load_libGLX_dll();

	wasInit = true;

	return true;
}


bool  quit()
{
    std::lock_guard<std::mutex> mTxLock(gui_mtx);

	if (wasInit == false)
		return false;

	// TODO: ensure there are no gui/event thread running

	// reset pointers
	// must point to fake implementation to avoid faults
	// unload X11 dll
	unload_libGLX_dll();
	unload_libX11_dll();

	wasInit = false;

	return true;
}

bool _useSinglethreadEventPolling = false;

bool EW_GRAPHICS_EXPORT setSinglethreadEventPollingMode()
{
	_useSinglethreadEventPolling = true;
	return true;
}

bool EW_GRAPHICS_EXPORT singlethreadEventPollingIsEnabled()
{
	return _useSinglethreadEventPolling;
}

bool EW_GRAPHICS_EXPORT multithreadEventPollingIsEnabled()
{
	return _useSinglethreadEventPolling == false;
}

gui::gui()
{
}

gui::~gui()
{
}

const char * gui::class_name() const
{
	return "gui";
}

}
}
}
