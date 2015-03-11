#include <iostream>
#include <vector>
#include <cstdlib>
// ---------------------------------


#include <ew/ew_config.hpp>      // holds system specific includes and #define


#include <ew/maths/maths.hpp>

#include <ew/system/system.hpp>

#include <ew/core/threading/thread.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/application/simple_application.hpp>
#include <ew/maths/maths.hpp>
#include <ew/graphics/graphics.hpp>
#include <ew/core/time/time.hpp>

#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"


using namespace ew::graphics::gui;
using namespace ew::implementation::graphics::rendering::opengl;

//
#include <ew/graphics/graphics.hpp>

//
#include <stdexcept>


// NEW
#include <ew/graphics/gui/widget/window/window.hpp>


#include "mywindow.hpp"


namespace test
{
namespace graphics
{

using namespace ew::core::types;

/* ----------------------------------------------- */
using namespace ew::core::threading;
using namespace ew::graphics::gui;


mutex nrRuningthreads_mtx;
u32 nrRuningthreads = 0; // will inc/dec by threads
u32 nrthreads = 0; // will be filled by args

// will be masked
s32 x = 0;
s32 y = 0;
u32 width = 250;
u32 height = 250;

#define MAX_WIDTH  1600
#define MAX_HEIGHT 1200

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
	std::cerr << "  renderthreadFunc( myWindow * win ) \n";

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

	////////////////////////////////////////////////////

	// vars
	double rot_angle = 0.0;
	double angle_inc_per_second = 10.0;

	//  u32 render_begin = 0;
	//u32 render_end = 0;
	u32 rot0 = 0;
	u32 rot1 = 0;
	//  u32 time_render_frame = 0;
	u32 target_fps = 60;

	while (win->loop == true) {
		// render_begin = ew::core::time::get_ticks();

		dpy->lock();
		win->lock();
		win->lockDrawingContext();
		{
		}
		// swap_buffers:
		win->swapBuffers();
		win->unlockDrawingContext();
		win->unlock();
		dpy->unlock();

		// render_end = ew::core::time::get_ticks();
		//    time_render_frame = render_end - render_begin;
		rot1 =  ew::core::time::get_ticks();
		u32 diff = rot1 - rot0;
		if (diff >= 1000 / target_fps) {
			rot_angle += angle_inc_per_second * ((long double) diff / 1000.0);
			if (rot_angle >= 360.0)
				rot_angle -= 360.0;
			rot0 =  ew::core::time::get_ticks();
		}

		ew::core::time::sleep(1);

	} // ! while ( isAvailable() == true )
}


void windowthread()
{
	main_window * win = 0;

	std::cerr << "void  windowthread()" << "\n";

	window_properties winProperties;

	winProperties.x = x;
	winProperties.y = y;
	winProperties.width = width;
	winProperties.height = height;
	winProperties.is_resizeable = true;
	winProperties.is_fullscreen = false;

	dpy->lock();
	win = new main_window(winProperties);
	win->show();
	dpy->unlock();

	thread * renderthread = new thread((thread::func_t) renderthreadFunc, (thread::arg_t) win, "renderthread");
	if (renderthread->start() != true) {
		delete renderthread;
		dpy->lock();
		delete win;
		dpy->unlock();
		return ;
	}
	renderthread->join();
	delete renderthread;

	dpy->lock();
	delete win; // destroy
	dpy->unlock();

	nrRuningthreads_mtx.lock();
	--nrRuningthreads;
	nrRuningthreads_mtx.unlock();

	std::cerr << "void  windowthread() :: done" << "\n";
}

int main(int ac, char ** av)
{
	if (!(ac == 2 || ac == 4)) {
		std::cerr << "usage : " << av[ 0 ] << " nr_window [ width height ]" << "\n";
		ew::system::exit(1);
	}

	nrthreads = atoi(av[ 1 ]);
	if (ac == 4) {
		width  = ew::maths::in_range<u32>(10, atoi(av[2]), MAX_WIDTH);
		height = ew::maths::in_range<u32>(10, atoi(av[3]), MAX_HEIGHT);
	}

	if (ew::core::time::init() == false) {
		std::cerr << "ew::core::time::init() :: error" << "\n";
		ew::system::exit(1);
	}


	if (ew::graphics::gui::init() == false) {
		std::cerr << "ew::graphics::gui::init() :: error" << "\n";
		ew::system::exit(1);
	}

	if (ew::graphics::rendering::init() == false) {
		std::cerr << "ew::graphics::gui::init() :: error" << "\n";
		ew::system::exit(1);
	}

	// alloc display before creating any widget
	dpy = new ew::graphics::gui::display();
	if (dpy->open() == false) {
		return 1;
	}

	if (false) {
		for (int i = 0; i < 20; ++i) {
			std::cerr << " --- test " << i << "\n";
			main_window * win = 0;

			window_properties winProperties;
			winProperties.x = x;
			winProperties.y = y;
			winProperties.width = width;
			winProperties.height = height;
			winProperties.is_resizeable = true;
			winProperties.is_fullscreen = false;

			dpy->lock();
			win = new main_window(winProperties);
			win->show();
			win->hide();
			delete win;
			dpy->unlock();
		}
	} else {
		nrRuningthreads = nrthreads;
		if (nrthreads) {
			thread ** windowthreadsVec = new thread * [ nrthreads ];

			for (u32 count = 0; count < nrthreads; ++count) {
				windowthreadsVec[ count ] = new thread((thread::func_t) windowthread, 0, "windowthread");
				windowthreadsVec[ count ] ->start();
			}

			// we should have an app quit on last window ??
			while (nrRuningthreads != 0)
				ew::core::time::sleep(100);

			for (u32 count = 0; count < nrthreads; ++count) {
				windowthreadsVec[ count ] ->join();
				dpy->lock();
				delete windowthreadsVec[ count ];
				dpy->unlock();
			}
			delete [] windowthreadsVec;
		}
	}

	std::cerr << "delete dpy..." << "\n";
	delete dpy;

	if (ew::graphics::gui::quit() == false) {
		std::cerr << "ew::graphics::gui::quit() :: error" << "\n";
		ew::system::exit(1);
	}

	return 0;
}

} // ! namespace graphics
} // ! namespace test


int main(int ac, char ** av)
{
	return test::graphics::main(ac, av);
}
