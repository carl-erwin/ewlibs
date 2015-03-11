#include <assert.h>
#include <iostream>

#include <ew/core/exception/exception.hpp>
#include <ew/core/time/time.hpp>
#include <ew/core/threading/thread.hpp>
#include <ew/core/object/object_locker.hpp>

#include "../gui/window_private_data.hpp"
#include "../gui/pixmap_private_data.hpp"

#include "../gui/x11_libX11.hpp"
#include "../gui/x11_libGLX.hpp"

#include <ew/graphics/renderer/rendering_context.hpp>

#include "rendering_context_private_data.hpp"

/*
  TODO: check X11 dpy locking scheme : add assert

*/

namespace ew
{
namespace graphics
{
namespace rendering
{

using namespace ew::core::types;

using namespace ew::core::threading;

using namespace ew::core::objects;

using namespace ew::graphics::gui;

// _____________________________________________________________

rendering_context::rendering_context(ew::graphics::gui::window * window)
	: d(new private_data)
{
	mutex_locker lock(d);
	d->window = window;

	Bool direct = True;
	if (window->d->_offscreen_pixmap) {
		d->_glxDrawable = window->d->_offscreen_pixmap->d->_glx_pixmap;
		// direct = False; // on NVIDIA doest not work
	} else {
		d->_glxDrawable = window->d->_x11_drawable;
	}

	{
		// TODO: assert  gui is locked
		d->_glxCtx = ew_glXCreateContext(window->d->getX11Display(),
						 window->d->_x11_visualInfo,
						 None,   /* GLXContext shareList */
						 direct /* direct acces to device */);
	}

	if (! d->_glxCtx) {
		throw ew::core::exceptions::object_creation_error();
	}
}

// _____________________________________________________________

rendering_context::~rendering_context()
{
	{
		mutex_locker lock(d);

		// TODO: assert  gui is locked
		ew_glXDestroyContext(d->window->d->getX11Display(),
				     d->_glxCtx);
	}
	delete d;
}

// ______________________________________________________________

bool rendering_context::lock()
{
	d->lock();
	assert(d->window != 0);
	assert(d->_th_owner == 0);

	if (d->window->d->_offscreen_pixmap) {
		d->_glxDrawable = d->window->d->_offscreen_pixmap->d->_glx_pixmap;
	} else {
		d->_glxDrawable = d->window->d->_x11_drawable;
	}

	ew::core::time::sleep(1);
	while (ew_glXMakeCurrent(d->window->d->_x11_dpy,
				 d->_glxDrawable,
				 d->_glxCtx) != True) {
		ew::core::time::sleep(1);
	}

	d->_th_owner = ew::core::threading::self();

	return true;
}

// ________________________________________________________________

bool rendering_context::unlock()
{
	assert(d->window != 0);

	assert(d->_th_owner == ew::core::threading::self());

	// commented : because on nvidia cause flickering
	// ew_glXMakeCurrent(d->window->d->_x11_dpy, None, NULL);
	// assert(ret == True);

	d->_th_owner = 0;
	d->unlock();
	return true;
}

// ________________________________________________________________
// remove this ?
bool rendering_context::trylock()
{
	assert(0);

	if (d->window == 0) {
		return false;
	}

	if (d->_th_owner == ew::core::threading::self()) {
		d->_nrLock++;
		return true;
	}

	if (d->trylock() == false)
		return false;

	//  ObjectLocker guilock(d->window->d->_guiDpy);
	int ret = ew_glXMakeCurrent(d->window->d->_x11_dpy,
				    d->_glxDrawable,
				    d->_glxCtx);
	if (ret == True) {
		d->_th_owner = ew::core::threading::self();
		d->_nrLock++;
		return true;
	}

	d->_nrLock = 0;
	d->_th_owner = 0;
	d->unlock();
	return false;
}

bool rendering_context::attach(ew::graphics::gui::window * window)
{
	d->window = window;
	lock();
	return true;
}

bool rendering_context::detach()
{
	d->_glxDrawable = None;
	d->window = 0;
	unlock();
	return true;
}

ew::graphics::gui::window * rendering_context::isAttachedTo()
{
	return d->window;
}

}
}
}
