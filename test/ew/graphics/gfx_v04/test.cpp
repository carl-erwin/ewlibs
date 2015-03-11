// -*- mode:c++; offset-width: 2; -*-
#include <iostream>
#include <vector>
#include <stdexcept>


#include <GL/glut.h>

//
#include <sys/time.h>



#include <ew/ew_config.hpp>      // holds system specific includes and #define

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


// NEW
#include <ew/graphics/gui/widget/window/window.hpp>
#include <ew/graphics/gui/widget/window/window_properties.hpp>

//
#include "../implementation/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/graphics/renderer/opengl/libGLU.hpp"

using namespace ew::implementation::graphics::rendering::opengl;


GLuint texID;
u32 app_width = 100;
u32 app_height = 100;
s32 app_x = 0;
s32 app_y = 0;


ew::graphics::gui::display * guiDpy;

inline bool checkGL_Error(const char * file = "unknown file", u32 line = 0)
{
	// std::cerr << "OpenGL :: check :: " << file << " @" << line << "\n";
	if (ew_glGetError() != GL_NO_ERROR) {
		std::cerr << "OpenGL :: error :: " << file << " @" << line << "\n";
		exit(1);
	}
	return false;
}

#define CHECK_GL_ERROR() checkGL_Error(__FILE__, __LINE__ )

// Freetype2
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

using namespace ew::core::types;

/* ----------------------------------------------- */
using namespace ew::core::MUTEX;
using namespace ew::core::THREAD;
using namespace ew::graphics::gui;


mutex * nrRuningthreads_mtx;
u32 nrRuningthreads = 0; // will inc/dec by threads
u32 nrthreads = 0; // will be filled by args

// will be masked
u32 width = 100;
u32 height = 100;
s32 x = 0;
s32 y = 0;

// /* Create checkerboard texture */
// #define checkImageWidth 64
// #define checkImageHeight 64
// static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

// void makeCheckImage(void)
// {
//   int i, j, c;
//   for (i = 0; i < checkImageHeight; i++) {
//     for (j = 0; j < checkImageWidth; j++) {

//       c = ((((i&0x8)==0)^((j&0x8))==0))*255;
//       checkImage[i][j][0] = (GLubyte) c;
//       checkImage[i][j][1] = (GLubyte) c;
//       checkImage[i][j][2] = (GLubyte) c;
//       checkImage[i][j][3] = (GLubyte) 255;
//     }
//   }
// }


class myWindow : public ew::graphics::gui::window
{
public:
	bool loop;

public:
	myWindow(ew::graphics::gui::display * dpy, window_properties & properties)
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
			ew_glViewport(0, 0, (GLsizei) w, (GLsizei) h);
			ew_glMatrixMode(GL_PROJECTION);
			ew_glLoadIdentity();
			ew_gluPerspective(60.0, (GLfloat) w / (GLfloat) h, 1.0, 30.0);
			ew_glMatrixMode(GL_MODELVIEW);
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

	virtual bool on_key_press(u32 key)
	{
		std::cerr << "myWindow::on_key_press(u32)" << "\n";
		if (key == 9)
			loop = false;
		return true;
	}

};


// move this to x11Window code

