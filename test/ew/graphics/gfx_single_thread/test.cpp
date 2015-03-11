#include <cstdlib>
#include <list>

#include <ew/Console.hpp>
#include <ew/graphics/Gui.hpp>
#include <ew/core/Time.hpp>

// renderer
#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"

// TODO: common code to call getProgAddr
#include "../implementation/platform/unix/graphics/gui/x11_libGLX.hpp"

using namespace ew::core;
using namespace ew::core::types;
using namespace ew::console;
using namespace ew::graphics::gui;
using namespace ew::graphics::gui::events;
using namespace ew::implementation::graphics::rendering::opengl;

namespace ew
{
namespace test
{

// will be masked
s32 x = 100;
s32 y = 100;
u32 width = 300;
u32 height = 300;

u32 MIN_WIDTH  = 10;
u32 MIN_HEIGHT = 10;
u32 MAX_WIDTH  = 1600;
u32 MAX_HEIGHT = 1200;

u32 w_nr_div = 8;
u32 h_nr_div = 8;

// app {
u32 nrWindow = 1;
class myWindow;
myWindow ** winVec;
static u32 nrDeleted = 0;
bool loop = true;
// }


ew::graphics::gui::display * dpy = 0;

ew::graphics::gui::display * guiDpy = 0;

extern void gears_init(void);
extern void draw(void);
extern void draw_gears_direct(void);
extern void gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width, GLint teeth, GLfloat tooth_depth);

extern GLfloat view_rotx;
extern GLfloat view_roty;
extern GLfloat view_rotz;

extern TLS_DECL GLfloat angle;

void render(myWindow * win);
void render_win(myWindow * win);
void render(u32 width, u32 height);
void update_win(myWindow * win);

// will be attached to window
GLenum ew_debug_glGetError(const char * msg, const char * function, int line)
{

	GLenum st = ew_glGetError();

	if (st != GL_NO_ERROR) {
		cerr << msg << " in " << function << " @ line " << line << "\n";
		cerr << "st = " << st << "\n";
	}

	return st;
}

void render(u32 width, u32 height)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat h = (GLfloat) height / (GLfloat) width;
	glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0); // 3D

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -40.0);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	{
		draw();
		// draw_gears_direct(); // TODO: rename in draw_gears()
	}
	glPopAttrib();
}

class offscreen_buffer
{
public:
	virtual ~offscreen_buffer() { }

	virtual bool resize(u32 width, u32 height) = 0;
	virtual bool bind() = 0;
	virtual bool unbind() = 0;
};

class opengl_framebuffer_object : public offscreen_buffer
{
public:
	GLuint fboId;
	GLuint rboId;
	GLuint textureId;
	u32 width;
	u32 height;

public:
	bool release()
	{
		ew_glDeleteTextures(1, &textureId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);
		ew_glDeleteRenderbuffersEXT(1, &rboId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);
		ew_glDeleteFramebuffersEXT(1, &fboId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);
		return true;
	}

	bool build(u32 width_, u32 height_)
	{

		width  = width_;
		height = height_;

		// if (width == 0)
		//   width = 1;

		// if (height == 0)
		//   height = 1;

		// create a texture object
		ew_glGenTextures(1, &textureId);
		ew_glBindTexture(GL_TEXTURE_2D, textureId);
		ew_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		ew_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		ew_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		ew_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		ew_glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic_direction mipmap
		// check non power of 2 ?
		ew_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		ew_glBindTexture(GL_TEXTURE_2D, 0);

		// create a framebuffer object
		ew_glGenFramebuffersEXT(1, &fboId);
		ew_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

		// create a renderbuffer object to store depth info
		ew_glGenRenderbuffersEXT(1, &rboId);
		ew_glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rboId);
		ew_glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
		ew_glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

