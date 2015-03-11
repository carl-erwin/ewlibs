
#include <assert.h>
#include <cstdlib>
#include <list>
#include <vector>

#include <ew/Console.hpp>
#include <ew/graphics/Gui.hpp>
#include <ew/graphics/color.hpp>

#include <ew/graphics/font/font.hpp>

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
using namespace ew::graphics::fonts;


/*
TODO:

event dispatch

for each node if last_ddd_widget pass event to last selected

dispatch_event:
 wid = window->get_last_selected_widget() -> w or nullptr
 wid = window->find_widget_under_coords(x, y) -> this or child

*/

namespace ew
{
namespace test
{

// will be masked
s32 x = 100;
s32 y = 100;
u32 width = 512;
u32 height = 512;

u32 MIN_WIDTH  = 10;
u32 MIN_HEIGHT = 10;
u32 MAX_WIDTH  = 1600;
u32 MAX_HEIGHT = 1200;

u32 w_nr_div = 1;
u32 h_nr_div = 1;


u32 nrWindow = 1;
class myWindow;
myWindow ** winVec;
static u32 nrDeleted = 0;
bool loop = true;

ew::graphics::gui::display * dpy = nullptr;
ew::graphics::gui::display * guiDpy = nullptr;

ew::graphics::fonts::font * ft = nullptr;

u32 font_size = 12;

size_t nr_cp = 0;
s32 * cp_array = nullptr;

extern void gears_init(void);
extern void draw(void);
extern void draw_gears_direct(void);
extern void gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width, GLint teeth, GLfloat tooth_depth);

extern GLfloat view_rotx;
extern GLfloat view_roty;
extern GLfloat view_rotz;

void render_win(myWindow * win);
void update_win(myWindow * win);




void render_gears(u32 width, u32 height)
{

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLfloat h = (GLfloat) height / (GLfloat) width;
		glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0); // 3D

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glTranslatef(0.0, 0.0, -40.0);
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		draw();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
	}
	glPopAttrib();
}


/*
  todo : find a name or rectangular region
*/
struct rect {
	s32 x;
	s32 y;
	u32 width;
	u32 height;
};



/* move to proper header/source file */
class opengl_framebuffer_object : public offscreen_buffer
{
public:
	GLuint fboId;
	GLuint rboId;
	GLuint textureId;
	u32    _width;
	u32    _height;

	virtual u32  width()
	{
		return _width;
	}
	virtual u32  height()
	{
		return _height;
	}

	bool texture_id(u32 * id)
	{
		*id = textureId;
		return true;
	}

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

		_width  = width_;
		_height = height_;

		// if (width == 0)
		//   width = 1;

		// if (height == 0)
		//   height = 1;

		// create a texture object
		ew_glGenTextures(1, &textureId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		ew_glBindTexture(GL_TEXTURE_2D, textureId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		ew_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		ew_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);


		ew_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		ew_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		ew_glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic_direction mipmap
		ew_debug_glGetError("", __FUNCTION__, __LINE__);


		// check non power of 2 ?
		ew_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		ew_glBindTexture(GL_TEXTURE_2D, 0);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		// create a framebuffer object
		ew_glGenFramebuffersEXT(1, &fboId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		ew_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		// create a renderbuffer object to store depth info
		ew_glGenRenderbuffersEXT(1, &rboId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		ew_glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rboId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		ew_glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, _width, _height);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		ew_glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		// attach the texture to FBO color attachment point
		ew_glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureId, 0);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		// attach the renderbuffer to depth attachment point
		ew_glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboId);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		// check FBO status
		GLenum status = ew_glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
			cerr << "error in FBO\n";
		} else {
			// cerr << "FBO OK\n";
		}
		// switch back to window-system-provided framebuffer
		ew_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		return true;
	}

	virtual bool set_width(u32 w)
	{
		return resize(w, height());
	}

	virtual bool set_height(u32 h)
	{
		return resize(width(), h);
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
#if 0
		ew_glGenerateMipmapEXT(GL_TEXTURE_2D);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		ew_glBindTexture(GL_TEXTURE_2D, 0);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);
#endif
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

offscreen_buffer * get_new_offscreen_buffer()
{
	return new opengl_framebuffer_object(128, 128);
}


void render_quads(s32 x, s32 y, u32 width, u32 height)
{
	glBegin(GL_QUADS);
	{
		glVertex2i(x, y);
		glVertex2i(x, y + height);
		glVertex2i(x + width, y + height);
		glVertex2i(x + width, y);
	}
	glEnd();
}


