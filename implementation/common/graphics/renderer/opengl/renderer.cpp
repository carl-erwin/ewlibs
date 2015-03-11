
#include <ew/ew_config.hpp>

#ifdef EW_TARGET_SYSTEM_WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __linux__
#include <GL/glext.h>
#endif

#ifdef EW_TARGET_SYSTEM_WIN32
// #include <GL/wglext.h>
#endif


#include <ew/core/types/types.hpp>
using namespace ew::core::types;


#include <ew/maths/maths.hpp>
using namespace ew::maths;


#include <ew/console/console.hpp>
using ew::console::cerr;

#include <ew/graphics/graphics.hpp>

//#include "graphics/color.hpp"
#include <ew/graphics/light.hpp>

#include "renderer.hpp"

#include "libGL.hpp"

// subdiviser le renderer en autant de fichier que d'include
// renderer/OpenGL/color.cpp
// renderer/OpenGL/light.cpp
// renderer/OpenGL/buffer.cpp       function that work on 2 or more Xbuffer : Clear(..) , etc
// renderer/OpenGL/framebuffer.cpp
// renderer/OpenGL/depthbuffer.cpp
// renderer/OpenGL/stencilbuffer.cpp
// renderer/OpenGL/vertexbuffer.cpp
// renderer/OpenGL/indexbuffer.cpp


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

// renommer renderer en OpenGLrenderer ???
// add rendering::init() ?

using namespace ew::graphics::rendering;

renderer::renderer()
	:
	_matrixMode(ew::graphics::rendering::UNDEFINED_MATRIX)
{
	cerr << "Render :: try : load_libGL_dll()" << "\n";
	load_libGL_dll();
}

renderer::~renderer()
{

}

bool renderer::beginScene(void)
{
	return true;
}

void renderer::endScene(void)
{

}

void renderer::endSceneAndFlipBuffers(void)
{
	// ew::System::Video::Device::GetCurrentVideoDevice()->Flip();
}

void renderer::flipBuffers(void)
{
	//ew::System::Video::Device::GetCurrentVideoDevice()->Flip();
}

void renderer::clearBuffer(bool color_buffer, bool z_buffer, bool stencil_buffer)
{
	GLbitfield gl_mask = 0;

	if (color_buffer == true)
		gl_mask |= GL_COLOR_BUFFER_BIT;

	if (z_buffer == true)
		gl_mask |= GL_DEPTH_BUFFER_BIT;

	if (stencil_buffer == true)
		gl_mask |= GL_STENCIL_BUFFER_BIT;

	ew_glClear(gl_mask);
}


// -------------------------------------------------------------------------------------------

void renderer::enableZBuffer(void)
{
	ew_glEnable(GL_DEPTH_TEST);
}

void renderer::disableZBuffer(void)
{
	ew_glDisable(GL_DEPTH_TEST);
}

