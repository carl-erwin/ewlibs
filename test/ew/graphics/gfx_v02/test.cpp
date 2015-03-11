#include <stdlib.h>

#include <cstdlib>
#include <cstdio>

#include <vector>

// ---------------------

#include <ew/system/system.hpp>

#include <ew/core/object/object_locker.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/mutex_locker.hpp>
#include <ew/core/threading/thread.hpp>
#include <ew/core/threading/condition_variable.hpp>
#include <ew/core/time/time.hpp>
#include <ew/maths/maths.hpp>


#include <ew/graphics/graphics.hpp>
#include <ew/graphics/gui/widget/window/window.hpp>
#include <ew/graphics/gui/event/event.hpp>
#include <ew/graphics/gui/event/push.hpp>


#include <ew/console/console.hpp>

#include <ew/core/application/simple_application.hpp>

#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"

namespace ew
{
namespace test
{

using namespace ew::core;
using namespace ew::core::types;
using namespace ew::core::objects;
using namespace ew::core::threading;
using namespace ew::graphics::gui;
using namespace ew::graphics::gui::events;

using ew::console::cerr;


using namespace ew::implementation::graphics::rendering::opengl;

extern TLS_DECL GLfloat angle;

ew::graphics::gui::display * guiDpy = 0;

extern GLfloat view_rotx;
extern GLfloat view_roty;
extern GLfloat view_rotz;



extern void gears_init(void);
extern void draw(void);

mutex nrRuningthreads_mtx;
condition_variable nrRuningthreads_cond(&nrRuningthreads_mtx);

u32 nrRuningthreads = 0; // will inc/dec by threads
u32 nrthreads = 0; // will be filled by args

// will be masked
#define APP_WIDTH 640
#define APP_HEIGHT 480

u32 app_width = APP_WIDTH;
u32 app_height = APP_HEIGHT;
s32 app_x = 0;
s32 app_y = 0;



TLS_DECL double angle_inc_per_second = 44.0;
TLS_DECL u32 rot0 = 0;
TLS_DECL u32 rot1 = 0;
TLS_DECL u32 target_fps = 60;


class myWindow : public ew::graphics::gui::window
{
public:
	bool loop;
	u32 fps;
	u32 sleep_frame;
	u32 target_fps;

public:
	myWindow(ew::graphics::gui::display * dpy, window_properties & properties)
		:
		window(dpy, (window *) 0, properties)
	{
		cerr << "class myWindow::myWindow(......) ok" << "\n";
		loop = true;
		fps = 0;
		target_fps = 60;
		sleep_frame = 1000 / target_fps;
	}

	virtual bool do_resize(u32 width, u32 height)
	{
		GLfloat h = (GLfloat) height / (GLfloat) width;

		glViewport(0, 0, (GLint) width, (GLint) height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -40.0);
		return true;
	}

	virtual bool on_resize(const widget_event * ev)
	{
		return do_resize(ev->width, ev->height);
	}

	virtual bool on_key_press(const keyboard_event * ev)
	{
		u32 key = ev->key;

		// cerr << "key =" << key << "\n";
		switch (key) {
		case keys::Escape:
			loop = false;
			break;
		case 86:
			angle_inc_per_second += 2.0 ;
			break;
		case 82:
			angle_inc_per_second -= 2.0 ;
			break;

		case 100: /* left */
			view_roty += 4.0f;
			break ;

		case 102: /* right */
			view_roty -= 4.0f;
			break ;

		case 98: /* up */
			view_rotz -= 4.0f;
			break ;

		case 104: /* down */
			view_rotz += 4.0f;
			break ;
		}

		return true;
	}

	virtual bool on_close(const widget_event * ev)
	{
		cerr << "myWindow::on_close(void)" << "\n";

		loop = false;

		return true;
	}

	virtual bool on_create(const widget_event * ev)
	{
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gears_init();

		// TODO: push on_resize event after on create in window:: ctor
		do_resize(width(), height());
		show();
		return true;
	}