		// attach the texture to FBO color attachment point
		ew_glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureId, 0);

		// attach the renderbuffer to depth attachment point
		ew_glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboId);

		// check FBO status
		GLenum status = ew_glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
			cerr << "error in FBO\n";
		} else {
			// cerr << "FBO OK\n";
		}
		// switch back to window-system-provided framebuffer
		ew_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		return true;
	}

	bool resize(u32 width_, u32 height_)
	{
		release();
		build(width_, height_);
		return true;
	}

	opengl_framebuffer_object(u32 width_, u32 height_)
	{
		build(width_, height_);
	}

	bool bind()
	{
		ew_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);
		return true;
	}

	bool unbind()
	{
		ew_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);
		return true;
	}

	virtual ~opengl_framebuffer_object()
	{
		ew_glDeleteTextures(1, &textureId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);
		ew_glDeleteRenderbuffersEXT(1, &rboId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);
		ew_glDeleteFramebuffersEXT(1, &fboId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);
	}
};

class myWindow : public ew::graphics::gui::window
{
public:
	opengl_framebuffer_object * offscreen;

public:
	bool  add_widget(widget * widget)
	{
		return true;
	}

public:
	myWindow(ew::graphics::gui::display * dpy, ew::graphics::gui::window * parent, window_properties prop)
		:
		window(dpy, parent,  prop),
		rot0(0),
		fps(0),
		update(60),
		fps_t0(0),
		t0(0),
		angle_inc_per_second(44.0),
		angle(0.0)
	{
		offscreen = 0;
		gears_rdr_flag = false;
	}

	void renderOffscreen()
	{

		glPushAttrib(GL_ALL_ATTRIB_BITS); // lighting and color mask
		{
			offscreen->bind();
			{
				ew::test::render(offscreen->width, offscreen->height);

				ew_glGenerateMipmapEXT(GL_TEXTURE_2D);
				ew_glBindTexture(GL_TEXTURE_2D, 0);
			}
			offscreen->unbind();
		}
		glPopAttrib();
	}

	void draw_rect_at(s32 x, s32 y, s32 _width, s32 _height)
	{

		//glNormal3f(0, 0, 1);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		s32 max_width = x + _width;
		s32 max_height = y + _height;
		glBegin(GL_QUADS);
		{
			static float T = 1.0f;

			glTexCoord2f(T, T);
			glVertex2i(max_width , max_height);

			glTexCoord2f(0.0f, T);
			glVertex2i(x         , max_height);

			glTexCoord2f(0.0f, 0.0f);
			glVertex2i(x     , y);

			glTexCoord2f(T, 0.f);
			glVertex2i(max_width, y);
		}
		glEnd();
	}

	virtual bool on_draw(const widget_event * ev)
	{

		if (!offscreen)
			return true;

		update_win(this);
		// angle = this->angle;

		renderOffscreen();

		s32 w_step  = width() / w_nr_div;
		s32 h_step  = height() / h_nr_div;

		// reset camera
		glClearColor(0.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set viewport
		glViewport(0, 0, width(), height());

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// set projection
		ew_gluOrtho2D(0, width(), 0, height());

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

#if 1
		// Lightning
		// texture needs light
		// set up light colors (ambient, diffuse, specular)
		// GLfloat lightKa[] = { 0.8, 0.7, 0.6, 0.5 };  // ambient light
		// GLfloat lightKa[] = { 1.0, 1.0, 1.0, 1.0 };  // ambient light
		// GLfloat lightKd[] = { 0.1, 0.2, 1.0, 0.5 };  // diffuse light
		// GLfloat lightKs[] = { 0.6, 0.4, 0.3, 0.1 };  // specular light

		u32 lightNum = GL_LIGHT1;
		glEnable(GL_LIGHTING);
		// glLightfv(lightNum, GL_AMBIENT,  lightKa);
		// glLightfv(lightNum, GL_DIFFUSE,  lightKd);
		// glLightfv(lightNum, GL_SPECULAR, lightKs);

		// position the light
		float lightPos[4] = { 0, 0, 1, 1}; // positional light
		glLightfv(lightNum, GL_POSITION, lightPos);
		glEnable(lightNum);
#endif

		glEnable(GL_TEXTURE_2D);
		{
			ew_glBindTexture(GL_TEXTURE_2D, offscreen->textureId);

			for (u32 h = 0; h + h_step <= height(); h += h_step * 2) {
				for (u32 w = 0; w + w_step <= width(); w += w_step * 2) {
					draw_rect_at(w,  h, w_step, h_step);
				}
			}
		}
		glDisable(GL_TEXTURE_2D);

		return true;
	}

	bool on_create(const widget_event * ev)
	{
#if 1
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_CULL_FACE);
		// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
		// ew_glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		// glEnable(GL_COLOR_MATERIAL);
		glClearColor(0, 0, 0, 0);                   // background color
		// glClearStencil(0);                          // clear stencil buffer
		// glClearDepth(1.0f);       // 0 is near, 1 is far
		// glDepthFunc(GL_LEQUAL);
#endif

		gears_init();

		// create offscreen buffer
		if (!offscreen) {
			offscreen = new  opengl_framebuffer_object(width() / w_nr_div, height() / h_nr_div);
		}

		return true;
	}