/* test widget */
class myRect : public ew::graphics::gui::widget
{
public:
	bool use_offscreen;
	offscreen_buffer * offscreen;


	myRect(s32 x, s32 y, double r_, double g_, double b_)
	{
		r = r_;
		g = g_;
		b = b_;
		use_offscreen = !true;
		offscreen = nullptr;
		set_x(x);
		set_y(y);
	}

	void render_quads(s32 x, s32 y, u32 width, u32 height)
	{
		glBegin(GL_QUADS);
		{
			glColor3d(r, g, b);
			glVertex2i(x, y);
			glVertex2i(x, y + height);
			glVertex2i(x + width, y + height);
			glVertex2i(x + width, y);
		}
		glEnd();
	}


	virtual bool have_offscreen_buffer()
	{
		return offscreen != nullptr;
	}

	virtual offscreen_buffer * get_offscreen_buffer()
	{
		return offscreen;
	}

	// TODO:
	virtual bool render_offscreen_buffer()
	{
		return true;
	}

	s32 sel_x;
	s32 sel_y;
	virtual bool selected_at(s32 x, s32 y)
	{
		sel_x = x;
		sel_y = y;
		return true;
	}

	//
	virtual   ~myRect()
	{
		delete offscreen;
	}

	double r;
	double g;
	double b;

	u32 next_p2(u32 v)
	{
		u32 p2 = 1;
		while (p2 < v)
			p2 <<= 1;

		return p2;
	}

	/* return true is operation is succesful */
	bool resize(u32 w, u32 h)
	{

		// if !use_offscreen
		set_width(w);
		set_height(h);

		if (use_offscreen == false)
			return true;

		if (offscreen == nullptr) {
			offscreen = get_new_offscreen_buffer();
		}

		if (offscreen == nullptr) {
			// error
			return false;
		}

		set_width(w);
		set_height(h);

#if 0
		widget * p = dynamic_cast<widget *>(this->get_parent());

		w = next_p2(p->width());
		h = next_p2(p->height());

		w = (p->width());
		h = (p->height());
#endif

		return offscreen->resize(w, h);
	}

	bool draw_quad(s32 x, s32 y, u32 width, u32 height, double r, double g, double b)
	{
		glColor3d(r, g, b);
		glBegin(GL_QUADS);
		{
			glVertex2i(x, y);
			glVertex2i(x, y + height);
			glVertex2i(x + width, y + height);
			glVertex2i(x + width, y);
		}
		glEnd();
		return true;
	}

