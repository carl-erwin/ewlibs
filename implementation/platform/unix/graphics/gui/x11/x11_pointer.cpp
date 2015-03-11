#include "common.hpp"

namespace ew
{
namespace graphics
{
namespace gui
{


/* pointer management */

int display::private_data::X11::ungrab_pointer(ew::graphics::gui::display * dpy, ew::graphics::gui::window * widget)
{
	dpy->lock();
	XUngrabPointer(dpy->d->_x11_dpy, CurrentTime);
	dpy->unlock();
	return 0;
}

int display::private_data::X11::grab_pointer(ew::graphics::gui::display * dpy, ew::graphics::gui::window * widget)
{
	if (!widget)
		return -1;

	/* TODO: define this private data */
	static const unsigned int event_mask = 0
					       | ButtonPressMask
					       | ButtonReleaseMask
					       | PointerMotionMask
					       | ButtonMotionMask
					       | LeaveWindowMask
					       | EnterWindowMask
					       | 0;
	dpy->lock();

	int ret = XGrabPointer(dpy->d->_x11_dpy,
			       widget->d->_x11_drawable,
			       False,
			       event_mask,
			       GrabModeAsync,
			       GrabModeAsync,
			       None,
			       None,
			       CurrentTime);
	dpy->unlock();

	/* TODO: check return type */
	return ret;
}

}
}
}
