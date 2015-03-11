#pragma once

// X11 specific
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

// Dll

#include <ew/core/dll/dll.hpp>
#include <ew/core/exception/exception.hpp>

#include <ew/core/wrapper/wrapper.hpp>

namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::exceptions;

bool load_libX11_dll();
bool unload_libX11_dll();

DECLARE_wrapper(int , XGetErrorText, ::Display * display, int code, char * buffer_return, int length);

DECLARE_wrapper(::Display *, XOpenDisplay, char * display_name);

DECLARE_wrapper(int , XCloseDisplay , ::Display * display);

DECLARE_wrapper(void , XFlushGC, ::Display *, GC);

DECLARE_wrapper(Status , XInitThreads , void);

DECLARE_wrapper(int , XWindowEvent, ::Display * display, ::Window w, long event_mask, ::XEvent * event_return);

DECLARE_wrapper(int , XUnmapWindow , ::Display * display, ::Window w);

DECLARE_wrapper(int , XUnmapSubwindows, ::Display * display, ::Window w);

DECLARE_wrapper(int , XPutBackEvent , ::Display * display, XEvent * event);

DECLARE_wrapper(Bool, XCheckWindowEvent, ::Display * display, ::Window w, long event_mask, ::XEvent * event_return);

DECLARE_wrapper(int , XFlush, ::Display * display);

DECLARE_wrapper(int , XSync, ::Display * display, Bool discard);

DECLARE_wrapper(int , XDestroyWindow , ::Display * display, ::Window w);

DECLARE_wrapper(::Colormap, XCreateColormap, ::Display * display, ::Window w, Visual * visual, int alloc);

DECLARE_wrapper(::Window , XCreateWindow, ::Display * display, ::Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int border_width, int depth, unsigned int _class, Visual * visual, unsigned long valuemask, ::XSetWindowAttributes * attributes);



DECLARE_wrapper(Status , XSetWMProtocols , ::Display * display, ::Window w, ::Atom * protocols, int count);

DECLARE_wrapper(::XSizeHints *, XAllocSizeHints, void);

DECLARE_wrapper(int , XFree, void * data);

DECLARE_wrapper(int , XSetNormalHints, ::Display *, ::Window, ::XSizeHints *);

DECLARE_wrapper(int , XSetStandardProperties, ::Display *, ::Window   , const char *, const char * , ::Pixmap, char **, int , ::XSizeHints *);


DECLARE_wrapper(Atom, XInternAtom, ::Display * display, char * atom_name, Bool only_if_exists);

DECLARE_wrapper(int , XChangeProperty , ::Display * display, ::Window w, ::Atom property, ::Atom type, int format, int mode, unsigned char * data, int nelements);

DECLARE_wrapper(int , XMapWindow, ::Display * display, ::Window w);

DECLARE_wrapper(int , XMoveWindow, ::Display * display, ::Window w, int x, int y);

DECLARE_wrapper(int , XResizeWindow , ::Display * display, ::Window w, unsigned width, unsigned height);

DECLARE_wrapper(int , XNextEvent, ::Display * display, ::XEvent * event_return);

DECLARE_wrapper(int , XPending, ::Display * display);


DECLARE_wrapper(int , XSetErrorHandler, int ( *)(::Display *, ::XErrorEvent *));

DECLARE_wrapper(char *, XDisplayName, char *);

DECLARE_wrapper(int , XSetIOErrorHandler, int ( *)(::Display *));

DECLARE_wrapper(int , XGetErrorDatabaseText, ::Display * display, char * name, char * message, char * default_string, char * buffer_return, int length);

DECLARE_wrapper(int , XSetTransientForHint, ::Display * display, ::Window w, ::Window prop_window);

DECLARE_wrapper(Status , XGetTransientForHint , ::Display * display, ::Window w, ::Window * prop_window_return);

DECLARE_wrapper(int, XFreeColormap , ::Display * display, Colormap colormap);

DECLARE_wrapper(Status, XGetGeometry , ::Display * display, ::Drawable d, ::Window * root_return, int * x_return, int * y_return, unsigned int * width_return, unsigned int * height_return, unsigned int * border_width_return, unsigned int * depth_return);

DECLARE_wrapper(Status, XGetWindowAttributes , ::Display * display, ::Window w, XWindowAttributes * window_attributes_return);

