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

// NEW
#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"



namespace test
{
namespace graphics
{

using namespace ew::core::types;

/* ----------------------------------------------- */
using namespace ew::core::threading;
using namespace ew::graphics::gui;
using namespace ew::graphics::gui::events;

using namespace ew::implementation::graphics::rendering::opengl;
using namespace ew::graphics::rendering;



mutex nrRuningthreads_mtx;
u32 nrRuningthreads = 0; // will inc/dec by threads
u32 nrthreads = 0; // will be filled by args

// Will be masked
u32 WIDTH = 640;
u32 HEIGHT = 480;
s32 WIN_X = 0;
s32 WIN_Y = 0;

s32 last_pointer_x = -10; // if < 0 NA
s32 last_pointer_y = -10; // if < 0 NA

mutex app_quit_mtx;
condition_variable app_quit_cond(&app_quit_mtx);

display * dpy;

inline int next_p2(int a)
{
	int rval = 1;

	while (rval < a)
		rval <<= 1;
	return rval;
}

#define X_INC 5.0f
#define Y_INC 5.0f

static u8 color_white[3] = { 0xff, 0xff, 0xff };
static u8 color_grey[3] = { 0xce, 0xce, 0xce };
static u8 color_red[3] = { 0xff, 0x00, 0x00 };
static u8 color_green[3] = { 0x00, 0xff, 0x00 };

static u32 last_PushTime = 0;


void    push_draw_event(widget * widget)
{
	u32  now = ew::core::time::get_ticks();

	if (now - last_PushTime < 4)
		return;

	struct widget_event * ev =  new struct widget_event();
	ev->type = WidgetDrawEvent;
	ev->widget = widget;
	ev->display = dpy;
	ev->time = now;
	dpy->push_events((events::event **)&ev, 1);

	last_PushTime = now;
}


class Slider : public ew::graphics::gui::widget
{
public:
	Slider(ew::graphics::gui::widget * parent, s32 x, s32 y, u32 w, u32 h)
		:
		_parent(parent),
		_x(x),
		_y(y),
		_w(w),
		_h(h)
	{

		current_color = color_white;
		current_color = color_grey;
		was_selected = false;
		pointer_over = false;
	}

	//
	//ew::core::object
	virtual const char * class_name() const
	{
		return "Slider";
	}
	virtual const char * getName() const
	{
		return "";
	}

	// ILockableObject
	virtual bool lock()
	{
		return false;
	}
	virtual bool trylock()
	{
		return false;
	}
	virtual bool unlock()
	{
		return false;
	}

	// env info
	// display
	virtual ew::graphics::gui::display * display() const
	{
		return _parent->display();
	}

	// rendering
	virtual rendering_context * renderingContext() const
	{
		return _parent->renderingContext();
	}
	virtual ew::graphics::rendering::renderer * renderer() const
	{
		return _parent->renderer();
	}

	// renderring context
	virtual bool trylockDrawingContext()
	{
		return false;
	}
	virtual bool lockDrawingContext()
	{
		return false;
	}
	virtual bool unlockDrawingContext()
	{
		return false;
	}
	virtual bool swapBuffers()
	{
		return false;
	}

	// position
	virtual s32 x()
	{
		return _x;
	};
	virtual s32 y()
	{
		return _y;
	};
	virtual bool set_x(s32 x)
	{
		_x = x;
		return true;
	};
	virtual bool set_y(s32 y)
	{
		_y = y;
		return true;
	};

	// size
	virtual u32  width() const
	{
		return _w;
	}
	virtual u32  height() const
	{
		return _h;
	}
	virtual bool setWidth(u32 w)
	{
		_w = w;
		return true;
	}
	virtual bool setHeight(u32 h)
	{
		_h = h;
		return true;
	}

	virtual bool isAvailable()
	{
		return true;
	}

	// actions
	virtual bool show()
	{
		return false;
	}
	virtual bool hide()
	{
		return false;
	}

	virtual bool move(s32 x, s32 y)
	{
		return false;
	}
	virtual bool resize(u32 width, u32 height)
	{
		return false;
	}

	// events callbacks
	virtual bool on_create(const widget_event * ev)
	{
		return true;
	}

	virtual bool on_destroy(const widget_event * ev)
	{
		return true;
	}

