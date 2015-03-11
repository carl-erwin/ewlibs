#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __linux__
#include <GL/glext.h>
#endif

#ifdef WIN32
// #include <GL/wglext.h>
#endif

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

#include <ew/graphics/color.hpp>
// #include <ew/graphics/light.hpp>


#include "renderer.hpp"

#include "libGL.hpp"

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

using namespace ew::core::types;

void renderer::clearColor(u8 r, u8 g, u8 b, u8 a)
{
	double Cr = (double) r / 255.0f;
	double Cg = (double) g / 255.0f;
	double Cb = (double) b / 255.0f;
	double Ca = (double) a / 255.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}

void renderer::clearColor(s8 r, s8 g, s8 b, s8 a)
{
	double Cr = (double) r / 127.0f;
	double Cg = (double) g / 127.0f;
	double Cb = (double) b / 127.0f;
	double Ca = (double) a / 127.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}

void renderer::clearColor(u16 r, u16 g, u16 b, u16 a)
{
	double Cr = (double) r / 65535.0f;
	double Cg = (double) g / 65535.0f;
	double Cb = (double) b / 65535.0f;
	double Ca = (double) a / 65535.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}

void renderer::clearColor(s16 r, s16 g, s16 b, s16 a)
{
	double Cr = (double) r / 32767.0f;
	double Cg = (double) g / 32767.0f;
	double Cb = (double) b / 32767.0f;
	double Ca = (double) a / 32767.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}

void renderer::clearColor(u32 r, u32 g, u32 b, u32 a)
{
	double Cr = (double) r / 4294967295.0f;
	double Cg = (double) g / 4294967295.0f;
	double Cb = (double) b / 4294967295.0f;
	double Ca = (double) a / 4294967295.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}

void renderer::clearColor(s32 r, s32 g, s32 b, s32 a)
{
	double Cr = (double) r / 2147483647.0f;
	double Cg = (double) g / 2147483647.0f;
	double Cb = (double) b / 2147483647.0f;
	double Ca = (double) a / 2147483647.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}

void renderer::clearColor(f32 r, f32 g, f32 b, f32 a)
{
	ew_glClearColor((GLclampf) r, (GLclampf) g, (GLclampf) b, (GLclampf) a);
}

void renderer::clearColor(double r, double g, double b, double a)
{
	ew_glClearColor((GLclampf) r, (GLclampf) g, (GLclampf) b, (GLclampf) a);
}

void renderer::clearColor(u8 * color)
{
	ew_glClearColor((GLclampf) color[ 0 ], (GLclampf) color[ 1 ], (GLclampf) color[ 2 ], (GLclampf) color[ 3 ]);
}

void renderer::clearColor(s8 * color)
{
	ew_glClearColor((GLclampf) color[ 0 ], (GLclampf) color[ 1 ], (GLclampf) color[ 2 ], (GLclampf) color[ 3 ]);
}

void renderer::clearColor(u16 * color)
{
	ew_glClearColor((GLclampf) color[ 0 ], (GLclampf) color[ 1 ], (GLclampf) color[ 2 ], (GLclampf) color[ 3 ]);
}

void renderer::clearColor(s16 * color)
{
	ew_glClearColor((GLclampf) color[ 0 ], (GLclampf) color[ 1 ], (GLclampf) color[ 2 ], (GLclampf) color[ 3 ]);
}

void renderer::clearColor(u32 * color)
{
	ew_glClearColor((GLclampf) color[ 0 ], (GLclampf) color[ 1 ], (GLclampf) color[ 2 ], (GLclampf) color[ 3 ]);
}
void renderer::clearColor(s32 * color)
{
	ew_glClearColor((GLclampf) color[ 0 ], (GLclampf) color[ 1 ], (GLclampf) color[ 2 ], (GLclampf) color[ 3 ]);
}

void renderer::clearColor(f32 * color)
{
	ew_glClearColor((GLclampf) color[ 0 ], (GLclampf) color[ 1 ], (GLclampf) color[ 2 ], (GLclampf) color[ 3 ]);
}


