#include <iostream>

#include "ew/graphics/gui/event/event.hpp"

#include "mywindow.hpp"
#include "mymenu.hpp"


#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"


using namespace ew::graphics::gui;
using namespace ew::implementation::graphics::rendering::opengl;


namespace test
{
namespace graphics
{

extern display * dpy;

main_window::main_window(window_properties & properties)
	:
	window(dpy, 0, properties),
	_menu0(0)
{
	loop = true;

	std::cerr << "class myWindow::myWindow(......) ok" << "\n";
	_x = properties.x;
	_y = properties.y;
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

bool main_window::on_mouse_button_press(const button_event * ev)
{
	std::cerr << __FUNCTION__ << "button pressed" << ev->button << "\n";

	if (ev->button == 1) {
		if (_menu0 == 0) {
			s32 x_pos = this->xPos() + this->width() / 2;
			s32 y_pos = this->yPos() + this->height() / 2;

			std::cerr << "x pos " << x_pos << "\n";
			std::cerr << "y pos " << y_pos << "\n";

			window_properties prop;
			prop.x = x_pos;
			prop.y = y_pos;
			prop.width = 100;
			prop.height = 100;
			prop.is_resizeable = true;
			prop.is_fullscreen = false;
			prop.have_decoration = false;

			_menu0 = new myMenu(this, prop);
		}
		_menu0->show();
		return true;
	}

	if (_menu0) {
		_menu0->hide();
		//delete _menu0;
		//_menu0 = 0;
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

	if (_menu0) {
		s32 x_pos = this->xPos() + this->width() / 2;
		s32 y_pos = this->yPos() + this->height() / 2;
		_menu0->move(x_pos, y_pos);
	}

	return true;
}

bool main_window::on_close(const widget_event * ev)
{
	std::cerr << "myWindow::on_close(void)" << "\n";
	loop = false;
	return true;
}

bool main_window::on_key_press(const keyboard_event * ev)
{
	std::cerr << "myWindow::on_key_press(...)" << "\n";
	if (ev->key == 9)
		loop = false;
	return true;
}


bool main_window::on_move(const widget_event * ev)
{
	std::cerr << "myWindow::on_move(s32 x, s32 y)" << "\n";

	if (_menu0) {
		s32 x_pos = this->xPos() + this->width() / 2;
		s32 y_pos = this->yPos() + this->height() / 2;

		std::cerr << "x pos " << x_pos << "\n";
		std::cerr << "y pos " << y_pos << "\n";


		_menu0->move(x_pos, y_pos);
	}

	return true;
}


} // ! namespace graphics
} // ! namespace test