	virtual bool push_projection()
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		return true;
	}
	virtual bool pop_projection()
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		return true;
	}

	virtual bool push_modelview()
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		return true;
	}

	virtual bool pop_modelview()
	{
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		return true;
	}

	// TODO: split an move to widget::render()
	// add w->have_offscreen_buffer();


	// myRect
	virtual bool render()
	{

		widget * p = dynamic_cast<widget *>(this->get_parent());

		glDisable(GL_LIGHTING);

		push_projection();
		push_modelview();

		set_projection();
		set_modelview();


		if (offscreen) {

			glPushAttrib(GL_ALL_ATTRIB_BITS); // lighting and color mask
			{
				offscreen->bind();
				{
					// setup offscreen
					{
						// set_bg
						glClearColor(0, 0, 0, 1.0);
						ew_debug_glGetError("", __FUNCTION__, __LINE__);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						ew_debug_glGetError("", __FUNCTION__, __LINE__);


						// setup view port
						glViewport(0, 0, width(), height());
						ew_debug_glGetError("", __FUNCTION__, __LINE__);
					}
					// real rendering
					{
						if (0) {
							render_gears(width(), height());
							glDisable(GL_DEPTH_TEST);
						}

						//glPushAttrib(GL_ALL_ATTRIB_BITS);
						{
							// set projection
							glMatrixMode(GL_PROJECTION);
							glLoadIdentity();
							ew_gluOrtho2D(0, width(), height(), 0);

							// set modelview
							glMatrixMode(GL_MODELVIEW);
							glLoadIdentity();

							ew::graphics::color3ub col(255, 255, 255);
							ew::graphics::fonts::font_print(ft, 0, 0, p->width(), p->height(), col, cp_array, nr_cp);
						}
						//glPopAttrib();

					}
				}
				offscreen->unbind();
			}
			glPopAttrib();

		} else {

			glPushAttrib(GL_ALL_ATTRIB_BITS);
			if (1) {
				glEnable(GL_DEPTH_TEST);
				glClear(GL_DEPTH_BUFFER_BIT);
				render_gears(p->width(), p->height());
				glDisable(GL_DEPTH_TEST);
			}
			glPopAttrib();
			//
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			ew_gluOrtho2D(0, p->width(), p->height(), 0);

			// setup
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glViewport(0, 0, p->width(), p->height());

			ew::graphics::color3ub col(255, 255, 255);
			ew::graphics::fonts::font_print(ft, x(), y(), p->width(), p->height(), col, cp_array, nr_cp);
		}

		pop_projection();
		pop_modelview();

		if (offscreen == nullptr)
			return true;

		glEnable(GL_LIGHTING);


		set_projection();
		set_modelview();

		// TODO:
		// bool draw_offscreen(offscreen, x,y,w,h, use_opengl_coord_system);

		bool use_opengl_coord_system = !true;
		widget * w = this;
//
		glPushAttrib(GL_ALL_ATTRIB_BITS); // lighting and color mask
		{

			glEnable(GL_BLEND);
			ew_debug_glGetError("", __FUNCTION__, __LINE__);

			glDisable(GL_DEPTH_TEST);
			ew_debug_glGetError("", __FUNCTION__, __LINE__);

			glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA); // nice effect

			glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
			glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR); // ok ?

			glBlendFunc(GL_ONE,  GL_DST_ALPHA);
			ew_debug_glGetError("", __FUNCTION__, __LINE__);

			glEnable(GL_TEXTURE_2D);
			{
				u32 texid;
				offscreen->texture_id(&texid);
				ew_glBindTexture(GL_TEXTURE_2D, texid);
				s32 x = w->x();
				s32 y = w->y();
				s32 max_width  = x + w->width();
				s32 max_height = y + w->height();

				if (use_opengl_coord_system) {
					// OpenGL coords
					glBegin(GL_QUADS);
					{
						static float T = 1.0f;

						glTexCoord2f(T, T);
						glVertex2i(max_width , max_height);

						glTexCoord2f(0, T);
						glVertex2i(x         , max_height);

						glTexCoord2f(0, 0);
						glVertex2i(x     , y);

						glTexCoord2f(T, 0);
						glVertex2i(max_width, y);
					}
					glEnd();

				} else {

					// windowing system coords
					glBegin(GL_QUADS);
					{
						static float T = 1.0f;

						glTexCoord2f(0, T);
						glVertex2i(x, y);

						glTexCoord2f(0, 0);
						glVertex2i(x, max_height);

						glTexCoord2f(T, 0);
						glVertex2i(max_width, max_height);

						glTexCoord2f(T, T);
						glVertex2i(max_width, y);
					}
					glEnd();
				}
			}
			glDisable(GL_TEXTURE_2D);
		}
		glPopAttrib();

		return true;
	}

	// virtual pure function in widget class
	virtual const char * class_name() const
	{
		return "myRect";
	}
	virtual const char * getName() const
	{
		return "myRect instance";
	}
};

class myWindow : public ew::graphics::gui::window
{
public:
	////
	u32 fps;
	u32 fps_t0;
	char fps_str[64];


public:
	myWindow(ew::graphics::gui::display * dpy, ew::graphics::gui::window * parent, window_properties prop)
		:
		window(dpy, parent,  prop)
	{
		fps = 0;
		fps_t0 = 0;

		fps_str[0] = '\0';

	}

	virtual bool set_projection()
	{

		// set viewport
		glViewport(0, 0, width(), height());

		bool use_opengl_coord_system = !true;

		if (use_opengl_coord_system) {
			// set projection
			ew_glMatrixMode(GL_PROJECTION);
			ew_glLoadIdentity();
			ew_gluOrtho2D(0, width(), 0, height());
		} else {
			// move to TextView
			// use the window coordinate system
			//  (0,0) --------> (w,0)
			//   |
			//   |
			//   |
			//   |
			//  (0,h)
			//
			ew_glMatrixMode(GL_PROJECTION);
			ew_glLoadIdentity();
			ew_gluOrtho2D(0, width(), height(), 0);
			if (0)
				ew_glOrtho((GLdouble) 0.0,      /* left   */
					   (GLdouble) width(),  /* right  */
					   (GLdouble) height(), /* bottom */
					   (GLdouble) 0,        /* top    */
					   (GLdouble) - 1000.0, /* near   */
					   (GLdouble) 1000.0    /* far    */);
		}

		return true;
	}