	virtual bool draw()
	{

		glColor3ubv(current_color);
		glBegin(GL_QUADS);
		{
			// glTexCoord2i( 0, 0 );
			glVertex2i(_x + 0, _parent->height() - _y);  // up - left

			// glTexCoord2i( 0, 1 );
			glVertex2i(_x + 0, _parent->height() - (_y + _h));   // donw - left

			//  glTexCoord2i( 1, 1 );
			glVertex2i(_x + _w, _parent->height() - (_y + _h));

			//   glTexCoord2i( 1, 0 );
			glVertex2i(_x + _w, _parent->height() - _y);
		}
		glEnd();

		return true;
	}

	virtual bool on_draw(const widget_event * ev)
	{
		draw();
		return true;
	}

	virtual bool on_move(const widget_event * ev)
	{
		return false;
	}
	virtual bool on_resize(const widget_event * ev)
	{
		return false;
	}
	virtual bool on_close(const widget_event * ev)
	{
		return false;
	}

	virtual bool on_key_press(const keyboard_event * ev)
	{
		return false;
	}
	virtual bool on_key_release(const keyboard_event * ev)
	{
		return false;
	}

	virtual bool on_mouse_button_press(const button_event * ev)
	{

		s32 x = ev->x;
		s32 y = ev->y;

		if (isCoordOver(x, y)) {
			was_selected = true;
			x_sel = x;
			y_sel = y;
		}

		updateSliderStatus();

		return true;
	}

	virtual bool on_mouse_button_release(const button_event * ev)
	{
		if (was_selected == true) {
			was_selected = false;
			updateSliderStatus();
		}

		return true;
	}

	virtual bool on_pointer_motion(const pointer_event * ev)
	{

		bool changed = pointer_over;

		if (isCoordOver(ev->x, ev->y)) {
			pointer_over = true;
		} else {
			pointer_over = false;
		}

		if (was_selected == true) {

			s32 diff_y = ev->y - y_sel;

			set_y(y() + diff_y);

			x_sel = ev->x;
			y_sel = ev->y;

			changed = !pointer_over;
		}

		updateSliderStatus();
		if (changed != pointer_over)
			return true;

		return false;
	}

	virtual bool on_pointer_enter(const pointer_event * ev)
	{
		return false;
	}
	virtual bool on_pointer_leave(const pointer_event * ev)
	{
		return false;
	}

	virtual bool on_focus_in(const widget_event * ev)
	{
		return false;
	}


	virtual bool on_focus_out(const widget_event * ev)
	{
		return false;
	}

	virtual bool updateSliderStatus()
	{

		u8 * last_cr_color = current_color;

		if (!isCoordOver(last_pointer_x, last_pointer_y)) {
			pointer_over = false;
		}

		if (pointer_over)
			current_color = color_green;
		else
			current_color = color_grey;

		if (was_selected) {
			current_color = color_red;
		}

		if (last_cr_color != current_color) {
			return true;
		}

		return false;
	}

	bool isCoordOver(s32 x_, s32 y_)
	{
		// Gui::...::GetMouse()->x()
		// Gui::...::GetMouse()->y()

		s32 max_x = x() + width();
		s32 max_y = y() + height();

		if (((x_ >= x()) && (x_ <= max_x)) && ((y_ >= y()) && (y_ <= max_y))) {
			return true;
		}
		return false;
	}

private:
	ew::graphics::gui::widget * _parent;
	// coords
	s32 _x;
	s32 _y;
	u32 _w;
	u32 _h;
	// slider color
public:
	u8 * current_color;
	bool was_selected;
	bool pointer_over;

	// last click
	s32 x_sel;
	s32 y_sel;

};

class main_window : public ew::graphics::gui::window
{
public:
	bool loop;
	mutex quit_mtx;
	condition_variable * quit_cond;

	int nb_slider;
	Slider ** slider;

private:
	int destroy_sliders()
	{
		if (slider) {
			for (int i = 0; i < nb_slider; ++i)
				delete slider[i];
			delete [] slider;
		}
		return 0;
	}

	int build_sliders()
	{
		destroy_sliders();
		nb_slider = width() / (12 * 2);
		slider = new Slider * [nb_slider];
		for (int i = 0; i < nb_slider; ++i) {
			slider[i] = new Slider(this,  i * (12 * 2), 0, 12, height() / 2.0f);
		}
		return 0;
	}

