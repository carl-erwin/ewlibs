#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <ew/ew_config.hpp>     // holds system specific includes and #define
#include <ew/core/time/time.hpp>
#include <ew/maths/maths.hpp>
#include <ew/core/threading/thread.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/mutex_locker.hpp>
#include <ew/core/threading/condition_variable.hpp>
#include <ew/core/application/simple_application.hpp>
#include <ew/maths/maths.hpp>
#include <ew/graphics/graphics.hpp>
#include <ew/graphics/gui/event/event.hpp>
#include <ew/graphics/gui/event/push.hpp>
#include <ew/console/console.hpp>
using ew::console::cerr;

#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU_wrappers.hpp"



namespace ew
{
namespace test
{

using namespace ew::core;
using namespace ew::core::types;
using namespace ew::core::threading;
using namespace ew::graphics::gui;
using namespace ew::implementation::graphics::rendering::opengl;

mutex nrRuningthreads_mtx;
condition_variable nrRuningthreads_cond(&nrRuningthreads_mtx);

u32 nrRuningthreads = 0; // will inc/dec by threads
u32 nrthreads = 0; // will be filled by args

// will be masked
#define APP_WIDTH 250
#define APP_HEIGHT 250

u32 app_width = APP_WIDTH;
u32 app_height = APP_HEIGHT;
s32 app_x = 0;
s32 app_y = 0;


ew::graphics::gui::display * guiDpy = 0;

class myWindow : public ew::graphics::gui::window
{
public:
	bool loop;

	double rot_x_angle;
	double rot_y_angle;
	double rot_z_angle;
	double x_angle_inc_per_second;
	double y_angle_inc_per_second;
	double z_angle_inc_per_second;

	u32 rot0;
	u32 rot1;
	u32 fps;
	u32 target_fps;
	u32 sleep_frame;

public:
	myWindow(ew::graphics::gui::display * dpy, window_properties & properties)
		:
		window(dpy, (window *)0, properties)
	{
		loop = true;

		rot_x_angle = 0.0;
		rot_y_angle = 0.0;
		rot_z_angle = 0.0;

		x_angle_inc_per_second = 20.0;
		y_angle_inc_per_second = 40.0;
		z_angle_inc_per_second = 120.0;

		rot0 = 0;
		rot1 = 0;
		fps = 0;
		target_fps = 60;
		sleep_frame = 1000 / target_fps;
	}

	virtual bool do_resize(u32 w, u32 h)
	{
		//     clear the window
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0,
			       (GLdouble)((GLdouble) w / (GLdouble) h),
			       (GLdouble) 0.1,
			       (GLdouble) 1000.0);

		glViewport(0, 0, w, h);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		return true;
	}

	virtual bool on_resize(const widget_event * ev)
	{
		return do_resize(ev->width, ev->height);
	}

	virtual bool on_close(const widget_event * ev)
	{
		loop = false;
		return true;
	}

	virtual bool on_key_press(const keyboard_event * ev)
	{

		if (ev->key == 9)
			loop = false;
		return true;
	}

	virtual bool on_create(const widget_event * ev)
	{
		// glClearColor(c, c, c, c);
		glClearColor(0.0, 0.0, 0.0, 0.0);
		// glClearColor(105.0/255.0, 140.0/255.0, 98.0/255.0, 1.0);
		// glEnable( GL_CULL_FACE );
		glEnable(GL_DEPTH_TEST);
		resize(ev->width, ev->height);
		show();
		return true;
	}

	virtual bool on_draw(const widget_event * ev)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// draw axis
		glPushMatrix();
		{
			glTranslatef(-2.0f , -2.0f, -10.0f);
			glColor3ub(0xff, 0xff, 0xff);

			f32 axisLenght = 10.0f;
			glBegin(GL_LINES);
			{
				glColor4ub(0xff, 0x00, 0x00, 0xff);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(axisLenght, 0.0f, 0.0f);

				glColor4ub(0x00, 0xff, 0x00, 0xff);
				glVertex3f(0.0, 0.0, 0.0);
				glVertex3f(0.0, axisLenght, 0.0);

				glColor4ub(0x00, 0x00, 0xff, 0xff);
				glVertex3f(0.0, 0.0, 0.0);
				glVertex3f(0.0, 0.0, axisLenght);
			}
			glEnd();
		}
		glPopMatrix();