	virtual bool set_movelview()
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		return true;
	}

#if 1

	// TODO:
	// scene.clear();
	// scene.set_bg();
	// scene.bind_texture(texid);
	// scene.begin(quad);
	// scene.add_tex_coord(t1); scene.add_vertex(v1);
	// scene.add_tex_coord(t2); scene.add_vertex(v2);
	// scene.add_tex_coord(t3); scene.add_vertex(v3);
	// scene.add_tex_coord(t4); scene.add_vertex(v4);
	// scene.end(quad);

	virtual bool render()
	{

		double bg_r = (float)(0xe9) / 255.0; // 233
		double bg_g = (float)(0xe9) / 255.0;
		double bg_b = (float)(0xe9) / 255.0;

		double bg_a = 100.0 / 100.0;
		ew_glClearColor(bg_r, bg_g, bg_b, bg_a);

		ew_glClearColor(0.0, 0.0, 1.0, 1.0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_gears(width(), height());

		set_projection();
		set_movelview();

		glEnable(GL_BLEND);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		glDisable(GL_DEPTH_TEST);
		ew_debug_glGetError("", __FUNCTION__, __LINE__);

		glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // ok
		u32 nr = number_of_children();
		for (u32 i = 0; i < nr; ++i) {
			myRect * r = dynamic_cast<myRect *>(get_child(i));
			r->render();
		}

		set_projection();
		set_movelview();

		// status bar delim
		ew_glColor4ub(0x77, 0x77, 0x77, 0xff);
		glBegin(GL_LINES);
		{
			glVertex2i(0          , height() - 16);    // left
			glVertex2i(0 + width(), height() - 16);    // right
		}
		glEnd();

		++fps;
		u32 fps_t1 = ew::core::time::get_ticks();
		if (fps_t1 - fps_t0 >= 1000) {
			// print fps
			snprintf(fps_str, sizeof(fps_str), "fps %d", fps);
			fps = 0;
			fps_t0 = fps_t1;
		}

		ew::graphics::color3ub col(255, 0, 255);
		ew::graphics::fonts::font_print(ft, 0, 0, width(), height(), col, fps_str);


		return true;
	}
#endif

	virtual bool on_draw(const widget_event * ev)
	{
		render();
		return true;
	}


	bool on_create(const widget_event * ev)
	{
		std::cerr << __PRETTY_FUNCTION__ <<  "\n";

		// create font
		ft = new font("/home/ceg/.ew/config/fonts/default.ttf",  font_size, font_size);

		myRect * rect = nullptr;

		gears_init();
		glDisable(GL_LIGHTING);

		std::cerr << "ft->pixel_height() = " << ft->pixel_height() << "\n";

		int rect_w = width();
		int rect_h = height() / 2; // ft->pixel_height() + 2;
		rect_h = ft->pixel_height() + 2;

		int rect_x_start = 0;
		int rect_y_start = ft->pixel_height();


		int rect_x_spacing = 1;
		int rect_y_spacing = 1;

		u32 count = 0;
		for (u32 j = rect_y_start; j < height(); j += rect_h + rect_y_spacing) {
			for (u32 i = rect_x_start; i < width(); i += rect_w + rect_x_spacing) {

				rect = new myRect(i, j, 1.0, 0.0, 0.0);

				add_widget(rect);

				rect->resize(rect_w, rect_h);

				rect->r = i * 0.003;
				rect->g = j *  0.004;
				rect->b = (i + j) * 0.007;

				++count;
				assert(count == this->number_of_children());
				if (count == 1) {
					//return true;
				}

				if ((count % 10000) == 0) {
					std::cerr << " ... " << number_of_children() << " children created\n";
				}
			}
		}

		std::cerr << " ... " << number_of_children() << " children created\n";

		return true;

		++count;
		rect = new myRect(width() - 16, 0, 0.0, 0.0, 0.0);
		rect->resize(16, height());
		add_widget(rect);

		std::cerr << " ... " << number_of_children() << " children created\n";

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
		return true;
	}

	bool  on_resize(const widget_event * ev)
	{
		return resize(ev->width, ev->height);
	}

	virtual bool on_key_press(const keyboard_event * ev)
	{

		std::cerr << __FUNCTION__
			  << "ev->x = " << ev->x << ", "
			  << "ev->y = " << ev->x << "\n";

		return true;
	}

	bool on_mouse_button_press(const button_event * ev)
	{

		// std::cerr << __FUNCTION__ << " button " << ev->button << "\n";

		widget * w = find_widget_under_coords(ev->x, ev->y); // TODO: translate event -> local_event

		if (w == get_selected_child()) {
			this->select_child(nullptr);
			return true;
		}

		if (w != this) {
			std::cerr << "w != this\n";
			bool select = w->selected_at(w->x() - ev->x, w->y() - ev->y);
			if (select == true) {
				std::cerr << "select child\n";
				this->select_child(w);
			}
		} else {
			std::cerr << "w == this\n";
			this->select_child(nullptr);
		}

		return true;
	}

	bool on_mouse_button_release(const button_event * ev)
	{

		// std::cerr << __FUNCTION__ << " button " << ev->button << "\n";

		widget * w = find_widget_under_coords(ev->x, ev->y);

		if (w == get_selected_child()) {
			this->select_child(nullptr);
			return true;
		}

		return true;
	}


	virtual bool on_pointer_motion(const pointer_event * ev)
	{

		widget * w = get_selected_child();

		//std::cerr << __PRETTY_FUNCTION__ << " move widget " << w << "...\n";

		if (w != nullptr) {

			myRect * r = static_cast<myRect *>(w);

			r->set_x(ev->x + r->sel_x);
			r->set_y(ev->y + r->sel_y);
		}

		return true;
	}

};

