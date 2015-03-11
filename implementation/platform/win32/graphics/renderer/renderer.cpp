#include <iostream>
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

EW_PLATFORM_EXPORT bool init()
{
	std::cerr << "rendering::init()" << "\n";
	// unload libGL, LibGLU
	// move this in renderer opengl dll loading
	load_libGL_dll();
	// load_libGLU_dll();
	return true;
}

EW_PLATFORM_EXPORT bool quit()
{
	// unload libGL, libGLU
	return true;
}

//  class  EW_GRAPHICS_EXPORT renderer { }

}
}
}