void renderer::setrender_mode(enum render_mode mode)
{
	switch (mode) {
	case WIREFRAME_RENDER_MODE: {
		ew_glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	break;

	case POINT_RENDER_MODE: {
		ew_glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	break;

	case FILL_RENDER_MODE: {
		ew_glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	}

}

void renderer::loadIdentityMatrix(void)
{
	ew_glLoadIdentity();
}

void renderer::multMatrix(const Matrix44f & mat)
{
	ew_glMultMatrixf((const GLfloat *) & mat);
}


void renderer::multMatrix(const Matrix44d & mat)
{
	ew_glMultMatrixd((const GLdouble *) & mat);
}

void renderer::multTransposeMatrix(const mat4f & mat)
{
	mat4f tmp = mat.Transpose();
	ew_glMultMatrixf((const GLfloat *) & tmp);
}

void renderer::multTransposeMatrix(const mat4d & mat)
{
	mat4d tmp = mat.Transpose();
	ew_glMultMatrixd((const GLdouble *) & tmp);
}

// ----------------------------------------------

void renderer::beginPrimitive(enum Primitive primitive)
{
	switch (primitive) {

	case RECTANGLE_PRIMITIVE: {
		ew_glBegin(GL_QUADS);
	}
	break;
	case TRIANGLES_PRIMITIVE: {
		ew_glBegin(GL_TRIANGLES);
	}
	break;
	case POINTS_PRIMITIVE: {
		ew_glBegin(GL_POINTS);
	}
	break;
	case LINES_PRIMITIVE: {
		ew_glBegin(GL_LINES);
	}
	break;

	default:
		cerr << "BeginPrimitive Not implemented Yet : " << static_cast<s32>(primitive) << "\n";
	}
}


void renderer::endPrimitive(void)
{
	ew_glEnd();
}


void renderer::vertex3(s16 x, s16 y, s16 z)
{
	ew_glVertex3s(x, y, z);
}

void renderer::vertex3(s32 x, s32 y, s32 z)
{
	ew_glVertex3i(x, y, z);
}

void renderer::vertex3(f32 x, f32 y, f32 z)
{
	ew_glVertex3f(x, y, z);
}

void renderer::vertex3(double x, double y, double z)
{
	ew_glVertex3d(x, y, z);
}

void renderer::vertex3(s16 * v)
{
	ew_glVertex3sv((const GLshort *) v);
}

void renderer::vertex3(s32 * v)
{
	ew_glVertex3iv((const GLint *) v) ;
}

void renderer::vertex3(f32 * v)
{
	ew_glVertex3fv((const GLfloat *) v) ;
}

void renderer::vertex3(double * v)
{
	ew_glVertex3dv((const GLdouble *) v);
}



void renderer::viewport(s32 x, s32 y, s32 width, s32 height)
{
	ew_glViewport(x, y, width, height);
}

void renderer::matrixMode(enum MatrixMode mode)
{
	switch (mode) {
	case UNDEFINED_MATRIX: {
		// throw exception
	}
	break;

	case PROJECTION_MATRIX: {
		ew_glMatrixMode(GL_PROJECTION);
		_matrixMode = mode;
	}
	break;

	case MODELVIEW_MATRIX: {
		ew_glMatrixMode(GL_MODELVIEW);
		_matrixMode = mode;
	}
	break;

	case TEXTURE_MATRIX: {
		ew_glMatrixMode(GL_TEXTURE);
		_matrixMode = mode;
	}
	break;

	case COLOR_MATRIX: {
	}
	break;
	}
}


enum MatrixMode renderer::getMatrixMode(void)
{
	return (_matrixMode);
}


// -------------------------------------


bool renderer::enableLighting(void)
{
	ew_glEnable(GL_LIGHTING);
	return (true);
}

bool renderer::disableLighting(void)
{
	ew_glDisable(GL_LIGHTING);
	return (true);
}

void renderer::setLight(u32 index, Light & light)
{
	switch (light.GetType()) {
	case Light::POSITION: {
		if (light.m_position.w == 0.0f)
			light.m_position.w = 1.0f;
	}
	break;

	case Light::SPOT: {
	}
	break;

	case Light::DIRECTIONAL: {
		if (light.m_position.w != 0.0f)
			light.m_position.w = 0.0f;
	}
	break;
	}
	//
	ew_glLightfv(GL_LIGHT0 + index, GL_POSITION, (const GLfloat *) light.m_position.GetDataPtr());
	ew_glLightfv(GL_LIGHT0 + index, GL_AMBIENT, (const GLfloat *) light.m_ambient.GetDataPtr());
	ew_glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, (const GLfloat *) light.m_diffuse.GetDataPtr());
	ew_glLightfv(GL_LIGHT0 + index, GL_SPECULAR, (const GLfloat *) light.m_specular.GetDataPtr());

	// This part is untested
	ew_glLightfv(GL_LIGHT0 + index, GL_SPOT_DIRECTION, (const GLfloat *) light.m_direction.GetDataPtr());
	//   ew_glLightf(GL_LIGHT0  + index, GL_SPOT_CUTOFF,    light.m_spot_cutoff);

	//   ew_glLightf(GL_LIGHT0  + index, GL_SPOT_EXPONENT,  light.m_spot_exponent);
	//
	ew_glLightf(GL_LIGHT0 + index, GL_CONSTANT_ATTENUATION, light.m_constant_attenuation);
	ew_glLightf(GL_LIGHT0 + index, GL_LINEAR_ATTENUATION, light.m_linear_attenuation);
	ew_glLightf(GL_LIGHT0 + index, GL_QUADRATIC_ATTENUATION, light.m_quadratic_attenuation);
}

void renderer::lightOn(u32 index)
{
	// if (index > 8 // gl maxlight see caps )
	ew_glEnable(GL_LIGHT0 + index);
}

void renderer::lightOff(u32 index)
{
	// if (index > 8 // gl maxlight see caps )
	ew_glDisable(GL_LIGHT0 + index);
}

}
}
}
}
} // ! ew::implementation::graphics::rendering::opengl
