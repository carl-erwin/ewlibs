#include <ew/graphics/renderer/renderer.hpp>

#include "implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "implementation/common/graphics/renderer/opengl/libGLU.hpp"


namespace ew
{
namespace graphics
{
namespace rendering
{

using namespace ew::implementation::graphics::rendering::opengl;

static int ref = 0;

EW_GRAPHICS_EXPORT bool init()
{
	if (ref == 0) {
		// load libGL, LibGLU
		load_libGL_dll();
		load_libGLU_dll();
	}
	++ref;

	return true;
}

EW_GRAPHICS_EXPORT bool quit()
{
	--ref;
	if (ref == 0) {
		// unload libGLU, LibGL
		unload_libGLU_dll();
		unload_libGL_dll();
	}
	return true;
}


}
}
}