	int draw_sliders()
	{
		for (int i = 0; i < nb_slider; ++i) {
			slider[i]->draw();
		}
		return 0;
	}

public:
	main_window(window_properties & properties)
		:
		window(dpy, 0, properties),
		nb_slider(0),
		slider(0)
	{

		loop = true;
		quit_cond = new condition_variable(&quit_mtx);
		build_sliders();
	}

	~main_window()
	{
		destroy_sliders();
		delete quit_cond;
		quit_cond = 0;
	}


	virtual bool on_create(const widget_event * ev)
	{

		static unsigned char texture[ 64 ][ 64 ][ 4 ];
		for (int j = 0; j < 64; ++j)
			for (int i = 0; i < 64; ++i) {
				//   texture[ j ][ i ][ 0 ] = 0x2C;
				//   texture[ j ][ i ][ 1 ] = 0x5C;
				//   texture[ j ][ i ][ 2 ] = 0x8A;
				//   texture[ j ][ i ][ 3 ] = 0xFF;

				texture[ j ][ i ][ 0 ] = 0xff;
				texture[ j ][ i ][ 1 ] = 0xff;
				texture[ j ][ i ][ 2 ] = 0xff;
				texture[ j ][ i ][ 3 ] = 0xff;
			}

		GLuint texID;

		glShadeModel(GL_SMOOTH);
		//glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
		glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glGenTextures(1, &texID);

		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);

		resize(width(), height());
		return true;
	}

	virtual bool on_key_press(const keyboard_event * ev)
	{
		u32 key = ev->key;

		switch (key) {
		case 9: { /* esc */
			loop = false;
			mutex_locker lock(quit_mtx);
			quit_cond->signal();
		}
		break ;
		}

		return true;
	}

	virtual bool resize(u32 w , u32 h)
	{

		//     clear the window
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//     ew_gluPerspective( 45.0,
		//          ( GLdouble ) ( ( GLdouble ) w / ( GLdouble ) h ),
		//          ( GLdouble ) 0.001,
		//          ( GLdouble ) 10000.0 );

		ew_gluOrtho2D((GLdouble) 0.0,  // left,
			      (GLdouble) w, // right,
			      (GLdouble) 0, // bottom,
			      (GLdouble) h);  // top

		glViewport(0, 0, w, h);

		glMatrixMode(GL_MODELVIEW);

		build_sliders();
		return true;
	}

	virtual bool on_resize(const widget_event * ev)
	{
		return resize(ev->width, ev->height);
	}


	virtual bool on_close(const widget_event * ev)
	{

		mutex_locker lock(quit_mtx);
		quit_cond->signal();

		loop = false;
		return true;
	}

	virtual bool on_pointer_enter(const pointer_event * ev)
	{

		for (int i = 0; i < nb_slider; ++i) {
			slider[i]->updateSliderStatus();
		}

		push_draw_event(this);

		return true;
	}

	virtual bool on_pointer_leave(const pointer_event * ev)
	{

		// hack to set pointer state
		last_pointer_x = -1;
		last_pointer_y = -1;

		for (int i = 0; i < nb_slider; ++i) {
			slider[i]->updateSliderStatus();
		}

		push_draw_event(this);

		return true;
	}


	virtual bool on_pointer_motion(const pointer_event * ev)
	{
		s32 x = ev->x;
		s32 y = ev->y;

		std::cerr << "window : p motion (" << x << "," << y << ")\n" ;

		bool changed = false;

		// todo : move this to toolkit internals
		last_pointer_x = x;
		last_pointer_y = y;

		for (int i = 0; i < nb_slider; ++i) {
			if (slider[i]->on_pointer_motion(ev))
				changed = true;
		}

		// may push too many event
		if (changed)
			push_draw_event(this);

		return true;
	}

	virtual bool on_mouse_button_press(const button_event * ev)
	{

		last_pointer_x = ev->x;
		last_pointer_y = ev->y;

		for (int i = 0; i < nb_slider; ++i) {
			slider[i]->on_mouse_button_press(ev);
		}

		push_draw_event(this);
		return true;
	}

	virtual bool on_mouse_button_release(const button_event * ev)
	{
		last_pointer_x = ev->x;
		last_pointer_y = ev->y;

		for (int i = 0; i < nb_slider; ++i) {
			slider[i]->on_mouse_button_release(ev);
		}
		push_draw_event(this);
		return true;
	}