		// draw a colored cube
		glPushMatrix();
		{
			glScalef(0.75, 0.75, 0.75);

			glTranslatef(-2.0f , -2.0f, -10.0f);
			glRotated(rot_x_angle, 1, 0, 0);
			glRotated(rot_y_angle, 0, 1, 0);
			glRotated(rot_z_angle, 0, 0, 1);

			glColor3f(1, 0, 0);
			glBegin(GL_POLYGON);
			glVertex3f(1, -1, -1);
			glVertex3f(1,  1, -1);
			glVertex3f(1,  1,  1);
			glVertex3f(1, -1,  1);
			glEnd();

			glColor3f(0, 1, 1);
			glBegin(GL_POLYGON);
			glVertex3f(-1, -1, -1);
			glVertex3f(-1,  1, -1);
			glVertex3f(-1,  1,  1);
			glVertex3f(-1, -1,  1);
			glEnd();

			glColor3f(0, 1, 0);
			glBegin(GL_POLYGON);
			glVertex3f(-1, 1, -1);
			glVertex3f(1, 1, -1);
			glVertex3f(1, 1,  1);
			glVertex3f(-1, 1,  1);
			glEnd();

			glColor3f(1, 0, 1);
			glBegin(GL_POLYGON);
			glVertex3f(-1, -1, -1);
			glVertex3f(1, -1, -1);
			glVertex3f(1, -1,  1);
			glVertex3f(-1, -1,  1);
			glEnd();

			glColor3f(0, 0, 1);
			glBegin(GL_POLYGON);
			glVertex3f(-1, -1, 1);
			glVertex3f(1, -1, 1);
			glVertex3f(1,  1, 1);
			glVertex3f(-1,  1, 1);
			glEnd();

			glColor3f(1, 1, 0);
			glBegin(GL_POLYGON);
			glVertex3f(-1, -1, -1);
			glVertex3f(1, -1, -1);
			glVertex3f(1,  1, -1);
			glVertex3f(-1,  1, -1);
			glEnd();
		}
		glPopMatrix();

		++fps;

		rot1 = ew::core::time::get_milliseconds_since_startup();
		u32 diff = rot1 - rot0;
		if (diff >= 1000 / target_fps) {
			while (rot_x_angle >= 360.0) rot_x_angle -= 360.0;
			while (rot_y_angle >= 360.0) rot_y_angle -= 360.0;
			while (rot_y_angle >= 360.0) rot_y_angle -= 360.0;


			rot_x_angle += x_angle_inc_per_second * ((long double)diff / 1000.0);
			rot_y_angle += y_angle_inc_per_second * ((long double)diff / 1000.0);
			rot_z_angle += z_angle_inc_per_second * ((long double)diff / 1000.0);

			rot0 = ew::core::time::get_milliseconds_since_startup();
		}

		if (sleep_frame > 0)
			ew::core::time::sleep(sleep_frame);

		// if (loop == true)
		{
			push_draw_event(this);
		}
		return true;
	}
};

// move this to x11Window code


void renderthreadFunc(myWindow * win)
{
	cerr << " renderthreadFunc( myWindow * win ) \n";
	push_draw_event(win);

	// win->idle()
	while (win->loop == true) {
		// win->update_fps();
		if (win->fps > win->target_fps) {
			if (win->sleep_frame)
				win->sleep_frame++;
		} else if (win->fps < win->target_fps) {

			if (win->sleep_frame > 0) {
				win->sleep_frame--;
				if (win->sleep_frame == 0) {
					// cerr << "sleep frame == 0\n";
				}
			}
		}

		win->fps = 0;
	}
}

void        windowthread()
{
	myWindow * win = 0;
	window_properties properties;

	properties.x = app_x;
	properties.y = app_y;
	properties.width = app_width;
	properties.height = app_height;
	properties.is_resizeable = true;
	properties.is_fullscreen = false;
	properties.have_decoration = true;

	guiDpy->lock();
	win = new myWindow(guiDpy, properties);
	guiDpy->unlock();

	renderthreadFunc(win);

	guiDpy->lock();
	delete win;
	guiDpy->unlock();

	mutex_locker lock(&nrRuningthreads_mtx);
	--nrRuningthreads;
	if (!nrRuningthreads)
		nrRuningthreads_cond.signal();
}

int        main(int ac, char ** av)
{
	cerr << "ac = " << ac << "\n";

	if (ac == 2 || ac == 4) {
		if (ac == 4) {
			app_width  = ew::maths::min(APP_WIDTH * 2, atoi(av[2]));
			app_height = ew::maths::min(APP_HEIGHT * 2, atoi(av[3]));
		}
	} else {
		cerr <<  "usage : " << av[0] << " nr_window [ width height ]" << "\n";
		exit(1);
	}

	ew::core::time::init();

	if (ew::graphics::gui::init() == false) {
		cerr <<  "ew::graphics::gui::init() :: error" << "\n";
		exit(1);
	}

	if (ew::graphics::rendering::init()  == false) {
		cerr <<  "ew::graphics::rendering::init() :: error" << "\n";
		exit(1);
	}

	// alloc display before creating any widget
	guiDpy = new ew::graphics::gui::display(); // getGuiInstance()->allocDisplay();
	if (guiDpy->open() == false) {
		return 1;
	}

	cerr << " app_width   = " << app_width  << "\n";
	cerr << " app_height  = " << app_height << "\n";

	nrthreads = atoi(av[1]);
	nrRuningthreads = nrthreads;
	if (nrthreads) {
		thread ** windowthreadsVec = new thread * [nrthreads];

		for (u32 count = 0; count < nrthreads; ++count)
			windowthreadsVec[count] = new thread((thread::func_t)windowthread, 0, 0);
		for (u32 count = 0; count < nrthreads; ++count) {
			windowthreadsVec[count]->start();
		}

		// we should have an app quit on last window ??
		mutex_locker lock(&nrRuningthreads_mtx);
		nrRuningthreads_cond.wait();
	}

	delete guiDpy;

	if (ew::graphics::gui::quit() == false)
		cerr <<  "ew::graphics::gui::quit() :: error" << "\n";

	ew::core::time::quit();

	return 0;
}

}
}

int main(int ac, char ** av)
{
	return ew::test::main(ac, av);
}