	virtual bool on_draw(const widget_event * ev)
	{

		if (!loop)
			return true;


#ifdef __linux__ // not ok yet on FreeBSD
		double r = (random() % 256) * 0.001;
		double g = (random() % 256) * 0.001;
		double b = (random() % 256) * 0.001;

		glClearColor(r, g, b, 1.0);
#endif

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw();

		rot1 = ew::core::time::get_ticks();
		u32 diff = rot1 - rot0;
		static const u32 min_time = 1000 / (target_fps);
		if (diff > min_time) {
			rot0 = ew::core::time::get_ticks();

			angle += angle_inc_per_second * ((long double) diff / 1000.0);
			while (angle >= 360.0)
				angle -= 360.0;
		}

		if (sleep_frame > 0)
			ew::core::time::sleep(sleep_frame);

		push_draw_event(this);
		fps++;
		return true;
	}

};

// move this to x11Window code
void renderthreadFunc(myWindow * win)
{
	push_draw_event(win);
	while (win->loop == true) {
		ew::core::time::sleep(1000);
		cerr << "fps = " << win->fps << "\n";

		if (win->fps >= win->target_fps) {
			if (win->sleep_frame)
				win->sleep_frame++;

		} else if (win->fps < win->target_fps) {

			if (win->sleep_frame > 0) {
				win->sleep_frame--;
				if (win->sleep_frame == 0) {
					cerr << "sleep frame == 0\n";
				}
			}
		}
		win->fps = 0;
	}
}

void windowthread()
{
	window_properties properties;

	properties.x = app_x;
	properties.y = app_y;
	properties.width = app_width;
	properties.height = app_height;
	properties.is_resizeable = true;
	properties.is_fullscreen = false;
	properties.have_decoration = true;

	guiDpy->lock();
	myWindow * win = new myWindow(guiDpy, properties);
	guiDpy->unlock();

	renderthreadFunc(win);

	guiDpy->lock();
	delete win;
	guiDpy->unlock();

	mutex_locker lock(nrRuningthreads_mtx);
	--nrRuningthreads;

	if (nrRuningthreads == 0)
		nrRuningthreads_cond.signal();

	cerr << "void  windowthread() :: done" << "\n";
}

int main(int ac, char ** av)
{
	cerr << "ac = " << ac << "\n";

	if (ac == 2 || ac == 4) {
		if (ac == 4) {
			app_width = ew::maths::min(APP_WIDTH, ::atoi(av[ 2 ]));
			app_height = ew::maths::min(APP_HEIGHT, ::atoi(av[ 3 ]));
		}
	} else {
		cerr << "usage : " << av[ 0 ] << " nr_window [ width height ]" << "\n";
		ew::system::exit(1);
	}

	if (ew::core::time::init() == false) {
		cerr << "ew::core::time::init() :: error" << "\n";
		ew::system::exit(1);
	}

	if (ew::graphics::gui::init() == false) {
		cerr << "ew::graphics::gui::init() :: error" << "\n";
		ew::system::exit(1);
	}

	if (ew::graphics::rendering::init() == false) {
		cerr << "ew::graphics::gui::init() :: error" << "\n";
		ew::system::exit(1);
	}

	cerr << " app_width   = " << app_width << "\n";
	cerr << " app_height  = " << app_height << "\n";

	guiDpy = new ew::graphics::gui::display();
	if (guiDpy->open() == false) {
		return 1;
	}

	{
		nrthreads = atoi(av[ 1 ]);
		nrRuningthreads = nrthreads;
		if (nrthreads) {
			thread ** windowthreadsVec = new thread * [ nrthreads ];

			for (u32 count = 0; count < nrthreads; ++count) {
				char name[32];
#ifdef WIN32
				::_snprintf(name, 32, "windowthread no %d", count);
#else
				::snprintf(name, 32, "windowthread no %d", count);
#endif
				windowthreadsVec[ count ] = new thread((thread::func_t) windowthread , 0, name);
				windowthreadsVec[ count ] ->start();
			}

			cerr << "wait for threads\n";
			mutex_locker lock(&nrRuningthreads_mtx);
			nrRuningthreads_cond.wait();

			cerr << "no more thread running\n";

			for (u32 count = 0; count < nrthreads; ++count) {
				windowthreadsVec[ count ]->join();
				guiDpy->lock();
				delete windowthreadsVec[ count ];
				guiDpy->unlock();
			}
			guiDpy->lock();
			delete [] windowthreadsVec;
			guiDpy->unlock();
		}
	}

	delete guiDpy;

	bool error = false;
	if (ew::graphics::rendering::quit() == false) {
		cerr << "ew::graphics::gui::quit() :: error" << "\n";
		error = true;
	}


	if (ew::graphics::gui::quit() == false) {
		cerr << "ew::graphics::gui::quit() :: error" << "\n";
		error = true;
	}

	if (ew::core::time::quit() == false) {
		cerr << "ew::core::time::quit() :: error" << "\n";
		error = true;
	}

	return (error == true) ? 0 : 1;
}


}
}


int main(int ac, char ** av)
{
	return ew::test::main(ac, av);
}