	virtual bool on_draw(const widget_event * ev)
	{

		glEnable(GL_TEXTURE_2D);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();
		glPushMatrix();

		glLoadIdentity();
		//glTranslatef( 0.0f, 0.0f, -25.0f );
		glTranslatef(0.0f, 0.0f, -1.0f);


		for (int i = 0; i < nb_slider; ++i) {
			slider[i]->on_draw(ev);
		}

		glPopMatrix();
		return true;
	}

};



// move this to x11Window code

void renderthreadFunc(main_window * win)
{


	////////////////////////////////////////////////////

	/*
	  // ACTIVE LOOP
	  while (win->loop == true) {
	  win->lock();
	  win->lockDrawingContext();
	  win->on_draw();
	  win->unlockDrawingContext();
	  win->unlock();

	  ew::core::time::sleep(32);
	}
	*/

	mutex_locker cond_mtx_lock(win->quit_mtx);
	win->quit_cond->wait();
}


void windowthread()
{
	main_window * win = 0;

	window_properties winProperties;

	winProperties.x = WIN_X ;
	winProperties.y = WIN_Y ;
	winProperties.width = WIDTH;
	winProperties.height = HEIGHT;
	winProperties.is_resizeable = true;
	winProperties.is_fullscreen = false;
	//winProperties.have_decoration = false;

	dpy->lock();
	win = new main_window(winProperties);
	win->show();
	dpy->unlock();

	bool useRenderthread = true; // true;
	if (useRenderthread == false) {
		renderthreadFunc(win);
	} else {
		thread * renderthread = new thread((thread::func_t) renderthreadFunc,
						   (thread::arg_t) win,
						   "renderthread");
		if (renderthread->start() != true) {
			// thread_exit();
			// win->event_thread()->stop();
		}
		renderthread->join();
		delete renderthread;
	}

	dpy->lock();
	delete win; // destroy
	dpy->unlock();


	nrRuningthreads_mtx.lock();
	--nrRuningthreads;
	nrRuningthreads_mtx.unlock();

	if (nrRuningthreads == 0)
		app_quit_cond.signal();

	// std::cerr << "void  windowthread() :: done" << "\n";
}

int main(int ac, char ** av)
{
	if (!((ac == 2) || (ac == 4))) {
		std::cerr << "usage : " << av[ 0 ] << " nr_window [ width height ]" << "\n";
		exit(1);
	}

	nrthreads = atoi(av[ 1 ]);
	if (ac == 4) {
		WIDTH = ew::maths::max(100, atoi(av[ 2 ]));
		HEIGHT = ew::maths::max(100, atoi(av[ 3 ]));
	}


	if (ew::core::time::init() == false) {
		std::cerr << "ew::core::time::init() :: error" << "\n";
		exit(1);
	}


	if (ew::graphics::gui::init() == false) {
		std::cerr << "ew::graphics::gui::init() :: error" << "\n";
		exit(1);
	}

	if (ew::graphics::rendering::init() == false) {
		std::cerr << "ew::graphics::rendering::init() :: error" << "\n";
		exit(1);
	}

	// alloc display before creating any widget
	dpy = new ew::graphics::gui::display();
	if (dpy->open() == false) {
		return 1;
	}

	nrRuningthreads = nrthreads;
	if (nrthreads) {
		thread ** windowthreadsVec = new thread * [ nrthreads ];

		for (u32 count = 0; count < nrthreads; ++count) {
			windowthreadsVec[count] = new thread((thread::func_t)windowthread, 0, "windowthread");
			windowthreadsVec[count]->start();
		}

		// we should have an app quit on last window ??
		app_quit_cond.wait();

		for (u32 count = 0; count < nrthreads; ++count) {
			windowthreadsVec[ count ] ->join();
			dpy->lock();
			delete windowthreadsVec[ count ];
			dpy->unlock();
			windowthreadsVec[ count ] = 0;
		}

		delete [] windowthreadsVec;
	}

	delete dpy;

	std::cerr << "ew::graphics::rendering::quit()" << "\n";
	if (ew::graphics::rendering::quit() == false) {
		exit(1);
	}

	std::cerr << "ew::graphics::gui::quit()" << "\n";
	if (ew::graphics::gui::quit() == false) {
		std::cerr << "ew::graphics::gui::quit() :: error" << "\n";
		exit(1);
	}

	std::cerr << "ew::core::time::quit()" << "\n";
	if (ew::core::time::quit() == false) {
		std::cerr << "ew::core::time::quit() :: error" << "\n";
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
