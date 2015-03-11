#include <assert.h>

// rename this
#include <cstdio>
#include <cstring>

// c++ lang
#include <vector>
#include <list>

// X11 specific
//#include <X11/X.h>
//#include <X11/Xlib.h>
//#include <X11/Xatom.h>

#include "x11_libX11.hpp"
#include "x11_libGLX.hpp"


// Ew
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/mutex_locker.hpp>
#include <ew/core/threading/thread.hpp>

#include <ew/core/object/object_locker.hpp>

#include <ew/maths/functions.hpp>

#include <ew/graphics/gui/display/display.hpp>
#include <ew/graphics/gui/widget/widget.hpp>
#include <ew/graphics/gui/widget/widget_properties.hpp>

#include <ew/graphics/renderer/renderer.hpp>
#include <ew/graphics/renderer/rendering_context.hpp>

#include <ew/graphics/gui/event/event.hpp>

#include <ew/console/console.hpp>

// implementation details
#include "../renderer/rendering_context_private_data.hpp"

#include "display_private_data.hpp"
#include "window_private_data.hpp"
#include "pixmap_private_data.hpp"

#include "libX11_wrappers.hpp"
#include "x11_libGLX_wrappers.hpp"

#include "../../../../common/graphics/renderer/opengl/libGL.hpp"
#include "../../../../common/graphics/renderer/opengl/libGL_wrappers.hpp"

// TODO
/*

XSetErrorHandler, XGetErrorText, XDisplayName, XSetIOErrorHandler,
XGetErrorDatabaseText - default error handlers

donner le choix des handle d'input
threaded callback
or direct polling

window->getEvent() == RESIZE : ...

*/

// move to Core
// man glXIntro


/*
we should trace event like ClientMessage ---> bool onQuit  if true quit_was_handled
*/


