#include <assert.h>
#include <list>
#include <iostream>
#include <cstdlib>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <pthread.h>

/* According to POSIX.1-2001 */
#include <sys/select.h>
#include <math.h>

#include <sys/time.h>


using std::cerr;
// --------------------

unsigned int Width = 640;
unsigned int Height = 480;

// --------------------

#ifndef M_PI
#define M_PI 3.14159265f
#endif

static struct timeval t0 = { 0, 0 };


int get_milliseconds_since_startup(void)
{
	struct timeval t1;

	gettimeofday(&t1, NULL);

	return ((t1.tv_sec - t0.tv_sec) * 1000) + ((t1.tv_usec - t0.tv_usec) / 1000);
}



void
gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
     GLint teeth, GLfloat tooth_depth)
{
	GLint i;
	GLfloat r0, r1, r2;
	GLfloat angle, da;
	GLfloat u, v, len;

	r0 = inner_radius;
	r1 = outer_radius - tooth_depth / 2.0f;
	r2 = outer_radius + tooth_depth / 2.0f;

	da = 2.0f * M_PI / teeth / 4.0f;

	glShadeModel(GL_FLAT);

	glNormal3f(0.0f, 0.0f, 1.0f);

	/* draw front face */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5f);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5f);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5f);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5f);
	}
	glEnd();


	/* draw front sides of teeth */
	glBegin(GL_QUADS);
	da = 2.0f * M_PI / teeth / 4.0f;
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;

		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5f);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5f);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5f);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5f);
	}
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);

	/* draw back face */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5f);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5f);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5f);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5f);
	}
	glEnd();


	/* draw back sides of teeth */
	glBegin(GL_QUADS);
	da = 2.0f * M_PI / teeth / 4.0f;
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;

		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5f);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5f);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5f);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5f);
	}
	glEnd();


	/* draw outward faces of teeth */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;

		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5f);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5f);
		u = r2 * cos(angle + da) - r1 * cos(angle);
		v = r2 * sin(angle + da) - r1 * sin(angle);
		len = sqrt(u * u + v * v);
		u /= len;
		v /= len;
		glNormal3f(v, -u, 0.0f);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5f);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5f);
		glNormal3f(cos(angle), sin(angle), 0.0f);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5f);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5f);
		u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
		v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
		glNormal3f(v, -u, 0.0f);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5f);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5f);
		glNormal3f(cos(angle), sin(angle), 0.0f);
	}

	glVertex3f(r1 * cos(0.0f), r1 * sin(0.0f), width * 0.5f);
	glVertex3f(r1 * cos(0.0f), r1 * sin(0.0f), -width * 0.5f);

	glEnd();


	glShadeModel(GL_SMOOTH);

	// BUG under valgrind and nvidia OpenGL version 1.5.8 NVIDIA 96.31
	/* draw inside radius cylinder */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0f * M_PI / teeth;
		GLfloat x = -cos(angle);
		GLfloat y = -sin(angle);

		//    std::cerr << "x = " << x << "\n";
		//    std::cerr << "y = " << y << "\n";
		//           x = (x < -1.0f) ? -1.0f : x;
		//           x = (x > +1.0f) ? 1.0f : x;
		//           y = (y < -1.0f) ? -1.0f : y;
		//           y = (y > +1.0f) ? +1.0f : y;
		glNormal3f(x, y, 0.0f);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5f);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5f);
	}
	glEnd();
}



GLfloat view_rotx = 20.0f, view_roty = 30.0f, view_rotz = 0.0f;
static GLint gear1, gear2, gear3;

GLfloat angle = 0.0f;

