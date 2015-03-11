// -*- mode:c++; offset-width: 2; -*-
#include <iostream>
#include <vector>
#include <stdexcept>



//#include <GL/glut.h>

//
#include <sys/time.h>

#include "FreeType.h"

// ------------------------------


#include <ew/ew_config.hpp>      // holds system specific includes and #define
#include <ew/Ew_program.hpp>

//


//
#include <ew/maths/maths.hpp>

#include <ew/core/thread/thread.hpp>
#include <ew/core/mutex/mutex.hpp>
#include <ew/core/application/simple_application.hpp>
#include <ew/maths/maths.hpp>
#include <ew/graphics/graphics.hpp>
#include "../implementation/graphics/renderer/opengl/renderer.hpp"

//
#include <ew/graphics/graphics.hpp>
#include <ew/Ew_program.hpp>


#include <ew/graphics/gui/widget/window/window.hpp>
#include <ew/graphics/gui/widget/window/window_properties.hpp>


#include "../implementation/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/graphics/renderer/opengl/libGLU.hpp"

using namespace ew::implementation::graphics::rendering::opengl;



freetype::font_data our_font;

using namespace ew::core::types;

/* ----------------------------------------------- */
using namespace ew::core::MUTEX;
using namespace ew::core::THREAD;
using namespace ew::graphics::gui;


mutex * nrRuningthreads_mtx;
u32 nrRuningthreads = 0; // will inc/dec by threads
u32 nrthreads = 0; // will be filled by args

// will be masked
u32 app_width = 100;
u32 app_height = 100;
s32 app_x = 0;
s32 app_y = 0;


display * guiDpy = 0;

inline int next_p2(int a)
{
	int rval = 1;

	while (rval < a)
		rval <<= 1;
	return rval;
}

// struct Glyph
// {
//   int  xoffset;
//   int  yoffset;
//   int  width;
//   int  height;
//   int  xadvance;
//   int  yadvance;
//   GLuint  texID;
// };

class myWindow : public ew::graphics::gui::window
{
public:
	bool loop;

public:
	myWindow(display * dpy, window_properties & properties)
		:
		window(dpy, (widget *) 0, properties)
	{
		std::cerr << "class myWindow::myWindow(......) ok" << "\n";
		loop = true;
	}

	virtual bool on_resize(u32 w, u32 h)
	{
		std::cerr << "myWindow::on_resize(..) ok" << "\n";

		lockDrawingContext();
		{
			ew_glViewport(0, 0, w, h);

			//     clear the window
			ew_glMatrixMode(GL_PROJECTION);
			ew_glLoadIdentity();
			ew_gluPerspective(45.0,
					  (GLdouble)((GLdouble) w / (GLdouble) h),
					  (GLdouble) 0.1,
					  (GLdouble) 100.0);

			//       gluOrtho2D( (GLdouble) 0.0, // left,
			//      (GLdouble) w, // right,
			//      (GLdouble) 0, // bottom,
			//      (GLdouble) h ); // top )




			// static const double c = 20.0/255.0;
			// ew_glClearColor(c, c, c, c);
			// ew_glClearColor(0.0, 0.1, 0, 0);
			//       ew_glClear(GL_COLOR_BUFFER_BIT);
			//       ew_glFlush();
			//       this->swapBuffers();
		}
		unlockDrawingContext();
		return true;
	}

	virtual bool on_close(void)
	{
		std::cerr << "myWindow::on_close(void)" << "\n";
		loop = false;
		return true;
	}

};



// move this to x11Window code

void renderthreadFunc(myWindow * win)
{
	win->lock();
	win->lockDrawingContext();


	ew_glShadeModel(GL_SMOOTH);
	ew_glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ew_glClearDepth(1.0f);
	ew_glEnable(GL_DEPTH_TEST);
	ew_glDepthFunc(GL_LEQUAL);
	ew_glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	our_font.init("../Test.ttf", 16);

	win->on_resize(app_width, app_height);

	win->unlockDrawingContext();
	win->unlock();


	////////////////////////////////////////////////////
	while (win->loop == true) {
		win->lock();
		if (win->lockDrawingContext() == true) {

			ew_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // Clear Screen And Depth Buffer
			ew_glLoadIdentity();     // Reset The Current Modelview Matrix
			ew_glTranslatef(0.0f, 0.0f, -1.0f);      // Move One Unit Into The Screen

			// Blue Text
			ew_glColor3ub(0, 0, 0xff);

			// Position The WGL Text On The Screen
			//glRasterPos2f(-0.40f, 0.35f);
			ew_glRasterPos2f(0.0f, 0.0f);

			// Here We Print Some Text Using Our FreeType Font
			// The only really important command is the actual print() call,
			// but for the sake of making the results a bit more interesting
			// I have put in some code to rotate and scale the text.

			// Red text
			ew_glColor3ub(0xff, 0, 0);

			ew_glPushMatrix();
			ew_glLoadIdentity();
			ew_glTranslatef(0, 0, -50);
			freetype::print(our_font, 320, 240, "Active FreeType Text");
			ew_glPopMatrix();

			// swap_buffers:
			win->swapBuffers();
		}
		win->unlockDrawingContext();
		win->unlock();


	} // ! while ( isAvailable() == true )
}


void windowthread()
{
	myWindow * win = 0;

	std::cerr << "void  windowthread()" << "\n";

	/* x11Window */
	// todo :
	window_properties properties;

	properties.x = app_x;
	properties.y = app_y;
	properties.width = app_width;
	properties.height = app_height;

	win = new myWindow(guiDpy, properties);
	if (!win)
		return ;

	win->show(true);
	// win->startEventthread();
	// win->event_thread()->start();

	bool useRenderthread = true; // true;
	if (useRenderthread == false) {
		renderthreadFunc(win);
	} else {
		//  release the glx ctx if we launch a thread
		if (win->unlockDrawingContext() == false) {
			std::cerr << "exit @ line " << __LINE__ << std::endl;
			_exit(1);
		}

		thread * renderthread = new thread((ew::core::THREAD::func_t) renderthreadFunc, (ew::core::THREAD::Arg_t) win);
		if (renderthread->start() != true) {
			// thread_exit();
			// win->event_thread()->stop();
		}
		renderthread->join();
	}

	// litle test
	win->lock()
	;
	win->unlock();

	delete win; // destroy

	nrRuningthreads_mtx = newmutex();
	nrRuningthreads_mtx->lock()
	;
	--nrRuningthreads;
	nrRuningthreads_mtx->unlock();

	std::cerr << "void  windowthread() :: done" << "\n";
}

int main(int ac, char ** av)
{
	if (ew::graphics::gui::init() == false) {
		std::cerr << "ew::graphics::gui::init() :: error" << "\n";
		ew::core::System::exit(1);
	}

	if (ew::graphics::rendering::init() == false) {
		std::cerr << "ew::graphics::gui::init() :: error" << "\n";
		ew::core::System::exit(1);
	}


	// alloc display before creating any widget
	guiDpy = new ew::graphics::gui::display();
	if (guiDpy->open() == false) {
		return 1;
	}

	windowthread();

	delete guiDpy;

	if (ew::graphics::gui::quit() == false) {
		std::cerr << "ew::graphics::gui::quit() :: error" << "\n";
		ew::core::System::exit(1);
	}



	return 0;
}
