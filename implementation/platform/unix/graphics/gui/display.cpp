#include <assert.h>
#include <cstring>
#include <list>

//
// X11 specific
//
#include <X11/X.h>
#include <X11/Xlib.h>

//
// Ew
//
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/thread.hpp>
#include <ew/core/time/time.hpp>
#include <ew/core/interface/lock_interface.hpp>
#include <ew/video/ivideo.hpp>
#include <ew/graphics/gui/display/display.hpp>
#include <ew/graphics/gui/widget/window/window.hpp>
#include <ew/graphics/gui/event/event.hpp>
#include <ew/console/console.hpp>

#include <ew/core/program/environment.hpp>


// Imp
#include "display_private_data.hpp"
#include "x11_libX11.hpp"
#include "libX11_wrappers.hpp"

#include <ew/codecs/text/unicode/utf8/utf8.hpp>

/*
  TODO:
  clean up
  Private data must have a Display * owner;
*/

namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core;
using namespace ew::core::types;
using namespace ew::core::threading;
using namespace ew::graphics::gui::events;

using ew::console::dbg;
using ew::graphics::gui::widget;
using ew::graphics::gui::window;


bool display::push_events(events::event ** evs, u32 nrEvents, bool force_push)
{
	if (force_push) {
		return d->get_event_dispatcher()->push(evs, nrEvents);
	}

	// can be done in the distpatcher itself
	for (u32 i = 0; i < nrEvents; i++) {
		if (! d->have_widget(d, evs[i]->widget)) {
			delete  evs[i];
			evs[i] = 0;
		}
	}

	return d->get_event_dispatcher()->push(evs, nrEvents);
}


u32 display::get_nr_pending_events(u32 type)
{
	return d->get_event_dispatcher()->get_nr_pending_events((event_type)type);
}


events::event *
display::pump_event()
{
	return d->get_event_dispatcher()->pop();
}

u32
display::poll_events(bool block, u32 timeout)
{
	return private_data::X11::poll_events(this , block, timeout);
}


events::event_dispatcher * display::get_event_dispatcher()
{
	return d->get_event_dispatcher();
}

display::display()
	:
	d(new private_data())
{
	// open(); TODO: WE must explicitly call open after display creation
}

display::~display()
{
	close();
	delete d;
}

const char * display::class_name() const
{
	return "ew::graphics::gui::display";
}



/* video interface */
const char ** display::get_supported_video_interface_names(void)
{
	return 0;
}

video_interface * display::get_video_interface(const char * name)
{
	// to do
	return 0;
}

void display::release_video_interface(video_interface * video_intf)
{

}

/* user input interface */

const char ** display::get_supported_user_input_interface_names(void)
{
	return 0;
}

user_input_interface * display::get_user_input_interface(const char * name)
{
	return 0;
}

void display::release_user_input_interface(user_input_interface * iuser_input_intf)
{

}

bool display::lock()
{
	d->lock();
	assert(d->_dpy_owner != ew::core::threading::self());
	d->_dpy_owner = ew::core::threading::self();
	XLockDisplay(d->_x11_dpy);
	return true;
}

bool display::unlock()
{
	assert(d->_dpy_owner == ew::core::threading::self());
	XUnlockDisplay(d->_x11_dpy);
	d->_dpy_owner = 0;
	d->unlock();
	return true;
}

ew::core::threading::thread * display::is_locked_by()
{
	return d->_dpy_owner;
}


bool display::open()
{
	// TODO : add mutex
	if (d->_is_running == true) {
		return true;
	}

	if (multithreadEventPollingIsEnabled() == true) {

		if (XInitThreads() == 0) {
			dbg << "X11::Server::open() : XInitthreads() : error " << "\n";
			dbg << " there is no support for multi threading\n" << "\n";
			return false;
		}
	}

	char * display_env_str = ew::core::program::getenv("DISPLAY");
	d->_x11_dpy = XOpenDisplay(display_env_str);
	if (!d->_x11_dpy) {
		dbg << "X11::Server::open() : XOpenDisplay : error : cannot connect to" << display_env_str << "\n";
		return false;
	}

	// d->X11_setup_error_handling()

	// returns the old io handler
	//  private_data::X11::default_ioerror_handler_funcptr = (int (*)(::Display *)) XSetIOErrorHandler(private_data::X11::display_ioerror_handler) ;
	private_data::X11::default_ioerror_handler_funcptr = reinterpret_cast<int ( *)(::Display *)>(XSetIOErrorHandler(private_data::X11::display_ioerror_handler)) ;

	// returns the old error handler
	private_data::X11::default_error_handler_funcptr = reinterpret_cast<int ( *)(::Display *, ::XErrorEvent *)>(XSetErrorHandler(private_data::X11::display_error_handler));

	// XSetCloseDownMode( d->_x11_dpy, RetainTemporary );

	d->_dpy_fd = ConnectionNumber(d->_x11_dpy);


	// TODO: d->X11_setup_atoms();
	// table lookup
	// { const char * name , atom }
	d->_WM_CLIENT_LEADER = XInternAtom(d->_x11_dpy, const_cast<char *>("WM_CLIENT_LEADER"), False);
	if (d->_WM_CLIENT_LEADER == None) {
		dbg << "Can't set WM_CLIENT_LEADER Atom\n";
		throw "error";
	}

	d->_WM_DELETE_WINDOW = XInternAtom(d->_x11_dpy, const_cast<char *>("WM_DELETE_WINDOW"), False);
	if (d->_WM_DELETE_WINDOW == None) {
		dbg << "Can't set WM_DELETE_WINDOW Atom\n";
		throw "error";
	}

	d->_NET_WM_WINDOW_TYPE = XInternAtom(d->_x11_dpy, const_cast<char *>("_NET_WM_WINDOW_TYPE"), False);
	if (d->_NET_WM_WINDOW_TYPE == None) {
		dbg << "Can't set _NET_WM_WINDOW_TYPE Atom\n";
		throw "error";
	}

	d->_NET_WM_WINDOW_TYPE_MENU = XInternAtom(d->_x11_dpy, const_cast<char *>("_NET_WM_WINDOW_TYPE_MENU"), False);
	if (d->_NET_WM_WINDOW_TYPE_MENU == None) {
		dbg << "Can't set _NET_WM_WINDOW_TYPE_MENU Atom\n";
		throw "error";
	}

	// setup utf8 text codec
	ew::codecs::text::unicode::utf8::init();

	// setup threading model
	if (singlethreadEventPollingIsEnabled() == false) {

		d->_event_poller_thread = new thread((thread::func_t) display::private_data::event_poller_thread_function,
						     (thread::arg_t)this, "Display event thread");
		d->start_event_poller_thread();
	} else {

		// create display event dispatcher
		d->set_event_dispatcher(new events::event_dispatcher(this));
	}

	// add to list of opened display (list/map/vec)
	return true;
}

bool display::close()
{
	if (! d->_x11_dpy)
		return false;

	// stop event thread
	if (singlethreadEventPollingIsEnabled() == false) {
		if (d->_is_running == false) {
			dbg << "X11::Server::close() : server is not running" << "\n";
			return false;
		}
		d->set_running(false);
		d->_event_poller_thread->join();
		delete d->_event_poller_thread;
		d->_event_poller_thread = 0;
	}

	delete d->get_event_dispatcher();
	d->set_event_dispatcher(0);

	// close x11 connection
	if (XCloseDisplay(d->_x11_dpy) == BadGC) {
		dbg << "X11::Server::close() : error : XCloseDisplay(_x11_dpy)\n";
		return false;
	}

	d->_x11_dpy = 0;

	return true;
}

}
}
}
