#include <ew/graphics/gui/gui.hpp>


namespace ew
{
namespace graphics
{
namespace gui
{

bool init()
{
	return true;
}

bool quit()
{
	return true;
}

bool  setSinglethreadEventPollingMode()
{
	return true;
}

bool  useSinglethreadEventPolling()
{
	return true;
}

Gui::Gui() { }
Gui::~Gui() { }

const char * Gui::class_name() const
{
	return "gui";
}
const char * Gui::getName() const
{
	return "Win32 gui";
}

}
}
}
