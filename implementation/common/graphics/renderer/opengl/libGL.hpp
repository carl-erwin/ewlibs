#pragma once

#include <cassert>

#ifdef EW_TARGET_SYSTEM_WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <ew/ew_config.hpp>

// Dll
#include <ew/core/dll/dll.hpp>
#include <ew/core/exception/exception.hpp>

// wrapper
#include <ew/core/wrapper/wrapper.hpp>

/* TODO : sort by gl version */

namespace ew
{
namespace implementation
{
namespace graphics
{
namespace rendering
{
namespace opengl
{

using namespace ew::core::exceptions;


EW_GRAPHICS_RENDERING_OPENGL_EXPORT   bool load_libGL_dll();
EW_GRAPHICS_RENDERING_OPENGL_EXPORT   bool unload_libGL_dll();

// openGL >= 1.2
DECLARE_wrapper(void, glDisable, GLenum cap);
DECLARE_wrapper(void, glEnable, GLenum cap);
DECLARE_wrapper(void, glBegin, GLenum mode);
DECLARE_wrapper(void, glEnd, void);
DECLARE_wrapper(void, glMatrixMode, GLenum mode);
DECLARE_wrapper(void, glClear, GLbitfield mask);
DECLARE_wrapper(void, glClearColor, GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
DECLARE_wrapper(void, glPolygonMode, GLenum face, GLenum mode);

DECLARE_wrapper(void, glLoadIdentity, void);
DECLARE_wrapper(void, glMultMatrixd, const GLdouble * m);
DECLARE_wrapper(void, glMultMatrixf, const GLfloat * m);

DECLARE_wrapper(void, glVertex2d, GLdouble x, GLdouble y);
DECLARE_wrapper(void, glVertex2f, GLfloat x, GLfloat y);
DECLARE_wrapper(void, glVertex2i, GLint x, GLint y);
DECLARE_wrapper(void, glVertex2s, GLshort x, GLshort y);
DECLARE_wrapper(void, glVertex3d, GLdouble x, GLdouble y, GLdouble z);
DECLARE_wrapper(void, glVertex3f, GLfloat x, GLfloat y, GLfloat z);
DECLARE_wrapper(void, glVertex3i, GLint x, GLint y, GLint z);
DECLARE_wrapper(void, glVertex3s, GLshort x, GLshort y, GLshort z);
DECLARE_wrapper(void, glVertex4d, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
DECLARE_wrapper(void, glVertex4f, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
DECLARE_wrapper(void, glVertex4i, GLint x, GLint y, GLint z, GLint w);
DECLARE_wrapper(void, glVertex4s, GLshort x, GLshort y, GLshort z, GLshort w);

DECLARE_wrapper(void, glVertex2dv, const GLdouble * v);
DECLARE_wrapper(void, glVertex2fv, const GLfloat * v);
DECLARE_wrapper(void, glVertex2iv, const GLint * v);
DECLARE_wrapper(void, glVertex2sv, const GLshort * v);
DECLARE_wrapper(void, glVertex3dv, const GLdouble * v);
DECLARE_wrapper(void, glVertex3fv, const GLfloat * v);
DECLARE_wrapper(void, glVertex3iv, const GLint * v);
DECLARE_wrapper(void, glVertex3sv, const GLshort * v);
DECLARE_wrapper(void, glVertex4dv, const GLdouble * v);
DECLARE_wrapper(void, glVertex4fv, const GLfloat * v);
DECLARE_wrapper(void, glVertex4iv, const GLint * v);
DECLARE_wrapper(void, glVertex4sv, const GLshort * v);
DECLARE_wrapper(void, glViewport, GLint x, GLint y, GLsizei width, GLsizei height);
DECLARE_wrapper(void, glLightf, GLenum light, GLenum pname, GLfloat param);
DECLARE_wrapper(void, glLighti, GLenum light, GLenum pname, GLint param);
DECLARE_wrapper(void, glLightfv, GLenum light, GLenum pname, const GLfloat * params);
DECLARE_wrapper(void, glLightiv, GLenum light, GLenum pname, const GLint * params);
DECLARE_wrapper(void, glGenTextures, GLsizei n, GLuint * textures);
DECLARE_wrapper(void, glBindTexture, GLenum target, GLuint texture);
DECLARE_wrapper(void, glTexParameterf, GLenum target, GLenum pname, GLfloat param);
DECLARE_wrapper(void, glTexParameteri, GLenum target, GLenum pname, GLint param);
DECLARE_wrapper(void, glTexImage2D, GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels);

DECLARE_wrapper(void, glTexSubImage2D, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels);

//  vertex array
DECLARE_wrapper(void, glEnableClientState, GLenum cap);
DECLARE_wrapper(void, glDisableClientState, GLenum cap);
DECLARE_wrapper(void, glArrayElement, GLint i);
DECLARE_wrapper(void, glClientActiveTextureARB, GLenum texture);
DECLARE_wrapper(void, glColorPointer, GLint size,  GLenum type, GLsizei stride, const GLvoid * ptr);
DECLARE_wrapper(void, glDrawArrays, GLenum mode, GLint first,     GLsizei count);
DECLARE_wrapper(void, glDrawElements, GLenum mode,  GLsizei count, GLenum type, const GLvoid * indices);
DECLARE_wrapper(void, glEdgeFlagPointer, GLsizei stride,     const GLvoid * ptr);
DECLARE_wrapper(void, glGetPointerv, GLenum pname, void ** params);
DECLARE_wrapper(void, glIndexPointer, GLenum type,  GLsizei stride, const GLvoid * ptr);
DECLARE_wrapper(void, glInterleavedArrays, GLenum format, GLsizei stride, const GLvoid * pointer);
DECLARE_wrapper(void, glNormalPointer, GLenum type,   GLsizei stride,  const GLvoid * ptr);
DECLARE_wrapper(void, glTexCoordPointer, GLint size,     GLenum type,    GLsizei stride,    const GLvoid * ptr);
DECLARE_wrapper(void, glVertexPointer, GLint size,   GLenum type,  GLsizei stride,  const GLvoid * ptr);

DECLARE_wrapper(void, glColor3b, GLbyte red, GLbyte green, GLbyte blue);
DECLARE_wrapper(void, glColor3d, GLdouble red, GLdouble green, GLdouble blue);
DECLARE_wrapper(void, glColor3f, GLfloat red, GLfloat green, GLfloat blue);
DECLARE_wrapper(void, glColor3i, GLint red, GLint green, GLint blue);
DECLARE_wrapper(void, glColor3s, GLshort red, GLshort green, GLshort blue);
DECLARE_wrapper(void, glColor3ub, GLubyte red, GLubyte green, GLubyte blue);
DECLARE_wrapper(void, glColor3ui, GLuint red, GLuint green, GLuint blue);
DECLARE_wrapper(void, glColor3us, GLushort red, GLushort green, GLushort blue);
DECLARE_wrapper(void, glColor4b, GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
DECLARE_wrapper(void, glColor4d, GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
DECLARE_wrapper(void, glColor4f, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
DECLARE_wrapper(void, glColor4i, GLint red, GLint green, GLint blue, GLint alpha);
DECLARE_wrapper(void, glColor4s, GLshort red, GLshort green, GLshort blue, GLshort alpha);
DECLARE_wrapper(void, glColor4ub, GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
DECLARE_wrapper(void, glColor4ui, GLuint red, GLuint green, GLuint blue, GLuint alpha);
DECLARE_wrapper(void, glColor4us, GLushort red, GLushort green, GLushort blue, GLushort alpha);
DECLARE_wrapper(void, glColor3bv, const GLbyte * v);
DECLARE_wrapper(void, glColor3dv, const GLdouble * v);
DECLARE_wrapper(void, glColor3fv, const GLfloat * v);
DECLARE_wrapper(void, glColor3iv, const GLint * v);
DECLARE_wrapper(void, glColor3sv, const GLshort * v);
DECLARE_wrapper(void, glColor3ubv, const GLubyte * v);
DECLARE_wrapper(void, glColor3uiv, const GLuint * v);
DECLARE_wrapper(void, glColor3usv, const GLushort * v);
DECLARE_wrapper(void, glColor4bv, const GLbyte * v);
DECLARE_wrapper(void, glColor4dv, const GLdouble * v);
DECLARE_wrapper(void, glColor4fv, const GLfloat * v);
DECLARE_wrapper(void, glColor4iv, const GLint * v);
DECLARE_wrapper(void, glColor4sv, const GLshort * v);
DECLARE_wrapper(void, glColor4ubv, const GLubyte * v);
DECLARE_wrapper(void, glColor4uiv, const GLuint * v);
DECLARE_wrapper(void, glColor4usv, const GLushort * v);
DECLARE_wrapper(void, glTranslated, GLdouble x, GLdouble y, GLdouble z);
DECLARE_wrapper(void, glTranslatef, GLfloat x, GLfloat y, GLfloat z);
DECLARE_wrapper(void, glRotated, GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
DECLARE_wrapper(void, glRotatef, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
DECLARE_wrapper(void, glScaled, GLdouble x, GLdouble y, GLdouble z);
DECLARE_wrapper(void, glScalef, GLfloat x, GLfloat y, GLfloat z);
DECLARE_wrapper(void, glPushMatrix, void);
DECLARE_wrapper(void, glPopMatrix, void);

DECLARE_wrapper(void, glShadeModel, GLenum mode);


DECLARE_wrapper(void, glNormal3b, GLbyte nx, GLbyte ny, GLbyte nz);
DECLARE_wrapper(void, glNormal3d, GLdouble nx, GLdouble ny, GLdouble nz);
DECLARE_wrapper(void, glNormal3f, GLfloat nx, GLfloat ny, GLfloat nz);
DECLARE_wrapper(void, glNormal3i, GLint nx, GLint ny, GLint nz);
DECLARE_wrapper(void, glNormal3s, GLshort nx, GLshort ny, GLshort nz);
DECLARE_wrapper(void, glNormal3bv, const GLbyte * v);
DECLARE_wrapper(void, glNormal3dv, const GLdouble * v);
DECLARE_wrapper(void, glNormal3fv, const GLfloat * v);
DECLARE_wrapper(void, glNormal3iv, const GLint * v);
DECLARE_wrapper(void, glNormal3sv, const GLshort * v);
DECLARE_wrapper(void, glCallList, GLuint list);
DECLARE_wrapper(GLuint, glGenLists, GLsizei range);
DECLARE_wrapper(void, glNewList, GLuint list, GLenum mode);
DECLARE_wrapper(void, glEndList, void);
DECLARE_wrapper(void, glMaterialf, GLenum face, GLenum pname, GLfloat param);
DECLARE_wrapper(void, glMateriali, GLenum face, GLenum pname, GLint param);
DECLARE_wrapper(void, glMaterialfv, GLenum face, GLenum pname, const GLfloat * params);
DECLARE_wrapper(void, glMaterialiv, GLenum face, GLenum pname, const GLint * params);
DECLARE_wrapper(void, glFrustum, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
DECLARE_wrapper(void, glDeleteTextures, GLsizei n, const GLuint * textures);


DECLARE_wrapper(void, glDepthFunc, GLenum func);
DECLARE_wrapper(void, glClearDepth, GLclampd depth);
DECLARE_wrapper(void, glBlendFunc, GLenum sfactor, GLenum dfactor);
DECLARE_wrapper(void, glTexCoord1d, GLdouble s);
DECLARE_wrapper(void, glTexCoord1f, GLfloat s);
DECLARE_wrapper(void, glTexCoord1i, GLint s);
DECLARE_wrapper(void, glTexCoord1s, GLshort s);
DECLARE_wrapper(void, glTexCoord2d, GLdouble s, GLdouble t);
DECLARE_wrapper(void, glTexCoord2f, GLfloat s, GLfloat t);
DECLARE_wrapper(void, glTexCoord2i, GLint s, GLint t);
DECLARE_wrapper(void, glTexCoord2s, GLshort s, GLshort t);
DECLARE_wrapper(void, glTexCoord3d, GLdouble s, GLdouble t, GLdouble r);
DECLARE_wrapper(void, glTexCoord3f, GLfloat s, GLfloat t, GLfloat r);
DECLARE_wrapper(void, glTexCoord3i, GLint s, GLint t, GLint r);
DECLARE_wrapper(void, glTexCoord3s, GLshort s, GLshort t, GLshort r);
DECLARE_wrapper(void, glTexCoord4d, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
DECLARE_wrapper(void, glTexCoord4f, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
DECLARE_wrapper(void, glTexCoord4i, GLint s, GLint t, GLint r, GLint q);
DECLARE_wrapper(void, glTexCoord4s, GLshort s, GLshort t, GLshort r, GLshort q);

DECLARE_wrapper(void, glTexCoord1dv, const GLdouble * v);
DECLARE_wrapper(void, glTexCoord1fv, const GLfloat * v);
DECLARE_wrapper(void, glTexCoord1iv, const GLint * v);
DECLARE_wrapper(void, glTexCoord1sv, const GLshort * v);
DECLARE_wrapper(void, glTexCoord2dv, const GLdouble * v);
DECLARE_wrapper(void, glTexCoord2fv, const GLfloat * v);
DECLARE_wrapper(void, glTexCoord2iv, const GLint * v);
DECLARE_wrapper(void, glTexCoord2sv, const GLshort * v);
DECLARE_wrapper(void, glTexCoord3dv, const GLdouble * v);
DECLARE_wrapper(void, glTexCoord3fv, const GLfloat * v);
DECLARE_wrapper(void, glTexCoord3iv, const GLint * v);
DECLARE_wrapper(void, glTexCoord3sv, const GLshort * v);
DECLARE_wrapper(void, glTexCoord4dv, const GLdouble * v);
DECLARE_wrapper(void, glTexCoord4fv, const GLfloat * v);
DECLARE_wrapper(void, glTexCoord4iv, const GLint * v);
DECLARE_wrapper(void, glTexCoord4sv, const GLshort * v);




DECLARE_wrapper(GLenum, glGetError, void);

DECLARE_wrapper(void, glDeleteLists, GLuint list, GLsizei range);
DECLARE_wrapper(void, glPushAttrib, GLbitfield mask);

DECLARE_wrapper(void, glHint, GLenum target, GLenum mode);

DECLARE_wrapper(void, glRasterPos2s, GLshort x, GLshort y);
DECLARE_wrapper(void, glRasterPos2i, GLint x, GLint y);
DECLARE_wrapper(void, glRasterPos2f, GLfloat x, GLfloat y);
DECLARE_wrapper(void, glRasterPos2d, GLdouble x, GLdouble y);
DECLARE_wrapper(void, glRasterPos3s, GLshort x, GLshort y, GLshort z);
DECLARE_wrapper(void, glRasterPos3i, GLint x, GLint y, GLint z);
DECLARE_wrapper(void, glRasterPos3f, GLfloat x, GLfloat y, GLfloat z);
DECLARE_wrapper(void, glRasterPos3d, GLdouble x, GLdouble y, GLdouble z);
DECLARE_wrapper(void, glRasterPos4s, GLshort x, GLshort y, GLshort z, GLshort w);
DECLARE_wrapper(void, glRasterPos4i, GLint x, GLint y, GLint z, GLint w);
DECLARE_wrapper(void, glRasterPos4f, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
DECLARE_wrapper(void, glRasterPos4d, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
DECLARE_wrapper(void, glRasterPos2sv, const GLshort * v);
DECLARE_wrapper(void, glRasterPos2iv, const GLint * v);
DECLARE_wrapper(void, glRasterPos2fv, const GLfloat * v);
DECLARE_wrapper(void, glRasterPos2dv, const GLdouble * v);
DECLARE_wrapper(void, glRasterPos3sv, const GLshort * v);
DECLARE_wrapper(void, glRasterPos3iv, const GLint * v);
DECLARE_wrapper(void, glRasterPos3fv, const GLfloat * v);
DECLARE_wrapper(void, glRasterPos3dv, const GLdouble * v);
DECLARE_wrapper(void, glRasterPos4sv, const GLshort * v);
DECLARE_wrapper(void, glRasterPos4iv, const GLint * v);
DECLARE_wrapper(void, glRasterPos4fv, const GLfloat * v);
DECLARE_wrapper(void, glRasterPos4dv, const GLdouble * v);

DECLARE_wrapper(void, glGetIntegerv, GLenum pname, GLint * params);
DECLARE_wrapper(void, glPopAttrib, void);
DECLARE_wrapper(void, glListBase, GLuint base);
DECLARE_wrapper(void, glGetFloatv, GLenum pname, GLfloat * params);

DECLARE_wrapper(void, glCallLists, GLsizei n, GLenum type, const GLvoid * lists);

DECLARE_wrapper(void, glFlush, void);

DECLARE_wrapper(void, glFinish, void);

DECLARE_wrapper(void, glOrtho, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);

DECLARE_wrapper(void, glColorMaterial, GLenum face, GLenum mode);


// because all gl function  are prefixed by ew_
// it is not optimal to write ew_glBegin(..) etc ..
// so create macros to let the library users use the OpenGL names
// by replacing gl* by ew_gl*

#ifndef WIN32
#include "libGL_wrappers.hpp"
#endif




// helpers
// will be attached to window
static inline GLenum ew_debug_glGetError(const char * msg, const char * function, int line)
{

	GLenum st = ew_glGetError();

	if (st != GL_NO_ERROR) {
		std::cerr << msg << " in " << function << " @ line " << line << "\n";
		std::cerr << "st = " << st << "\n";
		assert(0);
	}

	return st;
}

}
}
}
}
}
