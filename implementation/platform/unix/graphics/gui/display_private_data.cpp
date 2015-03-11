#include "x11/common.hpp"

namespace ew
{
namespace graphics
{
namespace gui
{

////////////////////////////////////////////////////////////////////////////////

/* X11 callbaks : x11/globals.cpp */

int (*display::private_data::X11::default_error_handler_funcptr)(::Display *, ::XErrorEvent *) = 0;
int (*display::private_data::X11::default_ioerror_handler_funcptr)(::Display *) = 0;

int display::private_data::X11::display_error_handler(::Display * dpy, ::XErrorEvent * errorEvent)
{

	int code = default_error_handler_funcptr(dpy, errorEvent);
	char errorMsg[ 512 ];

	// TODO: check ret code
	XGetErrorText(dpy, code, (char *) errorMsg, 512);

	dbg << " X11 ERROR MSG : " << errorMsg << "\n";

	return 0;
}

/*
  TODO: destroy display associeted ressosurce
  for ()
  XKillClient(Display *display, XID resource) )
  SystemForceExit(1);
  throw "gui Display IO error";
*/
int display::private_data::X11::display_ioerror_handler(::Display * x11_dpy)
{
	_exit(1);
	return default_ioerror_handler_funcptr(x11_dpy);
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

/* Widget management : x11/x11_widget.cpp */

/*
  use std::map instead
  use std::vector
  we don't have that much window ???
  lists are slow to walk through
  lock less container ??
*/
bool display::private_data:: have_widget(display::private_data * d, widget * win)
{
	// lock list ?
	std::list< widget * >::iterator it = d->_widget_list.begin();
	std::list< widget * >::iterator end = d->_widget_list.end();
	while (it != end) {
		if (*it++ == win)
			return true;
	}
	return false;
}

window *
display::private_data:: get_window(display::private_data * d, ::Drawable x11_widget)
{
	mutex_locker lock(d->_widget_list_mtx);

	std::list< widget * >::iterator it = d->_widget_list.begin();
	std::list< widget * >::iterator end = d->_widget_list.end();

	while (it != end) {
		window * curWidget = static_cast<window * >(*it++);

		if (!curWidget || !curWidget->d)
			continue;

		if (curWidget->d->_x11_drawable == x11_widget) {
			return curWidget;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void display::private_data::event_dispatcher_thread_function(display * dpy)
{
	// explictly not locked
	while (dpy->d->_is_running == true) {
		dpy->d->get_event_dispatcher()->dispatch_all_events();
	}
}

}
}
}

