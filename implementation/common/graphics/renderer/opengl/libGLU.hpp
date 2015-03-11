#pragma once

#include <ew/ew_config.hpp>

// Dll
#include <ew/core/dll/dll.hpp>
#include <ew/core/exception/exception.hpp>

// wrapper helpers
#include <ew/core/wrapper/wrapper.hpp>

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

EW_GRAPHICS_RENDERING_OPENGL_EXPORT   bool load_libGLU_dll();
EW_GRAPHICS_RENDERING_OPENGL_EXPORT   bool unload_libGLU_dll();

DECLARE_wrapper(void, gluPerspective, GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

DECLARE_wrapper(void, gluOrtho2D, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);


}
}
}
}
}
