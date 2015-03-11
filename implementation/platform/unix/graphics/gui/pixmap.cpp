#include <iostream>
#include <assert.h>

#include "x11_libX11.hpp"
#include "x11_libGLX.hpp"
#include "libX11_wrappers.hpp"
#include "x11_libGLX_wrappers.hpp"

#include <ew/graphics/gui/widget/pixmap/pixmap.hpp>
#include "pixmap_private_data.hpp"

#include <ew/graphics/gui/widget/window/window.hpp>
#include "window_private_data.hpp"

namespace ew
{
namespace graphics
{
namespace gui
{

pixmap::pixmap(window * window)
	:
	d(new private_data)
{
	d->_owner = window;
	d->was_init = false;
	resize(window->width(), window->height());
	d->was_init = true;
}

pixmap::~pixmap()
{
	window * window = d->_owner;
	glXDestroyGLXPixmap(window->d->_x11_dpy, d->_glx_pixmap);
	XFreePixmap(window->d->_x11_dpy, d->_x11_pixmap);
	delete d;
}

bool pixmap::resize(u32 width, u32 height)
{
	bool ret = true;
	::Pixmap new_x11_pixmap;
	::GLXPixmap new_glx_pixmap;


	window * window = d->_owner;

	assert(width >= window->width());
	assert(height >= window->height());


	new_x11_pixmap = XCreatePixmap(window->d->_x11_dpy,
				       window->d->_x11_drawable,
				       width, height,
				       window->d->_x11_visualInfo->depth);

	assert(new_x11_pixmap != BadAlloc);
	assert(new_x11_pixmap != BadDrawable);
	assert(new_x11_pixmap != BadValue);

	int scr = DefaultScreen(window->d->getX11Display()); // TODO: move to d->x11_screen_num
	GC  gc = DefaultGC(window->d->getX11Display(), scr); // TODO: move to d->_x11_gc

	// clear pixmap
	XFillRectangle(window->d->_x11_dpy, new_x11_pixmap, gc, 0, 0, width, height);

	new_glx_pixmap = glXCreateGLXPixmap(window->d->_x11_dpy,
					    window->d->_x11_visualInfo,
					    new_x11_pixmap);

	assert(new_x11_pixmap != BadAlloc);
	assert(new_x11_pixmap != BadDrawable);
	assert(new_x11_pixmap != BadValue);


	if (d->was_init == true) {
		glXDestroyGLXPixmap(window->d->_x11_dpy, d->_glx_pixmap);
		XFreePixmap(window->d->_x11_dpy, d->_x11_pixmap);
	}

	d->_x11_pixmap = new_x11_pixmap;
	d->_glx_pixmap = new_glx_pixmap;

	return ret;
}


}
}
}
