#pragma once

#include <GL/glx.h>
#include <GL/glxext.h>


// Dll

#include <ew/core/dll/dll.hpp> // TODO: remove ?
#include <ew/core/exception/exception.hpp> // TODO::remove ?

// wrapper helpers
#include <ew/core/wrapper/wrapper.hpp>

//TODO: update namespace + X11::GLX ?
namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::exceptions;

EW_GRAPHICS_RENDERING_OPENGL_EXPORT bool load_libGLX_dll();
EW_GRAPHICS_RENDERING_OPENGL_EXPORT bool unload_libGLX_dll();

//  GLX parts
typedef void(*glXGetProcAddress_ret_t)();
DECLARE_wrapper(glXGetProcAddress_ret_t, glXGetProcAddress, const GLubyte * procName);

DECLARE_wrapper(int, glXGetConfig, ::Display * dpy, ::XVisualInfo * vis, int attrib, int * value);

DECLARE_wrapper(GLXContext , glXCreateContext, ::Display * dpy, ::XVisualInfo * vis, ::GLXContext shareList, Bool direct);

DECLARE_wrapper(void , glXDestroyContext, ::Display * dpy, ::GLXContext ctx);

DECLARE_wrapper(::XVisualInfo *, glXChooseVisual, ::Display * dpy, int screen, int * attribList);

DECLARE_wrapper(Bool , glXMakeCurrent, ::Display * dpy, ::GLXDrawable drawable, ::GLXContext ctx);

DECLARE_wrapper(void, glXSwapBuffers, ::Display * dpy, ::GLXDrawable drawable);

DECLARE_wrapper(void, glXWaitGL, void);

DECLARE_wrapper(void, glXWaitX, void);

DECLARE_wrapper(::GLXPixmap, glXCreateGLXPixmap, Display * dpy, XVisualInfo * vis, ::Pixmap pixmap);

DECLARE_wrapper(void, glXDestroyGLXPixmap, ::Display * dpy, ::GLXPixmap pix);

DECLARE_wrapper(Bool, glXQueryVersion, ::Display * dpy, int * major, int * minor);

// glx >= 1.3
DECLARE_wrapper(GLXFBConfig *, glXChooseFBConfig, ::Display * dpy, int screen, const int * attrib_list, int * nelements);
DECLARE_wrapper(XVisualInfo *, glXGetVisualFromFBConfig, ::Display * dpy, GLXFBConfig config);

// SGIX fbconfig
DECLARE_wrapper(int, glXGetFBConfigAttribSGIX, ::Display * dpy, GLXFBConfigSGIX config, int attribute, int * value);
DECLARE_wrapper(GLXFBConfigSGIX *, glXChooseFBConfigSGIX, ::Display * dpy, int screen, const int * attrib_list, int * nelements);
DECLARE_wrapper(GLXPixmap, glXCreateGLXPixmapWithConfigSGIX, ::Display * dpy,  GLXFBConfig config, Pixmap pixmap);
DECLARE_wrapper(GLXContext, glXCreateContextWithConfigSGIX, ::Display * dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct);
DECLARE_wrapper(XVisualInfo *, glXGetVisualFromFBConfigSGIX, ::Display * dpy, GLXFBConfig config);
DECLARE_wrapper(GLXFBConfigSGIX, glXGetFBConfigFromVisualSGIX, ::Display * dpy, XVisualInfo * vis);

// SGIX pbuffer (SGIX_FBConfig is required )
DECLARE_wrapper(int, glXGetFBConfigAttrib, ::Display * dpy, GLXFBConfig config, int attribute, int * value);
DECLARE_wrapper(GLXPbuffer, glXCreateGLXPbufferSGIX, Display * dpy, GLXFBConfig config, unsigned int width, unsigned int height, int * attrib_list);
DECLARE_wrapper(void, glXDestroyGLXPbufferSGIX, Display * dpy, GLXPbuffer pbuf);
DECLARE_wrapper(void, glXQueryGLXPbufferSGIX, Display * dpy, GLXPbuffer pbuf, int attribute, unsigned int * value);
DECLARE_wrapper(void, glXSelectEventSGIX, Display * dpy, GLXDrawable drawable, unsigned long mask);
DECLARE_wrapper(void, glXGetSelectedEventSGIX, Display * dpy, GLXDrawable drawable, unsigned long * mask);


// TODO: merge libGL + libGLwindowSystem ( GLX, WGL, AGL ... )
// detect gl version and load ervery extension supported
// create glWindowsSystemGetProccAddr() wrapper to XXXgetProcAddr

// FBO
DECLARE_wrapper(void, glGenFramebuffersEXT, GLsizei n, GLuint * ids);
DECLARE_wrapper(void, glDeleteFramebuffersEXT, GLsizei n, const GLuint * ids);
DECLARE_wrapper(void, glBindFramebufferEXT, GLenum target, GLuint id);
// Render Buffer
DECLARE_wrapper(void, glGenRenderbuffersEXT, GLsizei n, GLuint * ids);
DECLARE_wrapper(void, glDeleteRenderbuffersEXT, GLsizei n, const GLuint * ids);
DECLARE_wrapper(void, glBindRenderbufferEXT, GLenum target, GLuint id);
DECLARE_wrapper(void, glRenderbufferStorageEXT, GLenum target, GLenum internalFormat, GLsizei width, GLsizei height);
DECLARE_wrapper(void, glGetRenderbufferParameterivEXT, GLenum target, GLenum param, GLint * value);
//
DECLARE_wrapper(void, glFramebufferRenderbufferEXT, GLenum target, GLenum attachmentPoint, GLenum renderbufferTarget, GLuint renderbufferId);
//
DECLARE_wrapper(void, glFramebufferTexture2DEXT, GLenum target, GLenum attachmentPoint, GLenum textureTarget, GLuint textureId, GLint  level);

DECLARE_wrapper(GLenum, glCheckFramebufferStatusEXT, GLenum target);

DECLARE_wrapper(void, glGenerateMipmapEXT, GLenum);


} // !gui
} // ! Graphics
} // ! Ew
