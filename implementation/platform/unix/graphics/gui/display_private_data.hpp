#pragma once

//
#include <iostream>
#include <list>

//
// X11 specific
//
#include <X11/X.h>
#include <X11/Xlib.h>

//
// Ew
//

#include <ew/core/interface/lock_interface.hpp>
#include <ew/core/object/object_locker.hpp>

#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/mutex_locker.hpp>
#include <ew/core/threading/thread.hpp>

#include <ew/core/time/time.hpp>

#include <ew/video/ivideo.hpp>
#include <ew/graphics/gui/display/display.hpp>
#include <ew/graphics/gui/widget/window/window.hpp>

#include <ew/graphics/gui/event/event.hpp>

#include "window_private_data.hpp"


namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::objects;
using namespace ew::core::threading;

/////////////////////////////////////////////////////////////////////////////////////////////
// EXPERIMENTAL
/////////////////////////////////////////////////////////////////////////////////////////////

using ew::graphics::gui::display;
using ew::graphics::gui::window;

class display::private_data : public ew::core::threading::mutex
{
public:

	private_data()
		:
		//Mutex(RECURSIVE_MUTEX),
		_dpy_owner(0),
		_x11_dpy(0),
		_dpy_fd(-1),
		_root_window(0),
		_is_running(false),
		_event_poller_thread(0),
		_default_event_dispatcher(0),
		_widget_list(0)
	{
	}

	~private_data()
	{
	}

	// move
	class X11   /* TODO: common interface to create and share with window ? */
	{
	public:

		static bool translate_gui_event(ew::graphics::gui::display * guiDpy, const ::XEvent * xevent, ew::graphics::gui::window * widget, events::event ** event_vec, u32 & lastPushedEvent);
		static bool x11key_to_unicode(ew::graphics::gui::display * guiDpy, const ::XEvent * xevent, events::keyboard_event * ev);
		static enum events::keys::key_value xkeysym_to_event_key(const KeySym keysym);

		static u32  poll_events(ew::graphics::gui::display * dpy, bool block = true, u32 timeout = 250 /* ms */);

		/* TODO: add comment */
		static int (*default_error_handler_funcptr)(::Display *, ::XErrorEvent *);
		/* TODO: add comment */
		static int (*default_ioerror_handler_funcptr)(::Display *);

		static int            display_error_handler(::Display * dpy , ::XErrorEvent * errorEvent);
		static int            display_ioerror_handler(::Display * dpy);

		static int            get_event(::Display * dpy, ::XEvent * xevent);
		static int            grab_pointer(ew::graphics::gui::display * dpy, ew::graphics::gui::window * widget);
		static int            ungrab_pointer(ew::graphics::gui::display * dpy, ew::graphics::gui::window * widget);
		static bool           get_event_queue_size(ew::graphics::gui::display * dpy, u32 * nr);

		////////////////////////////////////////////////////////////////////////////////
		// X11 info
		int get_version();
	};

	X11 * x11_drv;

	// class EVENT {
	// public:
	static void     event_poller_thread_function(ew::graphics::gui::display * dpy);
	static void     event_dispatcher_thread_function(ew::graphics::gui::display * dpy);

	static bool     have_widget(display::private_data * d,  ew::graphics::gui::widget * win);
	static window * get_window(display::private_data * d, ::Drawable x11_widget);

	// INTERNAL STATE
	void                  set_running(bool val);
	bool                  is_running();

	bool                  start_event_poller_thread();

	::Display      *      get_x11_display();  // return the XWindow display connection
	::Window              get_x11_root_window();  // return the XWindow root Window Id

	// WIDGET HANDLING //
	// get the class Window pointer associated to an internal window
	void  add_widget(widget * widget);
	void  del_widget(widget * widget);
	u32   get_widget_number();

	// Display DATA
	ew::core::threading::thread * _dpy_owner; // the locking thread

	// TODO:
	// move to class X11 instance
	// d->x11()->display
	// d->x11()->connection_fd
	// d->x11()->rootWindow
	// d->x11()->running

	::Display * _x11_dpy;
	int         _dpy_fd;
	::Window    _root_window;     // make a pointer to Window * that holds all informations of the root window
	bool        _is_running;      // replace by isAvailable()

	// EVENT HANDLING //
	ew::core::threading::thread * _event_poller_thread; // this thread will launch the dispatcher thread

	ew::graphics::gui::events::event_dispatcher * _default_event_dispatcher;
	ew::graphics::gui::events::event_dispatcher * get_event_dispatcher()
	{
		return _default_event_dispatcher;
	}

	void set_event_dispatcher(ew::graphics::gui::events::event_dispatcher * evd)
	{
		_default_event_dispatcher = evd;
	}

	// WIDGET HANDLING
	// widget container for the display
	// widget list  for this display
	std::list<ew::graphics::gui::widget *> _widget_list;
	ew::core::threading::mutex _widget_list_mtx;

	// decoration handling ...
	// common to all display ?
	/* to catch close event from window manager */
	//  X11_Atoms []
	Atom _WM_CLIENT_LEADER;
	Atom _WM_DELETE_WINDOW;
	Atom _NET_WM_WINDOW_TYPE;
	Atom _NET_WM_WINDOW_TYPE_MENU;
	Atom _NET_WM_WINDOW_TYPE_NORMAL;
	Atom _NET_WM_WINDOW_TYPE_DOCK;
	Atom _NET_WM_WINDOW_TYPE_TOOLBAR;
	Atom _NET_WM_WINDOW_TYPE_UTILITY;
	Atom _NET_WM_WINDOW_TYPE_SPLASH;
	Atom _NET_WM_WINDOW_TYPE_DIALOG;
};

inline void  display::private_data::set_running(bool val)
{
	_is_running = val;
}

inline bool  display::private_data::is_running()
{
	return _is_running;
}

inline ::Display  * display::private_data::get_x11_display()
{
	return _x11_dpy;
}

inline ::Window     display::private_data::get_x11_root_window()
{
	return _root_window;
}

inline void  display::private_data::add_widget(ew::graphics::gui::widget * widget)
{
	mutex_locker lock(_widget_list_mtx);
	_widget_list.push_back(widget);
}

inline void  display::private_data::del_widget(ew::graphics::gui::widget * widget)
{
	{
		mutex_locker lock(_widget_list_mtx);
		_widget_list.remove(widget);
	}

	// push a special event for event dispatcher
	// instead of direct call
	// internal events
	widget->display()->d->_default_event_dispatcher->remove_widget_events(widget);
}

inline u32   display::private_data::get_widget_number()
{
	mutex_locker lock(_widget_list_mtx);
	return _widget_list.size();
};


////////////////////////////////////////////////////////////////////////////////


}
}
}
