#include <iostream>

#include <ew/graphics/renderer/rendering_context.hpp>

#include "rendering_context_private_data.hpp"

namespace ew
{
namespace graphics
{
namespace rendering
{

rendering_context::rendering_context(ew::graphics::gui::window * widget)
	: d(new private_data)
{
	std::cerr << "rendering_context::rendering_context()\n";

	this->d->widget = widget;


	if (!(d->hglRC = ::wglCreateContext(d->widget->d->hDC))) {
		MessageBox(NULL, "Can't Create An OpenGL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		ExitProcess(1);
	}
	std::cerr << "rendering_context::rendering_context() : OK\n";
}

rendering_context::~rendering_context()
{
	// TODO
	delete d;
}


bool rendering_context::lock()
{
	// if window

	if (::wglMakeCurrent(d->widget->d->hDC, d->hglRC) == FALSE) {
		//MessageBox( NULL, "Can't use the OpenGL Rendering Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION );
		//ExitProcess( 1 );
		return false;
	}

	return true;
}

bool rendering_context::unlock()
{
	// test if owned
	if (::wglMakeCurrent(NULL, NULL) == FALSE) {
		return false;
	}
	return true;
}

bool rendering_context::trylock()
{
	return false;
}

bool isLocked()
{
	return false;
}

bool rendering_context::isAvailable()
{
	return false;
}

bool rendering_context::attach(ew::graphics::gui::window * widget)
{
	d->widget = widget;
	return true;
}

bool rendering_context::detach()
{
	unlock();
	d->widget = 0;
	return true;
}

ew::graphics::gui::window * rendering_context::isAttachedTo()
{
	return d->widget;
}

}
}
}