	bool on_close(const widget_event * ev)
	{
		// must set flag to exit loop
		hide();
		nrDeleted++;
		if (nrDeleted >= nrWindow)
			loop = false;
		return true;
	}

	bool  resize(u32 width , u32 height)
	{

		// offscreen->resize(w, h);
		if (offscreen && ((offscreen->width != width) || (offscreen->height != height))) {

			// cerr << "SIZE HAS CHANGED !!!\n";
			// offscreen->bind();
			// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// offscreen->unbind();

			// delete offscreen;
			// offscreen = new opengl_framebuffer_object(width / w_nr_div, height / h_nr_div);
			offscreen->resize(width / w_nr_div, height / h_nr_div);
		}

		return true;
	}

	bool  on_resize(const widget_event * ev)
	{
		return resize(ev->width, ev->height);
	}

	virtual bool on_key_press(const keyboard_event * ev)
	{
		u32 key = ev->key;
		cerr << "test :: key release = " << key << "\n";
		cerr << "test :: unicode = " << ev->unicode << "\n";
		switch (key) {
		case events::keys::Escape:
			loop = false;
			break;

		case events::keys::Keypad_Plus:
			angle_inc_per_second += 2.0 ;
			break;

		case 82:
			angle_inc_per_second -= 2.0 ;
			break;

		case events::keys::Left: /* left */
			view_roty += 4.0f;
			break ;

		case events::keys::Right: /* right */
			view_roty -= 4.0f;
			break ;

		case events::keys::Up: /* up */
			view_rotz -= 4.0f;
			break ;

		case events::keys::Down: /* down */
			view_rotz += 4.0f;
			break ;


		case keys::UNICODE: {

			if (ev->unicode == 'w') {
				if (ev->ctrl)
					w_nr_div--;
				else
					w_nr_div++;

				if (w_nr_div == 0)
					w_nr_div = 1;
			}

			if (ev->unicode == 'h') {
				if (ev->ctrl)
					h_nr_div--;
				else
					h_nr_div++;

				if (h_nr_div == 0)
					h_nr_div = 1;
			}

			std::cerr << "w_nr_div = " << w_nr_div << "\n";
			std::cerr << "h_nr_div = " << h_nr_div << "\n";
			std::cerr << "w_nr_div * h_nr_div = " << w_nr_div * h_nr_div << "\n";



			std::cerr << "width()  = " << width()  << "\n";
			std::cerr << "height() = " << height() << "\n";

			resize(width() , height());

		}
		break ;
		}

		return true;
	}

	// vars
	u32 rot0;
	u32 fps;
	u32 update;
	u32 fps_t0;
	u32 t0;
	double angle_inc_per_second;
	GLfloat angle;
	u32 last_frame;
	bool gears_rdr_flag;
};



void  update_win(myWindow * win)
{
	win->t0 = ew::core::time::get_ticks();

	u32 diff = win->t0 - win->rot0;
	static u32 min_time = 1000 / win->update;

	if (diff > min_time) {
		win->rot0 = ew::core::time::get_ticks();

		win->angle += win->angle_inc_per_second * ((long double) diff / 1000.0);
		if (win->angle >= 360.0)
			win->angle -= 360.0;

		angle = win->angle;
	}
}