void
draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotatef(view_rotx, 1.0f, 0.0f, 0.0f);
	glRotatef(view_roty, 0.0f, 1.0f, 0.0f);
	glRotatef(view_rotz, 0.0f, 0.0f, 1.0f);

	glPushMatrix();
	glTranslatef(-3.0f, -2.0f, 0.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glCallList(gear1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.1f, -2.0f, 0.0f);
	glRotatef(-2.0f * angle - 9.0f, 0.0f, 0.0f, 1.0f);
	glCallList(gear2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.1f, 4.2f, 0.0f);
	glRotatef(-2.0f * angle - 25.0f, 0.0f, 0.0f, 1.0f);
	glCallList(gear3);
	glPopMatrix();

	glPopMatrix();
}

GLfloat pos[ 4 ] = {5.0f, 5.0f, 10.0f, 0.0f};
GLfloat red[ 4 ] = {0.8f, 0.1f, 0.0f, 1.0f};
GLfloat green[ 4 ] = {0.0f, 0.8f, 0.2f, 1.0f};
GLfloat blue[ 4 ] = {0.2f, 0.2f, 1.0f, 1.0f};

void
gears_init(void)
{
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	/* make the gears */
	gear1 = glGenLists(1);
	glNewList(gear1, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
	gear(1.0f, 4.0f, 1.0f, 20, 0.7f);
	glEndList();

	gear2 = glGenLists(1);
	glNewList(gear2, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
	gear(0.5f, 2.0f, 2.0f, 10, 0.7f);
	glEndList();

	gear3 = glGenLists(1);
	glNewList(gear3, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	gear(1.3f, 2.0f, 0.5f, 10, 0.7f);
	glEndList();

	glEnable(GL_NORMALIZE);
}


// --------------------
class EwWindow;

class EwDisplay
{
public:
	EwDisplay()
	{
		cerr << __FUNCTION__ << "\n";
		assert(XInitthreads() != 0);
		x11_dpy  = XOpenDisplay(NULL);
		assert(x11_dpy != 0);
	}

	~EwDisplay()
	{
		cerr << __FUNCTION__ << "\n";
		XCloseDisplay(x11_dpy);
	}

	Display * x11_dpy;
	int fd;
	std::list<EwWindow *> window_list;
};

class EwWindow
{
public:
	EwWindow()
	{
		use_bgpixmap = false;
		use_offscreenpixmap = true;
	}

	EwDisplay * dpy;

	int width;
	int height;

	Window win;

	bool   use_bgpixmap;
	Pixmap bgpixmap; // bg pixmap
	GLXPixmap glxbgpixmap;     // gl pixmap :-)

	bool   use_offscreenpixmap;
	Pixmap pixmap; // x11 offscreen
	GLXPixmap glxpixmap;     // gl pixmap

	XSetWindowAttributes * x11_attr;
	XVisualInfo * x11_visualInfo;

	GLXContext glxCtx;

	std::list<XEvent> event_list;
};

EwDisplay * dpy;


// map thread -> window_struct

void pushEvent(EwDisplay * dpy, XEvent * ev)
{
	//    cerr << "ev->type = " <<  ev->type << "\n";
	// filter event

	switch (ev->type) {
	case NoExpose:
		return;
	}

	//    get window
	for (std::list<EwWindow *>::iterator it = dpy->window_list.begin();
	     it != dpy->window_list.end(); it++) {
		EwWindow * eW = *it;
		if (eW->win == ev->xany.window) {
			eW->event_list.push_back(*ev);
			return;
		}
	}
}


void * dpy_thread_func(void * arg)
{
	std::cerr << __FUNCTION__ << "\n";

	EwDisplay * local_dpy = (EwDisplay *)arg;

	bool loop = true;

	while (loop) {

		int ret;
#if 1
		bool block = true;
		while (block) {
			fd_set read_fdset;
			FD_ZERO(&read_fdset);
			FD_SET(local_dpy->fd, &read_fdset);

			struct timeval tv = {
				0 , 250000
			};

			ret = select(local_dpy->fd + 1,
				     &read_fdset,
				     (fd_set *)NULL,
				     (fd_set *)NULL,
				     &tv);
			if ((ret >= 0) && FD_ISSET(local_dpy->fd, &read_fdset)) {
				break;
			}
		}
#endif
		{
			XEvent ev;

			XLockDisplay(local_dpy->x11_dpy);
			int nrEvent = XPending(local_dpy->x11_dpy);
			while (nrEvent--) {
				ret = XNextEvent(local_dpy->x11_dpy, &ev);
				pushEvent(local_dpy, &ev);
			}
			XUnlockDisplay(local_dpy->x11_dpy);
		}
	}

	return NULL;
}



void * window_thread_func(void * arg)
{
	std::cerr << __FUNCTION__ << "\n";

	int id = (int)arg;
	cerr << "thread id = " << id << "\n";

	EwWindow  * eW = new EwWindow;


	eW->width = Width;
	eW->height = Height;

	eW->dpy = dpy;
	dpy->window_list.push_back(eW);

	XLockDisplay(eW->dpy->x11_dpy);

	int double_buffer_attrib[] = {  GLX_DOUBLEBUFFER,
					GLX_RGBA,
					GLX_DEPTH_SIZE, 24,
					GLX_RED_SIZE, 8,
					GLX_GREEN_SIZE, 8,
					GLX_BLUE_SIZE, 8,
					GLX_ALPHA_SIZE, 8,
					None,
				     };

	int single_buffer_attrib[] = {
		GLX_RGBA,
		GLX_DEPTH_SIZE, 24,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		None,
	};


	int scr = DefaultScreen(dpy->x11_dpy);

	GC gc = DefaultGC(eW->dpy->x11_dpy, scr);

	Window x11_rootWindow = RootWindow(dpy->x11_dpy, scr);

	eW->dpy = dpy;

	XVisualInfo * x11_visualInfo = glXChooseVisual(dpy->x11_dpy, scr, single_buffer_attrib);
	if (! x11_visualInfo) {
		std::cerr << "Error: couldn't get an RGB, Double-buffered visual\n";
		throw "Window :: Error: couldn't get an RGB, Double-buffered visual\n";
	}
	Colormap x11_colorMap   =
		XCreateColormap(dpy->x11_dpy, x11_rootWindow, x11_visualInfo->visual, AllocNone);


	XSetWindowAttributes attr;

	attr.background_pixmap =  None; // background, None, or ParentRelative
	attr.background_pixel = 0x00000000; // must change on clear color
	attr.border_pixmap = CopyFromParent;   // border of the window or CopyFromParent
	attr.border_pixel = 10;
	attr.bit_gravity = 0;
	attr.win_gravity = 0;
	attr.backing_store = Always; //Always;     //  NotUseful, WhenMapped, Always
	attr.backing_planes = 0; /* planes to be preserved if possible */
	attr.backing_pixel = 0;  /* value to use in restoring planes */
	attr.save_under = True;         /* should bits under be saved? (popups) */

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
	attr.override_redirect = True;
	attr.save_under = True;
	attr.event_mask |= CWOverrideRedirect;

	attr.colormap = x11_colorMap;  /* color map to be associated with window */
	attr.cursor = None;            /* cursor to be displayed (or None) */

	unsigned long valuemask = 0
				  //        | CWBackPixmap
				  | CWBackPixel
				  //        | CWBorderPixmap
				  //        | CWBorderPixel
				  | CWBitGravity
				  // | CWWinGravity       // the window disapear when resizing
				  | CWBackingStore
				  // | CWBackingPlanes
				  | CWBackingPixel
				  //    | CWOverrideRedirect
				  | CWSaveUnder
				  | CWEventMask
				  //    | CWDontPropagate
				  | CWColormap
				  //    | CWCursor
				  ;

	// ----
	eW->win = XCreateWindow(
			  eW->dpy->x11_dpy,
			  x11_rootWindow,
			  0,
			  0,
			  eW->width,
			  eW->height,
			  0,
			  x11_visualInfo->depth,
			  InputOutput,
			  x11_visualInfo->visual,
			  valuemask,
			  &attr);

	// must be recreated on resize
	eW->pixmap = XCreatePixmap(dpy->x11_dpy, eW->win, eW->width, eW->height, 24);
	XFillRectangle(dpy->x11_dpy, eW->pixmap, gc, 0, 0, eW->width, eW->height);

	if (eW->use_bgpixmap) {
		eW->bgpixmap = XCreatePixmap(dpy->x11_dpy, eW->win, eW->width, eW->height, 24);
		XFillRectangle(dpy->x11_dpy, eW->bgpixmap, gc, 0, 0, eW->width, eW->height);
		XSetWindowBackgroundPixmap(dpy->x11_dpy, eW->win, eW->bgpixmap);
		eW->glxbgpixmap = glXCreateGLXPixmap(dpy->x11_dpy,  x11_visualInfo,   eW->bgpixmap);
	}

	eW->glxpixmap = glXCreateGLXPixmap(dpy->x11_dpy,
					   x11_visualInfo,
					   eW->pixmap);

	eW->glxCtx = glXCreateContext(eW->dpy->x11_dpy,
				      x11_visualInfo,
				      None,   /* GLXContext shareList */
				      True /* direct acces to device */);

	XUnlockDisplay(eW->dpy->x11_dpy);



	// ---------------------

	XLockDisplay(eW->dpy->x11_dpy);
	if (XMapWindow(eW->dpy->x11_dpy, eW->win) < 0) {

	}
	//    XFlush(eW->dpy->x11_dpy);
	XUnlockDisplay(eW->dpy->x11_dpy);

	bool loop = true;


	{
		// lock
		while (glXMakeCurrent(eW->dpy->x11_dpy, eW->glxpixmap, eW->glxCtx) != True) {
			usleep(1);
		}

		gears_init();

		GLfloat h = (GLfloat) eW->height / (GLfloat) eW->width;
		glViewport(0, 0, (GLint) eW->width, (GLint) eW->height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -40.0);
		glXMakeCurrent(eW->dpy->x11_dpy, None, NULL);
	}


	int t0 = get_milliseconds_since_startup();
	int fps = 0;
	bool have_event = false;
	XEvent ev;
	while (loop) {

		if (eW->event_list.size()) {
			ev = eW->event_list.front();
			eW->event_list.pop_front();
			have_event = true;
		}

		XLockDisplay(eW->dpy->x11_dpy);
		{
			// lock
			while (glXMakeCurrent(eW->dpy->x11_dpy, eW->glxpixmap, eW->glxCtx) != True) {
				usleep(1);
			}

			if (have_event) {
				switch (ev.type) {
				case KeyPress: {
					XKeyPressedEvent * real_ev = (XKeyPressedEvent *)&ev;
					if (real_ev->keycode == 9)
						loop = false;
				}
				break;


				case NoExpose:
					break;
				case ConfigureNotify: {
					// on_resize
					XConfigureEvent * real_ev = (XConfigureEvent *)&ev;

					int old_width = eW->width;
					int old_height = eW->height;

					eW->width = real_ev->width;
					eW->height = real_ev->height;

					glXWaitX();

					// recreate pixmap
					// glXMakeCurrent(eW->dpy->x11_dpy, None, NULL);

					glXDestroyGLXPixmap(dpy->x11_dpy, eW->glxpixmap);
					XFreePixmap(dpy->x11_dpy, eW->pixmap);

					//
					eW->pixmap = XCreatePixmap(dpy->x11_dpy, eW->win,  eW->width,  eW->height, 24);

					//  XFillRectangle(dpy->x11_dpy, eW->pixmap, gc, 0, 0, eW->width, eW->height);

					// XFillRectangle(dpy->x11_dpy, eW->pixmap, gc, old_width, 0, eW->width - old_width, eW->height);
					// XFillRectangle(dpy->x11_dpy, eW->pixmap, gc, 0, old_height, 0, eW->height - old_height);


					//
					if (eW->use_bgpixmap) {
						glXDestroyGLXPixmap(dpy->x11_dpy, eW->glxbgpixmap);
						XFreePixmap(dpy->x11_dpy, eW->bgpixmap);
						eW->bgpixmap = XCreatePixmap(dpy->x11_dpy, eW->win, eW->width, eW->height, 24);

						// XFillRectangle(dpy->x11_dpy, eW->bgpixmap, gc, 0, 0, eW->width, eW->height);
						XFillRectangle(dpy->x11_dpy, eW->bgpixmap, gc, old_width, 0, eW->width - old_width, eW->height);
						XFillRectangle(dpy->x11_dpy, eW->bgpixmap, gc, 0, old_height, 0, eW->height - old_height);


						eW->glxbgpixmap = glXCreateGLXPixmap(dpy->x11_dpy,
										     x11_visualInfo,
										     eW->bgpixmap);

						while (glXMakeCurrent(eW->dpy->x11_dpy, eW->glxbgpixmap, eW->glxCtx) != True) {
							usleep(1);
						}

						// glClearColor(0.0, 0.0, 1.0, 1.0);
						// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						// glXMakeCurrent(eW->dpy->x11_dpy, None, NULL);
						XSetWindowBackgroundPixmap(dpy->x11_dpy, eW->win, eW->bgpixmap);
					}

					//
					eW->glxpixmap = glXCreateGLXPixmap(dpy->x11_dpy,
									   x11_visualInfo,
									   eW->pixmap);

					glXWaitGL();

					while (glXMakeCurrent(eW->dpy->x11_dpy, eW->glxpixmap, eW->glxCtx) != True) {
						usleep(1);
					}

					// XClearWindow(dpy->x11_dpy, eW->win);

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					GLfloat h = (GLfloat) eW->height / (GLfloat) eW->width;
					glViewport(0, 0, (GLint)  eW->width , (GLint) eW->height);
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();
					glTranslatef(0.0, 0.0, -40.0);
				}
				break;

				default:
					//cerr << "ev.type = " << ev.type << "\n";
					break;
				}

				have_event = false;
			}

			draw();
			// if(double)
			glXSwapBuffers(eW->dpy->x11_dpy, eW->glxpixmap);
			glXMakeCurrent(eW->dpy->x11_dpy, None, NULL);
			XCopyArea(eW->dpy->x11_dpy, eW->pixmap, eW->win, gc, 0, 0, eW->width, eW->height, 0, 0);
			fps++;
		}
		XUnlockDisplay(eW->dpy->x11_dpy);

		int t1 = get_milliseconds_since_startup();
		int diff = t1 - t0;
		if (diff > 1000) {
			cerr << "fps = " << fps << "\n";
			t0 = get_milliseconds_since_startup();
			fps = 0;
		}

		usleep(1600);

		angle += 2.0;
		if (angle >= 360.0)
			angle = 0;
	}

	return NULL;
}

int main(int ac, char ** av)
{
	dpy = new EwDisplay;
	dpy->fd = ConnectionNumber(dpy->x11_dpy);
	XFlush(dpy->x11_dpy);

	cerr << "fd = " << dpy->fd << "\n";

	int nrthread = 1;
	if (ac >= 2) {
		nrthread = ::atoi(av[1]);
	}

	if (ac >= 4) {
		Width = ::atoi(av[2]);
		Height = ::atoi(av[3]);
	}

	gettimeofday(&t0, NULL);
	// run threads

	pthread_t * dpy_thread = new pthread_t;

	assert(!pthread_create(dpy_thread,
			       NULL,
			       dpy_thread_func, dpy));

	pthread_t * threads = new pthread_t [nrthread];
	for (int i = 0; i < nrthread; i++) {
		assert(!pthread_create(&threads[i],
				       NULL,
				       window_thread_func, (void *)i));
	}

	cerr << "wait for thread\n";
	for (int i = 0; i < nrthread; i++) {
		pthread_join(threads[i], NULL);
		cerr << "thread " << i << "joined\n";
	}

	delete dpy;
	return 0;
}
