#include <iostream>

#include "ew/graphics/gui/event/event.hpp"
#include <ew/core/time/time.hpp>

#include "mywindow.hpp"
#include "mymenu.hpp"

namespace test
{
namespace graphics
{

main_window::main_window(ew::graphics::gui::display * dpy, window_properties & properties)
	:
	window(dpy, 0, properties),
	_menu0(0)
{
	loop = true;

	std::cerr << "class myWindow::myWindow(......) ok" << "\n";
}

main_window::~main_window()
{
	std::cerr << "class myWindow::~myWindow()" << "\n";
	if (_menu0 != 0) {
		std::cerr << "  DELETE MENU" << "\n";
		delete _menu0;
		_menu0 = 0;
	}
}


bool main_window::on_key_press(const keyboard_event * ev)
{
	if (ev->key == 9)
		loop = false;
	return true;
}

bool main_window::on_mouse_button_press(const button_event * ev)
{
	std::cerr << "button pressed" << ev->button << "\n";

	widget * parent = static_cast<widget *>(this);
	s32 x_pos = this->xPos() + this->width() - 50;
	s32 y_pos = this->yPos();


	if (ev->button == 1) {
		if (_menu0 == 0) {

			std::cerr << "x pos " << x_pos << "\n";
			std::cerr << "y pos " << y_pos << "\n";

			window_properties menuProp;

			menuProp.x = x_pos;
			menuProp.y = y_pos;
			menuProp.width = 100;
			menuProp.height = 100;
			menuProp.is_resizeable = false;
			menuProp.is_fullscreen = false;
			menuProp.have_decoration = false;

			// _menu0 = new myMenu(static_cast<widget *>(this), width() - 20, 10, 75, 100, true, false);
			_menu0 = new myMenu(this->display(),
					    parent,
					    menuProp);
			// _menu0 = new myMenu(this, -50, 20, 75, 100, true, false, true);
			_menu0->show();
			return true;
		} else {
			_menu0->move(x_pos, y_pos);
			_menu0->show();
			return true;
		}
	}

	if (_menu0) {
		_menu0->hide();
	}

	return true;
}

bool main_window::on_mouse_button_release(const button_event * ev)
{
	std::cerr << "button released" << ev->button << "\n";

	if (ev->button == 3 && _menu0 != 0) {
		_menu0->hide();
	}

	return true;
}

bool main_window::on_resize(const widget_event * ev)
{
	std::cerr << "myWindow::on_resize(..) ok" << "\n";

	ew_glMatrixMode(GL_PROJECTION);
	ew_glLoadIdentity();
	ew_gluPerspective(45.0,
			  (GLdouble)((GLdouble) ev->width / (GLdouble) ev->height),
			  (GLdouble) 0.1,
			  (GLdouble) 1000.0);

	ew_glViewport(0, 0, ev->width, ev->height);
	ew_glMatrixMode(GL_MODELVIEW);


	return true;
}

bool main_window::on_close(const widget_event * ev)
{
	std::cerr << "myWindow::on_close(void)" << "\n";
	loop = false;
	return true;
}

} // ! namespace graphics
} // ! namespace test