void render(myWindow * win)
{
	update_win(win);
	angle = win->angle;
	render(win->width(), win->height());
}

void render_scene(myWindow ** winVec, u32 nrWindow, bool use_fps_limiter)
{
	static const u32 fps = 60;
	static u32 target_freq = 1000 / fps;

	for (u32 i = 0; i < nrWindow; ++i) {

		myWindow * win = winVec[i];
		if (!win)
			continue ;

		if (use_fps_limiter == true) {
			u32 current_frame = ew::core::time::get_ticks();
			u32 diff = current_frame - win->last_frame;
			if (diff < target_freq) {
				ew::core::time::sleep(target_freq - diff);
			}
		}

		win->display()->lock();
		win->renderingContext()->lock();
		update_win(win);
		win->on_draw(nullptr);
		win->swapBuffers();
		win->renderingContext()->unlock();
		win->display()->unlock();
		++win->fps;

		{
			if (win->t0 - win->fps_t0 >= 1000) {
				cerr << "fps = " << win->fps << "\n";
				if (win->fps < fps)
					target_freq--;
				else if (win->fps > fps)
					target_freq++;

				win->fps_t0 = ew::core::time::get_ticks();
				win->fps = 0;
			}
		}

		if (use_fps_limiter == true) {
			win->last_frame = ew::core::time::get_ticks();
		}
	}
}

void poll_events(ew::graphics::gui::display * dpy, myWindow ** winVec, u32 nrWindow, bool * loop)
{
	dpy->poll_events(false, 16);

	u32 nr = dpy->get_event_dispatcher()->get_queue_size();
	if (nr) {
		dpy->get_event_dispatcher()->dispatch_events(nr);
	}
}

int main(int ac, char ** av)
{

	cerr << "usage : " << av[0] << " [nrWindow] [[width height]]]\n";
	if (ac == 4) {
		nrWindow = in_range<u32>(1, ::atoi(av[1]), 1000);
		width = in_range<u32>(MIN_WIDTH, ::atoi(av[2]), MAX_WIDTH);
		height = in_range<u32>(MIN_HEIGHT, ::atoi(av[3]), MAX_HEIGHT);
	}

	// init gui
	ew::core::time::init();
	ew::graphics::gui::init();
	ew::graphics::gui::setSinglethreadEventPollingMode();   // select event polling thread model :-)
	ew::graphics::rendering::init();

	// alloc display before creating any widget
	dpy = new ew::graphics::gui::display();
	if (dpy->open() == false) {
		return 1;
	}

	dpy->lock();
	{
		// create main window
		window_properties prop;
		prop.x =  x;
		prop.y =  y;
		prop.width =  width;
		prop.height = height;
		prop.is_resizeable = true;
		prop.is_fullscreen = false;
		prop.use_offscreen_buffer = !true;

		winVec = new myWindow * [ nrWindow ];

		std::cerr << "create " << nrWindow << " window" << ((nrWindow > 1) ? "s" : "") << "...\n";
		for (u32 i = 0; i < nrWindow; ++i) {
			winVec[i] = new myWindow(dpy, (ew::graphics::gui::window *)0,  prop);
		}
	}
	dpy->unlock();

	// display window
	for (u32 i = 0; i < nrWindow; ++i) {
		winVec[i]->show();
	}

	// main loop
	while (loop == true) {
		poll_events(dpy, winVec, nrWindow, &loop);
		render_scene(winVec, nrWindow, false /* fps_limiter is buggy */);
	}

	dpy->lock();
	{
		for (u32 i = 0; i < nrWindow; ++i) {
			if (winVec[i] != 0) {
				delete winVec[i];
				winVec[i] = 0;
			}
		}
		delete [] winVec;
	}
	dpy->unlock();

	delete dpy;

	ew::graphics::rendering::quit();
	ew::graphics::gui::quit();
	ew::core::time::quit();

	return 0;
}

}
}


int main(int ac, char ** av)
{
	int ret = 0;
	try {
		ret = ew::test::main(ac, av);
	}

	catch (ew::core::exception & e) {
		std::cerr << "e.what() = " << e.what() << "\n";
	}

	return ret;
}
