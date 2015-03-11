
#include <ew/ew_config.hpp>

#include <ew/graphics/renderer/renderer.hpp>

namespace ew
{
namespace graphics
{
namespace rendering
{

// renderer.cpp -----------------------------------------------------------------------------
static ew::graphics::rendering::renderer * current_renderer = 0;

void setCurrentrenderer(ew::graphics::rendering::renderer * renderer)
{
	current_renderer = renderer;
}

ew::graphics::rendering::renderer * getCurrentrenderer(void)
{
	return (current_renderer);
}

// renderer.cpp -----------------------------------------------------------------------------
bool beginScene(void)
{
	return (current_renderer->beginScene());
}
void endScene(void)
{
	current_renderer->endScene();
}
void endSceneAndFlipBuffers(void)
{
	current_renderer->endSceneAndFlipBuffers();
}
void flipBuffers(void)
{
	current_renderer->flipBuffers();
}

// buffer.cpp ------------------------------------------------------------------------------
void clearBuffer(bool color_buffer, bool z_buffer, bool stencil_buffer)
{
	current_renderer->clearBuffer(color_buffer, z_buffer, stencil_buffer);
}
// void ClearBuffer( BufferType buffer ) = 0;

// clear Framebuffer
// framebuffer.cpp -----------------------------------------------------------------------------

void clearColor(u8 r, u8 g, u8 b, u8 a)
{
	current_renderer->clearColor(r, g, b, a);
}
void clearColor(s8 r, s8 g, s8 b, s8 a)
{
	current_renderer->clearColor(r, g, b, a);
}
void clearColor(u16 r, u16 g, u16 b, u16 a)
{
	current_renderer->clearColor(r, g, b, a);
}
void clearColor(s16 r, s16 g, s16 b, s16 a)
{
	current_renderer->clearColor(r, g, b, a);
}
void clearColor(u32 r, u32 g, u32 b, u32 a)
{
	current_renderer->clearColor(r, g, b, a);
}
void clearColor(s32 r, s32 g, s32 b, s32 a)
{
	current_renderer->clearColor(r, g, b, a);
}
void clearColor(f32 r, f32 g, f32 b, f32 a)
{
	current_renderer->clearColor(r, g, b, a);
}
void clearColor(double r, double g, double b, double a)
{
	current_renderer->clearColor(r, g, b, a);
}

void clearColor(u8 * color)
{
	current_renderer->clearColor(color);
}
void clearColor(s8 * color)
{
	current_renderer->clearColor(color);
}
void clearColor(u16 * color)
{
	current_renderer->clearColor(color);
}
void clearColor(s16 * color)
{
	current_renderer->clearColor(color);
}
void clearColor(u32 * color)
{
	current_renderer->clearColor(color);
}
void clearColor(s32 * color)
{
	current_renderer->clearColor(color);
}
void clearColor(f32 * color)
{
	current_renderer->clearColor(color);
}
void clearColor(double * color)
{
	current_renderer->clearColor(color);
}

void clearColor(color4ub & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color4b & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color4us & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color4s & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color4ui & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color4i & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color4f & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color4d & color)
{
	current_renderer->clearColor(color);
}

void clearColor(color3ub & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color3b & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color3us & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color3s & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color3ui & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color3i & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color3f & color)
{
	current_renderer->clearColor(color);
}
void clearColor(color3d & color)
{
	current_renderer->clearColor(color);
}

// -----------------------------------------------------------------------------------------------
void enableZBuffer(void)
{
	current_renderer->enableZBuffer();
}
void disableZBuffer(void)
{
	current_renderer->disableZBuffer();
}
// matrix.cpp ------------------------------------------------------------------------------------
void loadIdentityMatrix(void)
{
	current_renderer->loadIdentityMatrix();
}
void multMatrix(const Matrix44f & mat)
{
	current_renderer->multMatrix(mat);
}
void multMatrix(const Matrix44d & mat)
{
	current_renderer->multMatrix(mat);
}
void multTransposeMatrix(const mat4f & mat)
{
	current_renderer->multTransposeMatrix(mat);
}
void multTransposeMatrix(const mat4d & mat)
{
	current_renderer->multTransposeMatrix(mat);
}

void matrixMode(enum MatrixMode mode)
{
	current_renderer->matrixMode(mode);
}
enum MatrixMode getMatrixMode(void)
{
	return (current_renderer->getMatrixMode());
}

// renderer.cpp ------------------------------------------------------------------------------

void setrender_mode(enum render_mode mode)
{
	current_renderer->setrender_mode(mode);
}
void beginPrimitive(enum Primitive primitive)
{
	current_renderer->beginPrimitive(primitive);
}
void endPrimitive(void)
{
	current_renderer->endPrimitive();
}

// -------------------------------------
void vertex3(s16 x, s16 y, s16 z)
{
	current_renderer->vertex3(x, y, z);
}
void vertex3(s32 x, s32 y, s32 z)
{
	current_renderer->vertex3(x, y, z);
}
void vertex3(f32 x, f32 y, f32 z)
{
	current_renderer->vertex3(x, y, z);
}
void vertex3(double x, double y, double z)
{
	current_renderer->vertex3(x, y, z);
}
void vertex3(s16 * v)
{
	current_renderer->vertex3(v);
}
void vertex3(s32 * v)
{
	current_renderer->vertex3(v);
}
void vertex3(f32 * v)
{
	current_renderer->vertex3(v);
}
void vertex3(double * v)
{
	current_renderer->vertex3(v);
}

// -------------------------------------

void viewport(s32 x, s32 y, s32 width, s32 height)
{
	current_renderer->viewport(x, y, width, height);
}

// light.cpp -------------------------------------------------------------------------------------

bool enableLighting(void)
{
	current_renderer->enableLighting();
	return (true);
}

bool disableLighting(void)
{
	current_renderer->disableLighting();
	return (true);
}

void setLight(u32 index, Light & light)
{
	current_renderer->setLight(index, light);
}

void lightOn(u32 index)
{
	current_renderer->lightOn(index);
}

void lightOff(u32 index)
{
	current_renderer->lightOff(index);
}


}
}
} // ! Ew :: Graphics :: rendering