void renderer::clearColor(double * color)
{
	ew_glClearColor((GLclampf) color[ 0 ], (GLclampf) color[ 1 ], (GLclampf) color[ 2 ], (GLclampf) color[ 3 ]);
}

void renderer::clearColor(color4ub & color)
{
	double Cr = (double) color.r / 255.0f;
	double Cg = (double) color.g / 255.0f;
	double Cb = (double) color.b / 255.0f;
	double Ca = (double) color.a / 255.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}

void renderer::clearColor(color4b & color)
{
	double Cr = (double) color.r / 127.0f;
	double Cg = (double) color.g / 127.0f;
	double Cb = (double) color.b / 127.0f;
	double Ca = (double) color.a / 127.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}

void renderer::clearColor(color4us & color)
{
	double Cr = (double) color.r / 656535.0f;
	double Cg = (double) color.g / 656535.0f;
	double Cb = (double) color.b / 656535.0f;
	double Ca = (double) color.a / 656535.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}

void renderer::clearColor(color4s & color)
{
	double Cr = (double) color.r / 32767.0f;
	double Cg = (double) color.g / 32767.0f;
	double Cb = (double) color.b / 32767.0f;
	double Ca = (double) color.a / 32767.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}

void renderer::clearColor(color4ui & color)
{
	double Cr = (double) color.r / 4294967295.0f;
	double Cg = (double) color.g / 4294967295.0f;
	double Cb = (double) color.b / 4294967295.0f;
	double Ca = (double) color.a / 4294967295.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}
void renderer::clearColor(color4i & color)
{
	double Cr = (double) color.r / 2147483647.0f;
	double Cg = (double) color.g / 2147483647.0f;
	double Cb = (double) color.b / 2147483647.0f;
	double Ca = (double) color.a / 2147483647.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) Ca);
}

void renderer::clearColor(color4f & color)
{
	ew_glClearColor((GLclampf) color.r, (GLclampf) color.g, (GLclampf) color.b, (GLclampf) color.a);
}

void renderer::clearColor(color4d & color)
{
	ew_glClearColor((GLclampf) color.r, (GLclampf) color.g, (GLclampf) color.b, (GLclampf) color.a);
}

/// color3



void renderer::clearColor(color3ub & color)
{
	double Cr = (double) color.r / 255.0f;
	double Cg = (double) color.g / 255.0f;
	double Cb = (double) color.b / 255.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) 1.0);
}

void renderer::clearColor(color3b & color)
{
	double Cr = (double) color.r / 127.0f;
	double Cg = (double) color.g / 127.0f;
	double Cb = (double) color.b / 127.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) 1.0);
}

void renderer::clearColor(color3us & color)
{
	double Cr = (double) color.r / 656535.0f;
	double Cg = (double) color.g / 656535.0f;
	double Cb = (double) color.b / 656535.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) 1.0);
}

void renderer::clearColor(color3s & color)
{
	double Cr = (double) color.r / 32767.0f;
	double Cg = (double) color.g / 32767.0f;
	double Cb = (double) color.b / 32767.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) 1.0);
}

void renderer::clearColor(color3ui & color)
{
	double Cr = (double) color.r / 4294967295.0f;
	double Cg = (double) color.g / 4294967295.0f;
	double Cb = (double) color.b / 4294967295.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) 1.0);
}
void renderer::clearColor(color3i & color)
{
	double Cr = (double) color.r / 2147483647.0f;
	double Cg = (double) color.g / 2147483647.0f;
	double Cb = (double) color.b / 2147483647.0f;

	ew_glClearColor((GLclampf) Cr, (GLclampf) Cg, (GLclampf) Cb, (GLclampf) 1.0);
}

void renderer::clearColor(color3f & color)
{
	ew_glClearColor((GLclampf) color.r, (GLclampf) color.g, (GLclampf) color.b, (GLclampf) 1.0);
}

void renderer::clearColor(color3d & color)
{
	ew_glClearColor((GLclampf) color.r, (GLclampf) color.g, (GLclampf) color.b, (GLclampf) 1.0);
}

}
}
}
}
} // ! ew::implementation::Graphics::renderer::OpenGL
