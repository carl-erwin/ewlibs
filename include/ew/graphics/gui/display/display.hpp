#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/object/object.hpp>
#include <ew/core/interface/lock_interface.hpp>
#include <ew/core/threading/thread.hpp>
#include <ew/graphics/gui/gui.hpp>
#include <ew/video/ivideo.hpp>
#include <ew/user/iuser_input.hpp>
#include <ew/graphics/gui/widget/window/forward.hpp>

namespace ew
{
namespace graphics
{
namespace gui
{


// TODO: #include <ew/graphics/gui/event/forward.hpp>
namespace events
{
struct event;
class  event_dispatcher;
}

using namespace ew::video;
using namespace ew::user;
using namespace ew::core::threading;

class EW_GRAPHICS_EXPORT display :
	public ew::core::object,
	public ew::core::objects::lock_interface
{
public:
	display();
	virtual ~display();

	//
	virtual const char * class_name() const;

	// event
	virtual u32 poll_events(bool block = false, u32 timeout = 250 /* ms */);
	virtual ew::graphics::gui::events::event * pump_event();
	virtual bool push_events(ew::graphics::gui::events::event ** ev, u32 nrEvents, bool force_push = false);
	virtual u32 get_nr_pending_events(u32 type);

	// video interface
	virtual const char ** get_supported_video_interface_names(void) ;
	virtual video_interface * get_video_interface(const char * name = "default") ;
	virtual void release_video_interface(video_interface * ivideoInterface) ;

	// user input interface
	virtual const char ** get_supported_user_input_interface_names(void) ;
	virtual user_input_interface * get_user_input_interface(const char * name = "default") ;
	virtual void release_user_input_interface(user_input_interface * iuser_inputInterface) ;

	// ILockableObject
	virtual bool lock();
	virtual bool unlock();
	virtual bool trylock()
	{
		return false;
	}; // not allowed

	//          virtual bool isLocked();

	virtual ew::core::threading::thread * is_locked_by();

	virtual bool open();    // first method to call
	virtual bool close();   // last method to call implicitly called in destructor

	//
	ew::graphics::gui::events::event_dispatcher * get_event_dispatcher();


private:
	class private_data;
	class private_data * d;

	friend class ew::graphics::gui::window;
	friend class ew::graphics::gui::events::event_dispatcher;
};

}
}
} // ! ew::graphics::gui

