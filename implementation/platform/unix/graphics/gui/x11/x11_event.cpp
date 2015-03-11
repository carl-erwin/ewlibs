#include "common.hpp"

namespace ew
{
namespace graphics
{
namespace gui
{

/* Event handling : x11/x11_event.cpp */

int display::private_data::X11::get_event(::Display * dpy, ::XEvent * xevent)
{
	int ret = XNextEvent(dpy, xevent);
	return ret;
}

void display::private_data::event_poller_thread_function(ew::graphics::gui::display * dpy)
{

	// move this to display
	dpy->d->set_event_dispatcher(new events::event_dispatcher(dpy));
	dpy->d->_is_running = true;

	threading::thread * event_dispatcher_thread;

	event_dispatcher_thread = new thread((thread::func_t)display::private_data::event_dispatcher_thread_function, dpy, "Event Dispatcher");

	event_dispatcher_thread->start();
	ew::core::time::sleep(10);
	// do not lock
	while (dpy->d->_is_running == true) {
		display::private_data::X11::poll_events(dpy);
	}

	dpy->d->get_event_dispatcher()->stop();
	event_dispatcher_thread->join();
	delete event_dispatcher_thread;
	event_dispatcher_thread = 0;

	delete dpy->d->get_event_dispatcher();
	dpy->d->set_event_dispatcher(0);
}

bool display::private_data::start_event_poller_thread()
{
	return _event_poller_thread->start();
}

bool display::private_data::X11::get_event_queue_size(ew::graphics::gui::display * dpy, u32 * nr)
{
	dpy->lock();
	int ret = XPending(dpy->d->_x11_dpy);
	dpy->unlock();
	if (ret < 0)
		return false;

	*nr = (u32)ret;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// TODO: propagate waitTime to dpy->poll*...

u32 display::private_data::X11::poll_events(ew::graphics::gui::display * dpy , bool block, u32 waitTime)
{

	auto evdispatch = dpy->get_event_dispatcher();
	auto pending = evdispatch->get_queue_size();

	u32 nr_pending_x11_events; // nr_pending_x11_events
	X11::get_event_queue_size(dpy, &nr_pending_x11_events);

	// poll the 2 queue in mono thread
	{
		do {
			// user "high" level events was push in second queue
			pending = evdispatch->get_queue_size();
			if (pending)
				break;

			if (nr_pending_x11_events)
				break;

			fd_set read_fdset;
			FD_ZERO(&read_fdset);
			FD_SET(dpy->d->_dpy_fd, &read_fdset);

			u32 timeout = waitTime * 1000;
			struct timeval tv = {
				0, timeout
			};


			int ret = sys_select(dpy->d->_dpy_fd + 1,
					     &read_fdset,
					     (fd_set *)NULL,
					     (fd_set *)NULL,
					     &tv);

			if (ret > 0 && (FD_ISSET(dpy->d->_dpy_fd, &read_fdset)))
				break;

			if (dpy->d->get_widget_number() == 0)
				break;


		} while (block);
	}

	X11::get_event_queue_size(dpy, &nr_pending_x11_events);
	if (nr_pending_x11_events) {
		::XEvent * xevent_vec = new ::XEvent [nr_pending_x11_events];
		dpy->lock();
		for (u32 i = 0; i < nr_pending_x11_events; i++) {
			X11::get_event(dpy->d->_x11_dpy, &xevent_vec[i]);
		}
		dpy->unlock();

		events::event ** event_vec = new events::event * [nr_pending_x11_events * 2];
		u32 pushedEvent = 0;
		for (u32 i = 0; i < nr_pending_x11_events; i++) {
			::XEvent * xevent = &xevent_vec[i];
			ew::graphics::gui::window * window = get_window(dpy->d, xevent->xany.window);
			translate_gui_event(dpy, xevent, window, event_vec, pushedEvent);
		}

		if (pushedEvent) {
			dpy->push_events(event_vec, pushedEvent, true /* force push */);

			pending += pushedEvent;
		}

		// put in private data to avoid multiple alloc/free
		delete [] event_vec;
		delete [] xevent_vec;
	}

	X11::get_event_queue_size(dpy, &nr_pending_x11_events);
//    std::cerr << __FUNCTION__ << " LEAVE : nr_pending_x11_events : " << nr_pending_x11_events << "\n";

	return pending;
}



/*
  \brief This function tranlate low level gui event (X11) to class ew::graphics::gui::events::event

  Warning : Some events does not have an associated Widget pointer
*/
bool display::private_data::X11::translate_gui_event(ew::graphics::gui::display * guiDpy,
		const ::XEvent * xevent,
		window * widget,
		events::event ** event_vec,
		u32 & lastPushedEvent)
{
	using namespace ew::graphics::gui::events;

	// time
	u32 event_time = ew::core::time::get_ticks();

	switch (xevent->type) {
	case KeyPress:
	case KeyRelease: {

		if (!widget)
			return 0;

		if (! widget->isAvailable())
			return 0;

		keyboard_event * ev = new struct keyboard_event;
		ev->display = guiDpy;
		ev->time = event_time;
		ev->x = xevent->xkey.x;
		ev->y = xevent->xkey.y;
		ev->widget = widget;
		ev->unicode = 0; // should be a special var no_unicode
		if (xevent->type == KeyPress) {
			ev->type = KeyPressEvent;
			x11key_to_unicode(guiDpy, xevent, ev);
		} else {
			ev->type = KeyReleaseEvent;
			// TODO:
			// x11keycode_to_unicode(guiDpy, xevent, ev);
		}

		// std::cerr << " xevent->xkey.keycode = " << xevent->xkey.keycode << "\n";
		event_vec[lastPushedEvent++] = ev;
		return true;
	}
	break ;

	// ----------------------

	case ButtonPress:
	case ButtonRelease: {
		EW_DEBUG(dbg << "[event] ButtonPress or ButtonRelease\n";)

		assert(widget != 0);

		/*
		  must remove this, the event dispatcher will do it
		*/
		if (! widget->isAvailable())
			return 0;

		button_event * ev = new button_event();

		ev->display = guiDpy;
		ev->time = event_time;
		if (xevent->type == ButtonPress) {

			// TODO
			// add :
			// widget->setManualGrab(true);
			// widget->gettManualGrab();

			// widget->d->_manual_grab = true;
			// private_data::X11::grab_pointer(guiDpy, widget);

			ev->type = ButtonPressEvent;
		} else {

			// widget->d->_manual_grab = false;
			// private_data::X11::ungrab_pointer(guiDpy, widget);

			ev->type = ButtonReleaseEvent;
		}

		ev->button = xevent->xbutton.button;
		ev->x = xevent->xbutton.x;
		ev->y =  xevent->xbutton.y;
		ev->widget = widget;

		event_vec[lastPushedEvent++] = ev;
		return true;
	}
	break ;

	case ConfigureNotify: {
		// pass the new size/dim to user class
		// build ew resize/move events

		EW_DEBUG(dbg << "[event] ConfigureNotify\n";)

		if (! widget)
			return 0;

		if (! widget->isAvailable())
			return 0;

		bool update_dimension = false;
		bool update_pos = false;

		//  dbg << "ConfigureNotify + widget ptr\n";
		if (((u32)xevent->xconfigure.width != widget->d->_properties.width) ||
		    ((u32)xevent->xconfigure.height != widget->d->_properties.height)) {
			update_dimension = true;
		}

		if ((xevent->xconfigure.x != widget->d->_properties.x)  || (xevent->xconfigure.y != widget->d->_properties.y))
			update_pos = true;

		if (!update_dimension) {
			if (xevent->xconfigure.x != widget->d->_properties.x)
				update_pos = true;

			if (xevent->xconfigure.y != widget->d->_properties.y)
				update_pos = true;
		}

		// work arround
		if (widget->d->_properties.x != 0 && widget->d->_properties.y != 0) {
			if (!xevent->xconfigure.x && !xevent->xconfigure.y)
				update_pos = false;
		}

		// build events
		// do not forget that push trigger on* callbacks
		// TODO : add push_events(Event *[], size)
		if (update_pos) {
			struct widget_event * ev = new struct widget_event();
			ev->display = guiDpy;
			ev->type = WidgetMotionEvent;
			ev->widget = widget;
			ev->time = event_time;
			ev->x = widget->d->_properties.x = (s32)xevent->xconfigure.x;
			ev->y = widget->d->_properties.y = (s32)xevent->xconfigure.y;

			event_vec[lastPushedEvent++] = ev;
		}

		if (update_dimension) {

			struct widget_event * ev = new struct widget_event();
			ev->display = guiDpy;
			ev->type = WidgetResizeEvent;
			ev->widget = widget;
			ev->time = event_time;
			ev->width  = widget->d->_properties.width = (u32) xevent->xconfigure.width;
			ev->height = widget->d->_properties.height = (u32) xevent->xconfigure.height;

			event_vec[lastPushedEvent++] = ev;
		}
	}
	break ;

	/*  */
	case ClientMessage: {
		dbg << "[ event ] ClientMessage\n";

		if (! widget)
			return 0;

		if (! widget->isAvailable())
			return 0;

		if (xevent->xclient.format == 32) {
			//       if ( ( u32 ) ( xevent->xclient.data.l[ 0 ] ) == ( u32 ) guiDpy->d->X11_ATOMS._WM_DELETE_WINDOW )

			if ((u32)(xevent->xclient.data.l[ 0 ]) == (u32) guiDpy->d->_WM_DELETE_WINDOW) {
				//    dbg << " ClientMessage :: WM_DELETE_WINDOW\n";

				struct widget_event * ev = new widget_event();
				ev->display = guiDpy;
				ev->type = WidgetCloseEvent;
				ev->widget = widget;
				ev->time = event_time;

				event_vec[lastPushedEvent++] = ev;
				return true;
			}
		}
	}
	break ;

	case MotionNotify: {
		EW_DEBUG(dbg << "[ event ] MotionNotify\n";)

		if (! widget)
			return 0;

		if (! widget->isAvailable())
			return 0;

		// build event
		struct pointer_event * ev = new struct pointer_event;
		ev->display = guiDpy;
		ev->widget = widget;
		ev->time = event_time;
		ev->type = PointerMotionEvent;
		ev->x = ((XMotionEvent *)xevent)->x;
		ev->y = ((XMotionEvent *)xevent)->y;


		// std::cerr << __PRETTY_FUNCTION__ << " motion(" << ev->x << "," << ev->y  << ")\n";

		event_vec[lastPushedEvent++] = ev;
		return true;
	}
	break ;

	case EnterNotify:
	case LeaveNotify: {

		EW_DEBUG(
		if (xevent->type == EnterNotify) {
		dbg << "[event] EnterNotify\n";
	} else {
		dbg << "[event] LeaveNotify\n";
	}
	)

		if (! widget)
			return 0;

		if (! widget->isAvailable())
			return 0;

		// build event
		struct pointer_event * ev = new struct pointer_event;
		ev->display = guiDpy;
		ev->widget = widget;
		ev->time = event_time;

		if (xevent->type == EnterNotify) {
			ev->type = PointerEnterEvent;
		} else {
			ev->type = PointerLeaveEvent;
		}

		ev->x = ((XEnterWindowEvent *)xevent)->x;
		ev->y = ((XEnterWindowEvent *)xevent)->y;
		// ad this ??
		// ev->x_root = ((XEnterWindowEvent *)xevent)->x_root;
		// ev->y_root = ((XEnterWindowEvent *)xevent)->y_root;

		// dbg << "enter/leave : mouse (X , Y) = (" << ((XMotion_event *)xevent)->x << " , "<< ((XMotionEvent *)xevent)->y << ")" << "\n";

		event_vec[lastPushedEvent++] = ev;
		return true;
	}
	break ;

	case FocusIn:
	case FocusOut: {
		EW_DEBUG(
		if (xevent->type == FocusIn) {
		dbg << "[event] FocusIn\n";
	} else {
		dbg << "[event] FocusOut\n";
	}
	)

		// destroyed window ???
		if (! widget)
			return 0;

		struct widget_event * ev =  new struct widget_event();
		ev->widget = widget;
		ev->display = guiDpy;
		ev->time = event_time;
		if (xevent->type == FocusIn) {
			ev->type =  WidgetFocusInEvent;
		} else {
			ev->type =  WidgetFocusOutEvent;
		}

		event_vec[lastPushedEvent++] = ev;
		return true;
	}
	break ;

	case KeymapNotify: {
		EW_DEBUG(dbg << "[event] KeymapNotify\n";)
	}
	break ;

	case Expose: {
		EW_DEBUG(dbg << "[event] Expose\n";)

		if (! widget)
			return 0;

		if (! widget->isAvailable())
			return 0;

		struct widget_event * ev =  new struct widget_event();
		ev->display = guiDpy;
		ev->type = WidgetDrawEvent;
		ev->widget = widget;
		ev->time = event_time;

		event_vec[lastPushedEvent++] = ev;
		return true;
	}
	break ;

	case GraphicsExpose: {
		EW_DEBUG(dbg << "[event] GraphicsExpose\n";)
	}
	break ;

	case NoExpose: {
		// dbg << "[event] NoExpose\n";
	}
	break ;

	case VisibilityNotify: {
		dbg << "[event] VisibilityNotify\n";
	}
	break ;

	case CreateNotify: {
		dbg << "[event] CreateNotify\n";
	}
	break ;

	case DestroyNotify: {
		dbg << "[event] DestroyNotify\n";

		if (! widget)
			return 0;

		dbg << " remove widget from list\n";
		// TODO : must send  new struct widget_event();
		// ev->type = WidgetDestroyEvent;
		// ev->widget = widget;
		// ev->display = guiDpy;
		// ev->time = event_time;

		widget->d->_destroyAck = true;
		guiDpy->d->del_widget(widget);
		widget->display()->d->get_event_dispatcher()->remove_widget_events(widget);
	}
	break ;

	case UnmapNotify: {
		dbg << "[event] UnmapNotify\n";
	}
	break ;

	case MapNotify: {
		dbg << "[event] MapNotify\n";

		struct widget_event * ev =  new struct widget_event();
		ev->type = WidgetMapEvent;
		ev->widget = widget;
		ev->display = guiDpy;
		ev->time = event_time;

		event_vec[lastPushedEvent++] = ev;
		return true;
	}
	break ;

	case MapRequest: {
		dbg << "[event] MapRequest\n";
	}
	break ;

	case ReparentNotify: {
		dbg << "[event] ReparentNotify\n";

		struct widget_event * ev = new struct widget_event();
		ev->display = guiDpy;
		ev->type = WidgetResizeEvent;
		ev->widget = widget;
		ev->time = event_time;
		ev->width  = widget->width();
		ev->height = widget->height();
		event_vec[lastPushedEvent++] = ev;
	}
	break ;

	case ConfigureRequest: {
		dbg << "[event] ConfigureRequest\n";
	}
	break ;

	case GravityNotify: {
		dbg << "[event] GravityNotify\n";
	}
	break ;

	case ResizeRequest: {
		dbg << "[event] ResizeRequest\n";
	}
	break ;

	case CirculateNotify: {
		dbg << "[event] CirculateNotify\n";
	}
	break ;

	case CirculateRequest: {
		dbg << "[event] CirculateRequest\n";
	}
	break ;

	case PropertyNotify: {
		dbg << "[event] PropertyNotify\n";
	}
	break ;

	case SelectionClear: {
		dbg << "[event] SelectionClear\n";
	}
	break ;

	case SelectionRequest: {
		dbg << "[event] SelectionRequest\n";
	}
	break ;

	case SelectionNotify: {
		dbg << "[event] SelectionNotify\n";
	}
	break ;

	case ColormapNotify: {
		dbg << "[event] ColormapNotify\n";
	}
	break ;

	case MappingNotify: {
		dbg << "[event] MappingNotify\n";
	}
	break ;

	default: {
		dbg << "[event] unhandled xevent type ??? " << (int) xevent->type << "\n";
	}
	}

	return 0;
}

}
}
}
