#include <windows.h>

#include <ew/ew_config.hpp>
#include <ew/graphics/gui/widget/window/window.hpp>

#include <iostream>
#include <list>

#include <ew/maths/maths.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/thread.hpp>

#include <ew/graphics/renderer/rendering_context.hpp>

#include "window_private_data.hpp"


//---------------------------------------------------------------------------

namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::types;
using namespace ew::core::threading;

// global static
std::list<Widget *> widget_list;
typedef std::list<Widget *>::iterator widget_listIt;


// --------------------------------------------------------
// improve this
void widget_eventHandler(void)
{
	MSG msg;

	while (true) {
		std::cerr << "void widget_eventHandler( void )\n";

		ZeroMemory(&msg, sizeof(msg));
		switch (GetMessage(&msg, NULL, 0, 0)) {
		case -1: {
			// handle the error and possibly exit
			std::cerr << "void widget_eventHandler( void ) :: GetMessage :: ERROR \n";
		}
		break ;

		case 0: {
			// handle the error and possibly exit
			std::cerr << "void widget_eventHandler( void ) :: GetMessage == 0 : WM_QUIT\n";
			return ;
		}
		break ;

		default: {
			std::cerr << "void widget_eventHandler( void ) :: default :: msg.message = " << msg.message << "\n";
			std::cerr << "void widget_eventHandler( void ) :: TranslateMessage( &msg )\n";
			TranslateMessage(&msg);
			std::cerr << "void widget_eventHandler( void ) :: DispatchMessage( &msg )\n";
			DispatchMessage(&msg);
		}
		}
	} // ! while

}

bool Window::show()
{
	this->lock();
	{
		d->_event_thread->start();
		// sleep ?
		//d->_event_threadMutex->lock();
		while (d->_showOk == false) {
			//std::cerr << "wait for window creation...\n";
			Sleep(150);
		}

		// d->_event_threadMutex->unlock();
	}
	this->unlock();
	return true;
}


bool Window::hide()
{
	return true;
}

// will be used by others ctors
Window::Window(Display * dpy, Window * parent, window_properties & properties)
	:
	d(new private_data())
{
	d->_properties = properties;
	d->_event_thread = new thread((func_t)Window::private_data::widgetthread, (Arg_t) this, "");
}

Window::~Window()
{
	delete d;
}

bool Window::lockDrawingContext()
{
	return d->rendering_context->lock();
}

bool Window::unlockDrawingContext()
{
	return d->rendering_context->unlock();
}

bool Window::swapBuffers()
{
	BOOL ret = SwapBuffers(this->d->hDC);
	return (ret == TRUE) ? true : false;
}

u32 Window::width()
{
	return this->d->_properties.width;
}

u32 Window::height()
{
	return this->d->_properties.height;
}

bool Window::isAvailable(void)
{
	return false;
}

bool Window::on_resize(u32 width, u32 height)
{
	return false;
}

bool Window::on_key_press(u32 key)
{
	return false;
}

bool Window::on_key_release(u32 key)
{
	return false;
}

bool Window::on_close(void)
{
	return false;
}

//
const char * Window::class_name() const
{
	return 0;
}

const char * Window::getName() const
{
	return 0;
}

Display * Window::display() const
{
	return 0;
}

bool Window::lock()
{
	return true;
}

bool Window::trylock()
{
	return true;
}

bool Window::unlock()
{
	return true;
}

bool Window::trylockDrawingContext()
{
	return true;
}

//
s32 Window::xPos()
{
	return d->_properties.x;
}

s32 Window::yPos()
{
	return d->_properties.y;
}

// actions
bool Window::move(s32 x, s32 y)
{
	return true;
}

bool Window::resize(u32 width, u32 height)
{
	return true;
}

// events
bool Window::on_draw()
{
	return true;
}

bool Window::on_move(s32 x, s32 y)
{
	return true;
}


bool Window::on_mouse_button_press(u32 button, s32 x, s32 y)
{
	return true;
}

bool Window::on_mouse_button_release(u32 button, s32 x, s32 y)
{
	return true;
}

bool Window::on_pointer_motion(s32 x, s32 y)
{
	return true;
}

ew::graphics::rendering::rendering_context * Window::renderingContext() const
{
	return d->rendering_context;
}

ew::graphics::rendering::renderer * Window::renderer() const
{
	return 0;
}

// position
s32 xPos()
{
	return 0;
}
s32 yPos()
{
	return 0;
}
bool Window::setXPos(s32 x)
{
	return true;
}
bool Window::setYPos(s32 y)
{
	return true;
}

// size

bool Window::setWidth(u32 w)
{
	return true;
}
bool Window::setHeight(u32 h)
{
	return true;
}

// events callbacks
bool Window::on_pointer_enter()
{
	return true;
}
bool Window::on_pointer_leave()
{
	return true;
}

bool Window::on_focus_in()
{
	return true;
}
bool Window::on_focus_out()
{
	return true;
}



}
}
}
