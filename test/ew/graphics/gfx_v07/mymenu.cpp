#include <iostream>


#include "mywindow.hpp"
#include "mymenu.hpp"


#include "ew/graphics/gui/event/event.hpp"

#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"

#include <ew/core/time/time.hpp>

using namespace ew::graphics::gui;
using namespace ew::implementation::graphics::rendering::opengl;

namespace test
{
namespace graphics
{


extern ew::graphics::gui::display * dpy;


void do_anim_func(myMenu * menu)
{
	std::cerr << "DO ANIM STARTED\n";
	double c = 0.0;
	int dir = 1;
	double inc = 0.01;
	while (menu->do_anim) {
		dpy->lock();
		menu->lockDrawingContext();
		{
			ew_glClearColor(c, c, c, 1.0);

			ew_glClear(GL_COLOR_BUFFER_BIT);
		}
		menu->swapBuffers();
		menu->unlockDrawingContext();
		dpy->unlock();

		if (dir == 1)
			c += inc;
		if (dir == -1)
			c -= inc;

		if (c >= 1.0)
			dir = -1;
		if (c <= 0.0)
			dir = 1;

		ew::core::time::sleep(16);
	}

	std::cerr << "DO ANIM EXIT\n";
}

myMenu::myMenu(window * parent, window_properties & properties)
	:
	window(dpy, parent, properties)
{
	std::cerr << "class myMenu::myMenu(......) ok" << "\n";
	do_anim = true;
	if (do_anim == true) {
		anim_th = new thread((thread::func_t)do_anim_func, (thread::arg_t)this, "anim_th");
		anim_th->start();
	} else
		anim_th = 0;
}

myMenu::~myMenu()
{
	std::cerr << "class myMenu::~myMenu()" << "\n";
	do_anim = false;
	if (anim_th) {
		anim_th->join();
		delete anim_th;
		anim_th = 0;
	}
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
	return true;
}

bool myMenu::hide()
{
	window::hide();

//  on_draw();
	return true;
}


bool myMenu::on_key_press(const keyboard_event * ev)
{
	std::cerr << "myMenu :: key pressed\n";

	hide();

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
	std::cerr << "myMenu :: x = " << ev->x << "\n";
	std::cerr << "myMenu :: y = " << ev->y << "\n";

//   hide();
	return true;
}

bool myMenu::on_mouse_button_release(const button_event * ev)
{
	std::cerr << "myMenu :: button released\n";

	return true;
}

bool myMenu::on_close(const widget_event * ev)
{
	std::cerr << "mymenu::on_close(void)" << "\n";
	hide();
	return true;
}


bool myMenu::on_move(const widget_event * ev)
{
	std::cerr << "mymenu::on_move";
	std::cerr << "x pos " << ev->x << "\n";
	std::cerr << "y pos " << ev->y << "\n";
	return true;
}

}
}