extern TLS_DECL GLfloat angle;

void render_scene(myWindow ** winVec, u32 nrWindow)
{
	angle += 1.0;
	for (u32 i = 0; i < nrWindow; ++i) {

		myWindow * win = winVec[i];
		if (!win)
			continue ;

		win->display()->lock();
		win->renderingContext()->lock();
		win->on_draw(0);
		win->swapBuffers();

		win->renderingContext()->unlock();
		win->display()->unlock();
	}
}

void poll_events(ew::graphics::gui::display * dpy, myWindow ** winVec, u32 nrWindow, bool * loop)
{
	dpy->poll_events(false, 1000 / 25);

	u32 nr = dpy->get_event_dispatcher()->get_queue_size();
	if (nr) {
		dpy->get_event_dispatcher()->dispatch_events(nr);
	}
}

int main(int ac, char ** av)
{

	cerr << "usage : " << av[0] << " [text] [font_size] [nrWindow] [[width height]]]\n";
	cerr << " ac =  " << ac << "\n";

	cp_array = new s32[2000];

	ac--;
	av++;
	if (ac >= 1) {
		// get text to display
		// todo: decode utf8 buffer into s32 code points array
		cerr << "copy text\n";
		for (int i = 0; av[0][i] != '\0'; ++i) {
			cp_array[i] = (s32)(av[0][i]);

			std::cerr << "cp_array[" << i << "] = " <<   cp_array[i] << "\n";
			++nr_cp;
		}

		ac--;
		av++;
	} else {

		const char * text = "Hello World !\n";
		cerr << "copy text\n";
		for (int i = 0; text[i] != '\0'; ++i) {
			cp_array[i] = (s32)(text[i]);

			std::cerr << "cp_array[" << i << "] = " <<   cp_array[i] << "\n";
			++nr_cp;
		}
	}


	if (ac >= 1) {
		font_size = in_range<u32>(1, ::atoi(av[0]), 256);
		ac--;
		av++;
	}


	if (ac >= 1) {
		nrWindow = in_range<u32>(1, ::atoi(av[0]), 1000);
		ac--;
		av++;
	}

	if (ac >= 2) {
		width = in_range<u32>(MIN_WIDTH, ::atoi(av[0]), MAX_WIDTH);
		height = in_range<u32>(MIN_HEIGHT, ::atoi(av[1]), MAX_HEIGHT);
		ac -= 2;
		av += 2;
	}

	// utf8::decode();


	// init gui
	ew::core::time::init();
	ew::graphics::gui::init();
	ew::graphics::gui::setSinglethreadEventPollingMode();   // select event polling thread model :-)
	ew::graphics::rendering::init();
	ew::graphics::fonts::init();

	// alloc display before creating any widget
	dpy = new ew::graphics::gui::display();
	if (dpy->open() == false)
		return 1;

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
		prop.use_offscreen_buffer = true;
		prop.clear_color = color4ub(0x0, 0xff, 0x0, 0xff);

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
		render_scene(winVec, nrWindow);
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