namespace ew
{
namespace graphics
{
namespace gui
{

extern "C" char * __progname;


using namespace ew::core::objects;
using namespace ew::core::threading;
using namespace ew::graphics::gui::events;
using namespace ew::implementation::graphics::rendering::opengl;
using ew::console::cerr;

// move this to lib_X11.*
void display_X11_return_value(int ret)
{
	switch (ret) {
	case BadAlloc: {
		cerr << "BadAlloc" << "\n";
	}
	break ;

	case BadAtom: {
		cerr << "BadAtom" << "\n";
	}
	break ;

	case BadMatch: {
		cerr << "BadMatch" << "\n";
	}
	break ;

	case BadValue: {
		cerr << "BadValue" << "\n";
	}
	break ;

	case BadWindow: {
		cerr << "BadWindow" << "\n";
	}
	break ;

	default: {
		cerr << "no error ??" << "\n";
	}
	}
}

// -----------------------------------------------------------------------------

window::window_private_data::window_private_data(window * parent)
	:
	_parent(parent),     // class_name , getWidgetClassID
	_owner(0),
	_isInitialized(false),
	_isAvailable(false),
	_destroyAck(false),
	_inDestroyProcess(false),
	_manual_grab(false),
	_x11_dpy(0),
	_x11_rootWindow(0),     // x11 parent
	_x11_drawable(0),
	_x11_colorMap(0),
	_x11_visualInfo(0),
	_rdr_ctx(0),
	_guiDpy(0),
	_lastResize_time(0)
{
}

// this is the build_window reverse calls order
// add destroy_window();
window::window_private_data::~window_private_data()
{
	mutex_locker mTxLock(this);

	_isAvailable = false;
	_inDestroyProcess = true;

	assert(_guiDpy->is_locked_by() == ew::core::threading::self());
	_guiDpy->d->del_widget(_owner);

	// input
	if (_x11_InputContext)
		XDestroyIC(_x11_InputContext);
	_x11_InputContext = 0;

	if (_x11_InputMethod)
		XCloseIM(_x11_InputMethod);
	_x11_InputMethod = 0;

	// gfx
	delete _background_pixmap;
	_background_pixmap = 0;
	delete _offscreen_pixmap;
	_offscreen_pixmap = 0;

	// destroy rendering ctx
	delete _rdr_ctx;
	_rdr_ctx = 0;


	XDestroyWindow(_x11_dpy, _x11_drawable);

	// TODO: remove this
	// push the remaaing code in an other callback : private::on_destroy
	XSync(_x11_dpy, False);
	// wait for XDestroyEvent ack
	XFreeColormap(getX11Display(),  _x11_colorMap);
	XFree(_x11_visualInfo);

	// set ourself to 0
	_owner->d = 0;
}

/*
  must be changed this way
  windowType : topLevel , popup,  dialog ,  desktop,  tool (splash screen, tooltip )

  //get display, screen number, root window and desktop geometry for
  //the current screen
  */

bool window::window_private_data::build_Window(ew::graphics::gui::display * dpy,
		ew::graphics::gui::window * parent,
		ew::graphics::gui::window * owner,
		window_properties & properties)
{

	::Window x11_rootWindow;
	::Window x11_drawable;
	Colormap x11_colorMap;


	// cerr << " window::private_data::build_Window\n";

	// get system info
	// if parent
	// get display from parent
	// faire un internal manager
	if (! dpy) {
		cerr << "Window(...) :: error :: can't create window , no display found\n";
		throw "Window(...) :: error :: can't create window , no display found\n";
	}
	this->_guiDpy = dpy;

	assert(dpy->is_locked_by() == ew::core::threading::self());

	// copy x11 info
	_x11_dpy = _guiDpy->d->get_x11_display();
	if (! _x11_dpy) {
		// throw
		cerr << "Window(...) :: error\n";
		throw "Window(...) :: error :: can't create window , no display found (2)\n";
	}

	int scrnum = DefaultScreen(_x11_dpy);

/// SLIT HERE glx / agl / wgl

	// getRootWindowInfo ??
	// todo : add detection of server config
	// by analysing Rootwindow info
	// when display->open();
	// PixelFormatDescriptor rootWindow->getPixelFormatDescriptor();
	// TODO: move to gui::Display() init
	// fill guiDpy->d->x11_visualInfo
	XVisualInfo * x11_visualInfo = 0;
	GLXFBConfigSGIX fbconfig = NULL;

	// glX Pbuffers
	if (0 && !x11_visualInfo) {

		/*
		 * Find a frame buffer configuration that suits our needs: it must
		 * work with both the pbuffers and the window, and must have 5551 pixels,
		 * because that is the only format that can be used to capture
		 * mipmapped video.
		 */

		int params[] = {
			GLX_RENDER_TYPE_SGIX,   GLX_RGBA_BIT_SGIX,
			GLX_DRAWABLE_TYPE_SGIX, GLX_PBUFFER_BIT_SGIX,
			GLX_ALPHA_SIZE,         1,
			GLX_RED_SIZE,           1,
			GLX_GREEN_SIZE,         1,
			GLX_BLUE_SIZE,          1,
			GLX_X_RENDERABLE_SGIX,  True,
			None,
		};
		int fbconfigCount;
		GLXFBConfigSGIX * fbconfigs = glXChooseFBConfigSGIX(_x11_dpy, scrnum, params, &fbconfigCount);
		if (fbconfigs == NULL) {
			cerr << "Could not get FBConfig\n";
		} else {
			cerr << "fbconfigCount == " << fbconfigCount << "\n";
		}

		// factorize test ?
		int i;
		for (i = 0;  i < fbconfigCount;  i++) {
			int alpha;
			int red;
			int green;
			int blue;

			cerr << "--- check config ---\n";

			if (fbconfigs[i] == 0) {
				continue;
			}

			glXGetFBConfigAttribSGIX(_x11_dpy, fbconfigs[i], GLX_ALPHA_SIZE, &alpha);
			glXGetFBConfigAttribSGIX(_x11_dpy, fbconfigs[i], GLX_RED_SIZE, &red);
			glXGetFBConfigAttribSGIX(_x11_dpy, fbconfigs[i], GLX_GREEN_SIZE, &green);
			glXGetFBConfigAttribSGIX(_x11_dpy, fbconfigs[i], GLX_BLUE_SIZE, &blue);

			cerr << "alpha " << alpha << "\n";
			cerr << "red " << red << "\n";
			cerr << "green " << green << "\n";
			cerr << "blue " << blue << "\n";

			if (alpha != 8)
				continue;
			if (red != 8)
				continue;
			if (green != 8)
				continue;
			if (blue != 8)
				continue;

			cerr << "found config\n";
			break;
		}

		if (i != fbconfigCount) {
			fbconfig = fbconfigs[i];
		}
		XFree(fbconfigs);

		//
		if (fbconfig) {

			int attrib[] = {
				GLX_PRESERVED_CONTENTS_SGIX,    GL_TRUE,
//                GLX_DIGITAL_MEDIA_PBUFFER_SGIX, GL_TRUE,
				None
			};

			// TODO: new class PixelBuffers {
			// }
			//
			GLXPbuffer pbuffer = glXCreateGLXPbufferSGIX(_x11_dpy, fbconfig, 512, 256, attrib);
			if (pbuffer != 0) {
				cerr << "pbuffer Ok \n";
			}
		} else {
			cerr << "pbuffer KO \n";
		}
	}

	// glXFbconfig
	if (!x11_visualInfo) {
		int singleBufferAttributess[] = {
			GLX_DRAWABLE_TYPE, GLX_PIXMAP_BIT,
			GLX_RENDER_TYPE,   GLX_RGBA_BIT,
			GLX_RED_SIZE,      8,   /* Request a single buffered color buffer */
			GLX_GREEN_SIZE,    8,   /* with the maximum number of color bits  */
			GLX_BLUE_SIZE,     8,   /* for each component                     */
			GLX_ALPHA_SIZE,    8,
			None
		};

		int doubleBufferAttributes[] = {
			GLX_DRAWABLE_TYPE, GLX_PIXMAP_BIT,
			GLX_RENDER_TYPE,   GLX_RGBA_BIT,
			GLX_DOUBLEBUFFER,  True,  /* Request a double-buffered color buffer with */
			GLX_RED_SIZE,      8,     /* the maximum number of bits per component    */
			GLX_GREEN_SIZE,    8,
			GLX_BLUE_SIZE,     8,
			GLX_ALPHA_SIZE,    8,
			None
		};

		int numReturned;
		GLXFBConfig     *     fbConfigs = NULL;

		if (properties.use_offscreen_buffer == false) {
			fbConfigs = glXChooseFBConfig(_x11_dpy, scrnum, doubleBufferAttributes, &numReturned);
			if (fbConfigs)
				haveDoubleBuffer = true;
		}

		if (fbConfigs == NULL) {    /* no double buffered configs available */
			fbConfigs = glXChooseFBConfig(_x11_dpy, DefaultScreen(_x11_dpy), singleBufferAttributess, &numReturned);
			haveDoubleBuffer = false;
		}
		/* Create an X colormap and window with a visual matching the first
		** returned framebuffer config */
		if (fbConfigs) {
			x11_visualInfo = glXGetVisualFromFBConfig(_x11_dpy, fbConfigs[0]);
		}
	}

	// read default
	if (!x11_visualInfo) {

		int nitems_return;
		long vinfo_mask = VisualIDMask | VisualScreenMask;

		XVisualInfo vinfo_template;
		::memset(&vinfo_template, 0, sizeof(XVisualInfo));
		vinfo_template.visualid = XVisualIDFromVisual(DefaultVisual(_x11_dpy, scrnum));
		vinfo_template.screen = scrnum;

		x11_visualInfo = XGetVisualInfo(_x11_dpy, vinfo_mask, &vinfo_template, &nitems_return);
		if (!x11_visualInfo) {
			cerr << "cannot detect root window configuration , nitems_return = " << nitems_return << "\n";
			throw "error";
		}
	}

	// glX classic init
	if (!x11_visualInfo) {
		int use_gl;
		int glx_level;
		int glx_rgba;

		struct {
			const char * attrib_name;
			int attrib;
			int * val;
		} glxAttribute[] = {
			{ "GLX_USE_GL",  GLX_USE_GL, &use_gl },
			{ "GLX_BUFFER_SIZE", GLX_BUFFER_SIZE, &pfd.bpp },
			{ "GLX_LEVEL", GLX_LEVEL, &glx_level },
			{ "GLX_RGBA", GLX_RGBA, &glx_rgba },
			{ "GLX_DOUBLEBUFFER", GLX_DOUBLEBUFFER, &haveDoubleBuffer },
			// GLX_STEREO
			// GLX_AUX_BUFFERS
			{ "GLX_RED_SIZE", GLX_RED_SIZE, &pfd.redSize },
			{ "GLX_GREEN_SIZE", GLX_GREEN_SIZE, &pfd.greenSize },
			{ "GLX_BLUE_SIZE", GLX_BLUE_SIZE, &pfd.blueSize },
			{ "GLX_ALPHA_SIZE", GLX_ALPHA_SIZE, &pfd.alphaSize },
			{ "GLX_DEPTH_SIZE", GLX_DEPTH_SIZE, &pfd.depthSize },
			// GLX_STENCIL_SIZE
			// GLX_ACCUM_RED_SIZE
			// GLX_ACCUM_GREEN_SIZE
			// GLX_ACCUM_BLUE_SIZE
			// GLX_ACCUM_ALPHA_SIZE
		};

		int error = 0;
		for (u32 i = 0; i < sizeof(glxAttribute) / sizeof(glxAttribute[0]); i++) {
			int ret;
			int res;
			ret = glXGetConfig(_x11_dpy, x11_visualInfo, glxAttribute[i].attrib, &res);
			if (ret != 0) {
				error = 1;
				break;
			}

			*(glxAttribute[i].val) = res;
			cerr << glxAttribute[i].attrib_name << " = '" << *(glxAttribute[i].val) << "'\n";
		}

		if (error) {
			throw "cannot read gfx config\n";
		}

		// renderer::bufferAttribute renderer->getBufferAttribute()
		// pfd.doubleBuffer
		// pfd.rgba; // format ? RGB RGBA ARGB BGR
		int i = 0;
		int attrib[32];
		// attrib[i++] = GLX_DOUBLEBUFFER;
		// attrib[i++] = glx_doublebuffer ? True : False;
		// attrib[i++] = GLX_RGBA;
		// attrib[i++] = glx_rgba ? True : False;
		attrib[i++] = GLX_DEPTH_SIZE;
		attrib[i++] = pfd.depthSize;
		attrib[i++] = GLX_RED_SIZE;
		attrib[i++] = pfd.redSize;
		attrib[i++] = GLX_GREEN_SIZE;
		attrib[i++] = pfd.greenSize;
		attrib[i++] = GLX_BLUE_SIZE;
		attrib[i++] = pfd.blueSize;
		attrib[i++] = GLX_ALPHA_SIZE;
		attrib[i++] = pfd.alphaSize;
		attrib[i++] = None;

		// TODO: add glx cl
		x11_visualInfo = glXChooseVisual(_x11_dpy, DefaultScreen(_x11_dpy), attrib);
	}

	if (!x11_visualInfo) {
		const char * error_msg = "cannot found any good visual\n";
		cerr << error_msg;
		throw error_msg;
	}

	//
	x11_rootWindow = RootWindow(getX11Display(), scrnum);

	// PixelFormat
	/* allocate color map */
	x11_colorMap = XCreateColormap(getX11Display(), x11_rootWindow, x11_visualInfo->visual, AllocNone);

	/*
	  add widget->setClearColor(Color3f & col);
	  int XSetWindowBackground(Display *display, Window w, unsigned long background_pixel);
	  and chage x11 window attr
	  class renderer
	*/
	/* window attributes : put in data ? */
	XSetWindowAttributes attr;

	attr.background_pixmap = None; // background, None, or ParentRelative

	// TODO: use X11 macros for endiannes
	attr.background_pixel =
		properties.clear_color.a << 24 |
		properties.clear_color.r << 16 |
		properties.clear_color.g << 8  |
		properties.clear_color.b << 0  |
		0;

	attr.border_pixmap = CopyFromParent;   // border of the window or CopyFromParent
	attr.border_pixel = 0;
	attr.bit_gravity = NorthWestGravity;
	attr.win_gravity = 0;

	attr.backing_store = Always; //Always;     //  NotUseful, WhenMapped, Always

	attr.backing_planes = 0; /* planes to be preserved if possible */
	attr.backing_pixel = 0;  /* value to use in restoring planes */

	attr.save_under = False;         /* should bits under be saved? (popups) */

	attr.event_mask = 0
			  | KeyPressMask
			  | KeyReleaseMask
			  | ButtonPressMask
			  | ButtonReleaseMask
			  | KeymapStateMask
			  | EnterWindowMask
			  | LeaveWindowMask
			  | PointerMotionMask
			  | ButtonMotionMask
			  | ExposureMask
			  | StructureNotifyMask
			  | FocusChangeMask
			  | PropertyChangeMask
			  | 0;

	attr.do_not_propagate_mask = 0;  /* set of events that should not propagate */
	attr.override_redirect = False;  /* boolean value for override_redirect */
	if (parent) {
		attr.override_redirect = True;
		attr.save_under = True;
		attr.event_mask |= CWOverrideRedirect;
	}

	attr.colormap = x11_colorMap;  /* color map to be associated with window */
	attr.cursor = None;            /* cursor to be displayed (or None) */


	// FIXME:
#if 0
	::Window parent_win_id = (parent != 0) ? parent->d->_x11_drawable : x11_rootWindow;
	parent_win_id = x11_rootWindow;
#endif


	unsigned long mask = 0
			     // | CWBackPixmap
			     | CWBackPixel
			     // | CWBorderPixmap
			     // | CWBorderPixel
			     | CWBitGravity
			     // | CWWinGravity       // the window disapear when resizing
			     | CWBackingStore
			     | CWBackingPlanes
			     | CWBackingPixel
			     //    | CWOverrideRedirect
			     | CWSaveUnder
			     | CWEventMask
			     //    | CWDontPropagate
			     | CWColormap
			     //    | CWCursor
			     ;

	// unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap;  // qt one's

	// add to list of windows : this enable events
	//        _guiDpy->d->addWidget(owner);

	x11_drawable =
		XCreateWindow(getX11Display(),
			      x11_rootWindow,   // parent_win_id, /* for transient window we need more info */
			      properties.x,
			      properties.y,
			      properties.width,
			      properties.height,
			      0,
			      x11_visualInfo->depth,
			      InputOutput,
			      x11_visualInfo->visual,
			      mask,
			      &attr);

	XSync(getX11Display(), False);

	// ???
	//     attr.bit_gravity = true ? NorthEastGravity : NorthWestGravity;
	//     XChangeWindowAttributes(_x11_dpy, x11_drawable, CWBitGravity, &attr);

	//          XSelectInput( getX11Display(),  x11_drawable, FocusChangeMask | KeyPressMask |
	//          KeyReleaseMask | PropertyChangeMask | StructureNotifyMask | KeymapStateMask);

	// set client leader property
	XChangeProperty(_x11_dpy, x11_drawable, _guiDpy->d->_WM_CLIENT_LEADER,
			XA_WINDOW, 32, PropModeReplace,
			(unsigned char *)&x11_drawable, 1);

	XChangeProperty(getX11Display(),
			x11_drawable, _guiDpy->d->_WM_DELETE_WINDOW,
			XA_WINDOW, 32, PropModeReplace,
			(unsigned char *)&x11_drawable, 1);

	/* This Allow us to catch a window close event from the window manager */
	XSetWMProtocols(getX11Display(), x11_drawable, &_guiDpy->d->_WM_DELETE_WINDOW, 1);

	// --------------------------------------------------------------------------------

	/* window : set hints and properties */
	{
		XSizeHints * sizehints = XAllocSizeHints();
		if (! sizehints) {
			::fprintf(stderr, "XAllocSizeHints() :: error\n");
			return false;
		}

		//  resizeAble, fullscreen
		// sizehints->x = properties.x; // obsolete
		// sizehints->y = properties.y; // obsolete
		sizehints->min_aspect.x = properties.x;
		sizehints->min_aspect.y = properties.y;
		sizehints->max_aspect.x = properties.x;
		sizehints->max_aspect.y = properties.y;
		// sizehints->width  = properties.width; // obsolete
		// sizehints->height = properties.height; // obsolete
		sizehints->min_width = properties.width;
		sizehints->min_height = properties.height;
		sizehints->max_width = properties.width;
		sizehints->max_height = properties.height;

		if (properties.is_resizeable == true) {
			sizehints->min_width = 1;
			sizehints->min_height = 1;
			sizehints->max_width = 4096;
			sizehints->max_height = 4096;
		}

		// warning me must place
		sizehints->flags = USSize | PMaxSize | PMinSize;

		if (parent)
			sizehints->flags = USPosition | PMaxSize | PMinSize;
		// menu are relative

		XSetNormalHints(getX11Display(), x11_drawable, sizehints);

		XSetStandardProperties(getX11Display(), x11_drawable,
				       "Ew glx app", "Ew glx app", None, (char **) NULL,
				       0, sizehints);
		XFree(sizehints);

		// --------------------------------------------------------------------------------
		// NO BORDER/DECORATION CODE

		// kwin : remove decoration
		if (properties.have_decoration == false) {
			Atom WM_HINTS;

			/* First try to set MWM hints */
			WM_HINTS = XInternAtom(getX11Display(), const_cast<char *>("_MOTIF_WM_HINTS"), True);
			if (WM_HINTS != None) {
				/* Hints used by Motif compliant window managers */
				struct {
					unsigned long flags;
					unsigned long functions;
					unsigned long decorations;
					long input_mode;
					unsigned long status;
				}
				MWMHints = {(1L << 1), 0, 0, 0, 0 };

				XChangeProperty(getX11Display(), x11_drawable,
						WM_HINTS, WM_HINTS, 32,
						PropModeReplace,
						(unsigned char *) & MWMHints,
						sizeof(MWMHints) / sizeof(long));
			}
		}


		XSync(getX11Display(), False);

		// ------------------------------------------------------------
		if (true) {
			::Window pwid = x11_rootWindow;
			if (parent)
				pwid = parent->d->_x11_drawable;

			if (parent)
				XSetTransientForHint(getX11Display(), x11_drawable, pwid);

			// menu window, tooltip, etc , we must have an exclusive state
			if (pwid != x11_rootWindow) {
				XSetWMProtocols(getX11Display(), x11_drawable, &_guiDpy->d->_NET_WM_WINDOW_TYPE_MENU, 1);

				int ret = XChangeProperty(getX11Display(), x11_drawable,
							  _guiDpy->d->_NET_WM_WINDOW_TYPE,
							  XA_ATOM,
							  32,
							  PropModeReplace,
							  (unsigned char *) & (_guiDpy->d->_NET_WM_WINDOW_TYPE_MENU),
							  sizeof(_guiDpy->d->_NET_WM_WINDOW_TYPE_MENU) / sizeof(long));

				cerr << "XChangeProperty" << "\n";
				display_X11_return_value(ret);
			}
		}
	}

	// XSync(getX11Display(), False);

	// remove this to use directly concerned vars
	// copy all tmp var // we should use references
	_properties = properties;
	_parent = parent;
	_owner = owner;
	_x11_visualInfo = x11_visualInfo;
	_x11_rootWindow = x11_rootWindow;
	_x11_drawable = x11_drawable;
	_x11_colorMap = x11_colorMap;

	// rendering Context
	// is use_offscreen_pixmap property is set
	_offscreen_pixmap = 0;
	if (properties.use_offscreen_buffer == true) {
		_offscreen_pixmap = new ew::graphics::gui::pixmap(owner);
	}

	_background_pixmap = 0;
	//
	// _background_pixmap = new ew::graphics::gui::Pixmap(owner);
	// XSetWindowBackgroundPixmap(getX11Display(), _x11_drawable, _background_pixmap->d->_x11_pixmap);

	// rendering Context
	_rdr_ctx = new ew::graphics::rendering::rendering_context(owner);

	// Input
	char x11_class_name[64] = {0};
	::strcpy(x11_class_name, __progname);
	_x11_InputMethod = XOpenIM(_x11_dpy, NULL, x11_class_name, x11_class_name);
	if (_x11_InputMethod) {

		// cerr << "LOCALE = " << XLocaleOfIM(_x11_InputMethod) << "\n";

		_x11_InputContext = XCreateIC(_x11_InputMethod,
					      XNClientWindow, _x11_drawable,
					      XNFocusWindow, _x11_drawable,
					      XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
					      XNResourceName, x11_class_name,
					      XNResourceClass, x11_class_name,
					      NULL);
	}

	if (!_x11_InputMethod) {
		cerr  << "no input method found\n";
	}
	if (!_x11_InputContext) {
		cerr  << "no input context created\n";
	}

	// if (properties.initial_position)
	//  owner->move( properties.x, properties.y );

	_isAvailable = true;
	_isInitialized = true;

	_guiDpy->d->add_widget(owner);

	// push WidgetCreateEvent
	{
		struct widget_event * ev =  new struct widget_event();
		ev->type = WidgetCreateEvent;
		ev->widget = owner;
		ev->display = owner->display();
		ev->time = ew::core::time::get_ticks(); // TODO: move to push ??

		ev->width  = properties.width;
		ev->height = properties.height;

		ev->display->push_events((events::event **)&ev, 1);
	}

	return true;
}

// -------------------------------------------------------------------------------

/*
  we must register all windows of the display
*/

window::window(ew::graphics::gui::display * dpy, ew::graphics::gui::window * parent, window_properties & properties)
	:
	d(new window_private_data(/* TODO dpy, */ parent))
{
	d->build_Window(dpy, parent, this, properties);
}

// ---------------------

// dtor
window::~window()
{
	delete d;
}

ew::graphics::gui::display * window::display() const
{
	return d->_guiDpy;
}

const char * window::class_name() const
{
	return "Window";
}

const char * window::getName() const
{
	return "X11_Window";
}

//ew::core::object | Window
bool window::lock()
{
	return d->lock();
}

bool window::trylock()
{
	return d->trylock();
}

bool window::unlock()
{
	return d->unlock();
}

bool window::isAvailable()
{
	if (d)
		return d->_isAvailable;
	return false;
}

ew::graphics::rendering::rendering_context * window::renderingContext() const
{
	return d->_rdr_ctx;
}

ew::graphics::rendering::renderer * window::renderer() const
{
	return 0;
}

bool window::trylockDrawingContext()
{
	return d->_rdr_ctx->trylock();
}

bool window::lockDrawingContext()
{
	return d->_rdr_ctx->lock();
}

bool window::unlockDrawingContext()
{
	return d->_rdr_ctx->unlock();
}

//  will be renamed flushBuffers()
bool window::swapBuffers()
{
	//  glFlush();
	if (d->_offscreen_pixmap) {

		// offscreen->copyToWindow(this, x, y, w, h)

		// switch offscreen->d->type()
		// OFFSCCREEN_TYPE_GLXPIXMAP
		// OFFSCCREEN_TYPE_FBO
		// OFFSCCREEN_TYPE_PBUFFER

		int scr = DefaultScreen(d->getX11Display()); // TODO: move to d->x11_screen_num
		// GC gc = DefaultGC(d->getX11Display(), scr); // TODO: move to d->_x11_gc

		// TODO: build this in offscren_pixmap ctor
		GC gc = XCreateGC(d->getX11Display(), RootWindow(d->getX11Display(), scr), 0, NULL);
		XGCValues gc_values;
		gc_values.graphics_exposures = False;
		XChangeGC(d->getX11Display(), gc, GCGraphicsExposures, &gc_values);

		XCopyArea(d->getX11Display(),
			  d->_offscreen_pixmap->d->_x11_pixmap,
			  d->_x11_drawable,
			  gc, 0, 0, width(), height(), 0, 0);

		XFreeGC(d->getX11Display(), gc);
	}


	if (d->haveDoubleBuffer) {
		glXSwapBuffers(d->getX11Display(), d->_x11_drawable);

	}
	glFinish();

	// glXWaitX();
	// glXWaitGL();

	return true;
}

bool window::show()
{
	XMapWindow(d->getX11Display(), d->_x11_drawable);
	return true;
}

bool window::hide()
{
	XUnmapWindow(d->getX11Display(), d->_x11_drawable);
	return true;
}

s32 window::xPos()
{
	return d->_properties.x;
}

s32 window::yPos()
{
	return d->_properties.y;
}

u32 window::width() const
{
	return d->_properties.width;
}

u32 window::height() const
{
	return d->_properties.height;
}

bool window::move(s32 x, s32 y)
{
	// int ret =
	// must test loc
	// XLockDisplay( d->getX11Display() );
	XMoveWindow(d->getX11Display(), d->_x11_drawable, x, y);
	// XUnlockDisplay( d->getX11Display() );

	return true;
}

bool window::on_create(const widget_event * ev)
{
	cerr << __FUNCTION__ << "\n";
	assert(0);
	return true;
}

bool window::on_destroy(const widget_event * ev)
{
	return true;
}

bool window::on_draw(const widget_event * ev)
{
	return true;
}

bool window::on_move(const widget_event * ev)
{
	return true;
}

bool window::resize(u32 width, u32 height)
{
	// validate input
	if (!width || !height)
		return false;

	// wrapper
	XLockDisplay(d->getX11Display());

	int ret = XResizeWindow(d->getX11Display(), d->_x11_drawable, width, height);

	XUnlockDisplay(d->getX11Display());

	if (ret != 0) {   // bad window or bad value
		if (ret == BadValue) {
			cerr << "error resize : bad value\n";
			return false;
		}
		if (ret == BadWindow) {
			cerr << "error resize : bad value\n";
			return false;
		}
	}

	XSync(d->getX11Display(), False);

	// update window geometry
	d->_properties.width = width;
	d->_properties.height = height;
	//         d->_properties.x = x;
	//         d->_properties.y = y;

	// we should check private data min/max defaults
	//  width  = ew::maths::max((u32)32,   width); // width = Clip( _width, 32, 4096);
	//  width  = ew::maths::min((u32)4096, width);
	//  height = ew::maths::max((u32)32,   height);
	//  height = ew::maths::min((u32)4096, height);

	widget::resize(width, height);


	return true;
}

bool window::setXPos(s32 x)
{
	return true;
}

bool window::setYPos(s32 y)
{
	return true;
}

bool window::setWidth(u32 w)
{
	return true;
}

bool window::setHeight(u32 h)
{
	return true;
}


// move this to Window
bool window::on_resize(const widget_event * ev)
{
	return true;
}

bool window::on_close(const widget_event * ev)
{
	cerr << "window::on_close()\n";
	return true;
}

// move to Window
bool window::on_key_press(const keyboard_event * ev)
{
	return false;
}

bool window::on_key_release(const keyboard_event * ev)
{
	return false;
}

bool window::on_mouse_button_press(const button_event * ev)
{
	return false;
}

bool window::on_mouse_button_release(const button_event * ev)
{
	return false;
}

bool window::on_pointer_motion(const pointer_event * ev)
{
	//    cerr << "  window::on_pointer_motion("<< x << "," << y << ")\n";
	return false;
}

bool window::on_pointer_enter(const pointer_event * ev)
{
	return true;
}

bool window::on_pointer_leave(const pointer_event * ev)
{
	return true;
}

bool window::on_focus_in(const widget_event * ev)
{
	return true;
}

bool window::on_focus_out(const widget_event * ev)
{
	return true;
}

} // ! namespace gui {
} // ! namespace graphics {
} // ! namespace ew {
