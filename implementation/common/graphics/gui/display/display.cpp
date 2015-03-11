#include <iostream>

#include <ew/graphics/gui/display/display.hpp>

namespace ew
{
namespace graphics
{
namespace gui
{

// namespace DISPLAY ?

static ew::graphics::gui::display * _instance = 0;

bool setDisplayInstance(ew::graphics::gui::display * dpy)
{
	_instance = dpy;
	return true;
}

ew::graphics::gui::display * getDisplayInstance()
{
	if (! _instance)
		throw "getDisplayInstance() :: error :: no instance !!";

	return _instance;
}

}
}
}
