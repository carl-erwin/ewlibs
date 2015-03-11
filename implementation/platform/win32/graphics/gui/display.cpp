
#include <ew/ew_config.hpp>

#include <ew/graphics/gui/display/display.hpp>



namespace ew
{
namespace graphics
{
namespace gui
{

// namespace DISPLAY ?

using namespace ew::Video;
using namespace ew::User;


class display::private_data
{
public:

};


display::Display()
	: d(new private_data)
{

}

display::~Display()
{
	delete d;
}


const char * display::class_name() const
{
	return "";
}

const char * display::getName() const
{
	return "";
}


/* video interface */
const char  ** display::get_supported_video_interface_names(void)
{
	return 0;
}

IVideoInterface * display::get_video_interface(char * name)
{
	return 0;
}

void   display::release_video_interface(IVideoInterface * ivideoInterface)
{

}


/* user input interface */
const char  ** display::get_supported_user_input_interface_names(void)
{
	return 0;
}

IUserInputInterface * display::get_user_input_interface(char * name)
{
	return 0;
}

void   display::release_user_input_interface(IUserInputInterface * iuser_inputInterface)
{

}


// we must be able to lock x11 display
bool display::lock()
{
	return true;
}

bool display::unlock()
{
	return true;
}

bool display::trylock()
{
	return true;
}

// first methods to call
bool display::open()
{
	return true;
}

bool display::close()
{
	return true;
}


// event
u32  display::poll_events()
{
	return 0;
}

ew::graphics::gui::events::Event  * display::pumpEvent()
{
	return 0;
}

bool  display::pushEvent(ew::graphics::gui::events::Event * ev)
{
	return true;
}


}
}
}