void renderthreadFunc(myWindow * win)
{
	u8 textureTest[ 64 ][ 64 ][ 4 ];

	srandom(255);
	for (int j = 0; j < 64; ++j)
		for (int i = 0; i < 64; ++i) {
			textureTest[ j ][ i ][ 0 ] = (u8) random() & 0xff;
			textureTest[ j ][ i ][ 1 ] = (u8) random() & 0xff;
			textureTest[ j ][ i ][ 2 ] = (u8) random() & 0xff;
			textureTest[ j ][ i ][ 3 ] = 0xff;

			//   textureTest[j][i][0] = 0x00;
			//   textureTest[j][i][1] = 0x00;
			//   textureTest[j][i][2] = 0xff;
			//   textureTest[j][i][3] = 0xff;



		}
	//   std::cerr << "  renderthreadFunc( myWindow * win ) \n";


	// setup gl
	win->lock()
	;
	win->lockDrawingContext();

	ew_glClearColor(0., 0., 0., 0.);
	CHECK_GL_ERROR();

	//   ew_glEnable( GL_TEXTURE_2D );
	//   CHECK_GL_ERROR();

	ew_glShadeModel(GL_FLAT);
	CHECK_GL_ERROR();

	ew_glEnable(GL_DEPTH_TEST);
	CHECK_GL_ERROR();

	// set projection
	win->unlockDrawingContext();
	win->on_resize(width, height);
	win->lockDrawingContext();

	//   makeCheckImage();
	//  ew_glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// CHECK_GL_ERROR();

	int list_base = ew_glGenLists(1);
	ew_glGenTextures(1, &texID);

	CHECK_GL_ERROR();
	ew_glBindTexture(GL_TEXTURE_2D, texID);
	CHECK_GL_ERROR();

	ew_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	CHECK_GL_ERROR();

	ew_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	CHECK_GL_ERROR();



	//   ew_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//   CHECK_GL_ERROR();

	//   ew_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//   CHECK_GL_ERROR();

	ew_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureTest);
	// ew_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	CHECK_GL_ERROR();
	// delete [] textureTest;
	ew_glNewList(list_base, GL_COMPILE);
	ew_glBegin(GL_QUADS);
	{
		ew_glTexCoord2f(0.0, 0.0);
		ew_glVertex3f(-1.0, 1.0, 0.0);
		ew_glTexCoord2f(0.0, 64.0);
		ew_glVertex3f(-1.0, -1.0, 0.0);
		ew_glTexCoord2f(64.0, 64.0);
		ew_glVertex3f(1.0, -1.0, 0.0);
		ew_glTexCoord2f(64.0, 0.0);
		ew_glVertex3f(1.0, 1.0, 0.0);
	}
	ew_glEnd();
	ew_glEndList();

	win->unlockDrawingContext();
	win->unlock();

	////////////////////////////////////////////////////
	static float rot = 0.0;
	while (win->loop == true) {
		win->lock()
		;
		if (win->lockDrawingContext() == true) {
			ew_glEnable(GL_TEXTURE_2D);
			ew_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ew_glMatrixMode(GL_MODELVIEW);
			ew_glLoadIdentity();

			// ew_glDisable(GL_DEPTH_TEST);
			// ew_glDisable(GL_BLEND);
			// ew_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			ew_glColor3ub(0xff, 0xff, 0xff);
			ew_glTranslatef(0.0f, 0.0f, -10.0f);
			//glRotatef(rot, 0,0,1);

			ew_glBindTexture(GL_TEXTURE_2D, texID);
			CHECK_GL_ERROR();

			ew_glBegin(GL_QUADS);
			{
				ew_glTexCoord2f(0.0, 0.0);
				ew_glVertex3f(-2.0, -1.0, 0.0);
				ew_glTexCoord2f(0.0, 64.0);
				ew_glVertex3f(-2.0, 1.0, 0.0);
				ew_glTexCoord2f(64.0, 64.0);
				ew_glVertex3f(0.0, 1.0, 0.0);
				ew_glTexCoord2f(64.0, 0.0);
				ew_glVertex3f(0.0, -1.0, 0.0);
			}
			ew_glEnd();

			//   ew_glCallList(list_base);

			rot += 0.1;

			win->swapBuffers();
			win->unlockDrawingContext();
		}
		win->unlock();

		ew::core::time::usleep(1);

	} // ! while ( win->loop )
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

	bool useRenderthread = false; // true;
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
	//   win->lock();
	//   win->unlock();

	delete win; // destroy

	//   nrRuningthreads_mtx->lock();
	--nrRuningthreads;
	//   nrRuningthreads_mtx->unlock();

	std::cerr << "void  windowthread() :: done" << "\n";
}

int main(int ac, char ** av)
{
	if (ac == 2 || ac == 4) {
		if (ac == 4) {
			width = ew::maths::max(100, atoi(av[ 2 ]));
			height = ew::maths::max(100, atoi(av[ 3 ]));
		}
	} else {
		std::cerr << "usage : " << av[ 0 ] << " nr_window [ width height ]" << "\n";
		exit(1);
	}

	if (ew::graphics::gui::init() == false) {
		std::cerr << "ew::graphics::gui::init() :: error" << "\n";
		exit(1);
	}

	if (ew::graphics::rendering::init() == false) {
		std::cerr << "ew::graphics::gui::init() :: error" << "\n";
		exit(1);
	}


	// alloc display before creating any widget
	guiDpy = new ew::graphics::gui::display();
	if (guiDpy->open() == false) {
		return 1;
	}

	nrthreads = atoi(av[ 1 ]);
	nrRuningthreads = nrthreads;
	if (nrthreads) {
		thread ** windowthreadsVec = new thread * [ nrthreads ];

		for (u32 count = 0; count < nrthreads; ++count)
			windowthreadsVec[ count ] = new thread((ew::core::THREAD::func_t) windowthread);
		for (u32 count = 0; count < nrthreads; ++count) {
			windowthreadsVec[ count ] ->start();
		}

		// we should have an app quit on last window ??
		while (nrRuningthreads != 0)
			ew::core::time::usleep(10000);
	}

	if (ew::graphics::gui::quit() == false) {
		std::cerr << "ew::graphics::gui::quit() :: error" << "\n";
		exit(1);
	}

	//  delete nrRuningthreads_mtx;

	//   std::cerr <<  "main() :: pause()" << "\n";
	//   pause();

	return 0;
}
