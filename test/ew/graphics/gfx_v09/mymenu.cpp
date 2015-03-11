#include <iostream>

#include <ew/core/time/time.hpp>

#include "mywindow.hpp"
#include "mymenu.hpp"


namespace test
{
namespace graphics
{


void myMenu::renderFunc(myMenu * menu)
{
	using namespace ew::core::types;

	double c = 1.0;
	s32 dir = 1;
	double inc = 0.001;

	while (true) {
		if (menu->isActive == false) {
			ew::core::time::sleep(10);
			continue ;
		}

		menu->lock();
		menu->lockDrawingContext();
		{
			ew_glClearColor(c, c, c, c);
			ew_glClear(GL_COLOR_BUFFER_BIT);
			menu->swapBuffers();
		}
		menu->unlockDrawingContext();
		menu->unlock();

		if (dir == 1)
			c += inc;
		if (dir == -1)
			c -= inc;
		if (c >= 1.0)
			dir = -1;
		if (c <= 0.0)
			dir = 1;

		ew::core::time::sleep(1);
	}

	std::cerr << "[LEAVE] void myMenu::renderFunc(myMenu * menu)\n";
}

myMenu::myMenu(ew::graphics::gui::display * dpy, widget * parent, window_properties & properties)
	:
	window(dpy, static_cast<window *>(parent), properties)
{
	std::cerr << "class myMenu::myMenu(......) ok" << "\n";
	renderthread = new thread((thread::func_t) myMenu::renderFunc,
				  (thread::arg_t) this, "myMenu::Renderthread");
}

myMenu::~myMenu()
{
	std::cerr << "class myMenu::~myMenu()" << "\n";
	delete renderthread;
	renderthread = 0;
}

bool myMenu::on_draw(const widget_event * ev)
{
	ew_glClearColor(0.0, .5, 0.0, 1.0);
	ew_glClear(GL_COLOR_BUFFER_BIT);

	return true;
}

bool myMenu::show()
{

	window::show();


	on_draw(nullptr);
	isActive = true;
	renderthread->start();

	//isActive = false;

	return true;
}


bool myMenu::hide()
{
	window::hide();
	return true;
}


bool myMenu::on_key_press(const keyboard_event * ev)
{
	std::cerr << "myMenu :: key pressed\n";

	this->hide();

	return true;
}

bool myMenu::on_key_release(const keyboard_event * ev)
{
	std::cerr << "myMenu :: key released\n";

	return true;
}

bool myMenu::on_mouse_button_press(const button_event * ev)
{
	std::cerr << "myMenu :: button pressed\n";

	return true;
}

bool myMenu::on_mouse_button_release(const button_event * ev)
{
	std::cerr << "button released\n";

	return true;
}

bool myMenu::on_close(const widget_event * ev)
{
	std::cerr << "mymenu::on_close(void)" << "\n";
	isActive = false;
	return true;
}

} // ! namespace graphics
} // ! namespace test
