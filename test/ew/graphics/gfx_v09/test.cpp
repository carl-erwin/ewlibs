#include <sys/time.h>

#include <cstdlib>

#include <iostream>
#include <vector>
#include <stdexcept>


// -----------------------------------

#include <ew/ew_config.hpp>      // holds system specific includes and #define
//
#include <ew/maths/maths.hpp>
#include <ew/core/threading/thread.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/mutex_locker.hpp>
#include <ew/core/threading/condition_variable.hpp>
#include <ew/core/time/time.hpp>
#include <ew/core/application/simple_application.hpp>
#include <ew/maths/maths.hpp>
#include <ew/graphics/graphics.hpp>
#include <ew/graphics/gui/event/event.hpp>
#include <ew/graphics/gui/widget/window/window.hpp>

#include <ew/core/time/time.hpp>

#include <ew/console/console.hpp>


#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"

#include "mywindow.hpp"



using namespace ew::core::types;
using namespace ew::core::threading;
using namespace ew::graphics::gui;
using namespace ew::graphics::gui::events;
using namespace ew::implementation::graphics::rendering::opengl;
using namespace ew::graphics::rendering;

namespace test
{
namespace graphics
{


using ew::console::cerr;


mutex nrRuningthreads_mtx;
condition_variable nrRuningthreads_cond(&nrRuningthreads_mtx);

u32 nrRuningthreads = 0; // will inc/dec by threads
u32 nrthreads = 0; // will be filled by args

// will be masked

// Will be masked
u32 WIDTH = 640;
u32 HEIGHT = 480;
s32 WIN_X = 0;
s32 WIN_Y = 0;

s32 x = 100;
s32 y = 100;
u32 width = 250;
u32 height = 250;

display * dpy = 0;

inline int next_p2(int a)
{
	int rval = 1;
	while (rval < a)
		rval <<= 1;
	return rval;
}



// move this to x11Window code

void renderthreadFunc(main_window * win)
{
	cerr << "  renderthreadFunc( myWindow * win ) \n";

	// setup gl
	dpy->lock();
	win->lock();
	win->lockDrawingContext();
	{
		ew_glEnable(GL_DEPTH_TEST);
		// ew_glEnable(GL_LINE_SMOOTH);
		//     ew_glMatrixMode(GL_TEXTURE);
		//     ew_glLoadIdentity();
		//     ew_glMatrixMode(GL_COLOR);
		//     ew_glLoadIdentity();
		//     ew_glDisable(GL_CULL_FACE);

		ew_glViewport(0, 0, win->width(), win->height());

		ew_glMatrixMode(GL_PROJECTION);
		ew_glLoadIdentity();

		GLdouble ratio;
		if (win->height()) {
			ratio = ((GLdouble) win->width()) / ((GLdouble) win->height());
		} else {
			ratio = 4.0 / 3.0;
		}

		ew_gluPerspective(45.0,
				  ratio,
				  (GLdouble) 0.001,
				  (GLdouble) 10000.0);

		ew_glMatrixMode(GL_MODELVIEW);
		ew_glLoadIdentity();

	}
	win->unlockDrawingContext();
	win->unlock();
	dpy->unlock();

	// vars
	double rot_angle = 0.0;
	double angle_inc_per_second = 10.0;

	struct timeval render_begin = {
		0, 0
	};
	struct timeval render_end = {
		0, 0
	};
	struct timeval rot0 = {
		0, 0
	};
	struct timeval rot1 = {
		0, 0
	};
	// u32 time_render_frame = 0;

	// infite loop
	u32 target_fps = 60;
	while (win->loop == true) {
		// release th cpu
		// sched_yield();
		::gettimeofday(&render_begin, 0);

		// win->lock();
		win->lockDrawingContext();
		win->unlockDrawingContext();
		// win->unlock();


		gettimeofday(&render_end, 0);
		// time_render_frame =  (render_end.tv_sec - render_begin.tv_sec) * 1000000 + (render_end.tv_usec - render_begin.tv_usec);
		// cerr << "time_render_frame = " << time_render_frame << "\n";

		gettimeofday(&rot1, 0);
		u32 diff = (rot1.tv_sec - rot0.tv_sec) * 1000 + (rot1.tv_usec - rot0.tv_usec) / 1000;
		if (diff >= 1000 / target_fps) {
			while (rot_angle >= 360.0)
				rot_angle -= 360.0;
			rot_angle += angle_inc_per_second * ((long double) diff / 1000.0);
			gettimeofday(&rot0, 0);
		}

		ew::core::time::usleep(1);
	}
}


void windowthread()
{
	main_window * win = 0;

	cerr << "void  windowthread()" << "\n";

	/* x11Window */
	// todo :
	window_properties winProperties;

	winProperties.x = WIN_X ;
	winProperties.y = WIN_Y ;
	winProperties.width = WIDTH;
	winProperties.height = HEIGHT;
	winProperties.is_resizeable = true;
	winProperties.is_fullscreen = false;
	winProperties.have_decoration = false;

	dpy->lock();
	win = new main_window(dpy, winProperties);
	win->show();
	dpy->unlock();

	bool useRenderthread = false; // true;
	if (useRenderthread == false) {
		renderthreadFunc(win);
	} else {

		thread * renderthread = new thread((thread::func_t) renderthreadFunc, (thread::arg_t) win,
						   "renderthread");
		if (renderthread->start() != true) {
			// thread_exit();
			// win->event_thread()->stop();
		}
		renderthread->join();
	}

	dpy->lock();
	delete win; // destroy
	dpy->unlock();

	nrRuningthreads_mtx.lock();
	--nrRuningthreads;
	if (!nrRuningthreads)
		nrRuningthreads_cond.signal();
	nrRuningthreads_mtx.unlock();

	cerr << "void  windowthread() :: done" << "\n";
}

int main(int ac, char ** av)
{
	if (ac == 2 || ac == 4) {
		if (ac == 4) {
			width = ew::maths::max(100, atoi(av[ 2 ]));
			height = ew::maths::max(100, atoi(av[ 3 ]));
		}
	} else {
		cerr << "usage : " << av[ 0 ] << " nr_window [ width height ]" << "\n";
		exit(1);
	}


	if (ew::core::time::init() == false) {
		cerr << "ew::core::time::init() :: error" << "\n";
		exit(1);
	}


	if (ew::graphics::gui::init() == false) {
		cerr << "ew::graphics::gui::init() :: error" << "\n";
		exit(1);
	}

	if (ew::graphics::rendering::init() == false) {
		cerr << "ew::graphics::rendering::init() :: error" << "\n";
		exit(1);
	}

	// alloc display before creating any widget
	dpy = new ew::graphics::gui::display();
	if (dpy->open() == false) {
		return 1;
	}

	nrthreads = atoi(av[ 1 ]);
	nrRuningthreads = nrthreads;
	if (nrthreads) {
		thread ** windowthreadsVec = new thread * [ nrthreads ];

		for (u32 count = 0; count < nrthreads; ++count)
			windowthreadsVec[ count ] = new thread((thread::func_t) windowthread,
							       (thread::arg_t) 0, "windowthread");
		for (u32 count = 0; count < nrthreads; ++count) {
			windowthreadsVec[ count ] ->start();
		}

		// we should have an app quit on last window ??
		nrRuningthreads_cond.wait();
	}

	cerr << "delete dpy..." << "\n";
	delete dpy;

	if (ew::graphics::gui::quit() == false) {
		cerr << "ew::graphics::gui::quit() :: error" << "\n";
		exit(1);
	}

	return 0;
}

} // ! namespace graphics
} // ! namespace test


int main(int ac, char ** av)
{
	return test::graphics::main(ac, av);
}