DECLARE_wrapper(int, XMoveResizeWindow , ::Display * display, ::Window w, int x, int y, unsigned width, unsigned height);

DECLARE_wrapper(int, XGrabPointer, ::Display * display, ::Window grab_window, Bool owner_events, unsigned int event_mask, int pointer_mode, int keyboard_mode, ::Window confine_to, Cursor cursor, Time time);

DECLARE_wrapper(int, XUngrabPointer, ::Display * display, Time time);

DECLARE_wrapper(int, XChangeActivePointerGrab, ::Display * display, unsigned int event_mask, Cursor cursor, Time time);

DECLARE_wrapper(int, XAllowEvents , ::Display * display, int event_mode, Time time);

DECLARE_wrapper(::Window, XCreateSimpleWindow, ::Display * display, ::Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int border_width, unsigned long border, unsigned long background);

DECLARE_wrapper(int, XChangeWindowAttributes, ::Display * display, ::Window w, unsigned long valuemask, XSetWindowAttributes * attributes);

DECLARE_wrapper(int, XSetInputFocus, ::Display * display, ::Window focus, int revert_to, ::Time time);

DECLARE_wrapper(void,  XLockDisplay, ::Display * display);

DECLARE_wrapper(void,  XUnlockDisplay, ::Display * display);

DECLARE_wrapper(::Pixmap, XCreatePixmap, ::Display * display, Drawable d, unsigned int width, unsigned int height, unsigned int depth);

DECLARE_wrapper(int, XFreePixmap, ::Display * display, ::Pixmap pixmap);

DECLARE_wrapper(int, XCopyArea, ::Display * display, ::Drawable src, ::Drawable dest, ::GC gc, int src_x, int src_y, unsigned int width, unsigned height, int dest_x, int dest_y);

DECLARE_wrapper(int, XSetWindowBackgroundPixmap , ::Display * display, ::Window w, ::Pixmap background_pixmap);

DECLARE_wrapper(int, XFillRectangle, ::Display * display, ::Drawable d, GC gc, int x, int y, unsigned int width, unsigned int height);

DECLARE_wrapper(XVisualInfo *, XGetVisualInfo, ::Display * display, long vinfo_mask, XVisualInfo * vinfo_template, int * nitems_return);

DECLARE_wrapper(Status , XMatchVisualInfo , ::Display * display, int screen, int depth, int c_class, XVisualInfo * vinfo_return);

DECLARE_wrapper(VisualID, XVisualIDFromVisual, ::Visual * visual);

DECLARE_wrapper(XIM, XOpenIM, ::Display * display, XrmDatabase db, char * res_name, char * res_class);

DECLARE_wrapper(Status, XCloseIM, XIM im);

DECLARE_wrapper(char *, XLocaleOfIM, XIM im);

DECLARE_wrapper(XIC, XCreateIC, XIM im, ...);

DECLARE_wrapper(void, XDestroyIC, XIC ic);

DECLARE_wrapper(XIM, XIMOfIC, XIC ic);

DECLARE_wrapper(int, XmbLookupString, XIC ic, XKeyPressedEvent * event, char * buffer_return, int bytes_buffer, KeySym * keysym_return, Status * status_return);

DECLARE_wrapper(int, XwcLookupString, XIC ic, XKeyPressedEvent * event, wchar_t * buffer_return, int wchars_buffer, KeySym * keysym_return, Status * status_return);

DECLARE_wrapper(int, Xutf8LookupString, XIC ic, XKeyPressedEvent * event, char * buffer_return, int bytes_buffer, KeySym * keysym_return, Status * status_return);

DECLARE_wrapper(char *, XKeysymToString, KeySym keysym);

DECLARE_wrapper(GC, XCreateGC, Display * display, Drawable d, unsigned long valuemask, XGCValues * values);

DECLARE_wrapper(int, XCopyGC, Display * display, GC src, unsigned long valuemask, GC dest);

DECLARE_wrapper(int, XChangeGC, Display * display, GC gc, unsigned long valuemask, XGCValues * values);

DECLARE_wrapper(Status, XGetGCValues, Display * display, GC gc, unsigned long valuemask, XGCValues * values_return);

DECLARE_wrapper(int, XFreeGC, Display * display, GC gc);

DECLARE_wrapper(GContext , XGContextFromGC, GC gc);

}
}
}
