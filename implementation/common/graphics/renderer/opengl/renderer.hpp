#pragma once

#include <ew/ew_config.hpp>
#include <ew/graphics/renderer/renderer.hpp>

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

using namespace ew::graphics::colors;
using namespace ew::graphics::rendering;

class EW_GRAPHICS_RENDERING_OPENGL_EXPORT renderer : public ew::graphics::rendering::renderer
{
public:
	renderer();

	virtual ~renderer();

	virtual bool beginScene(void);
	virtual void endScene(void);

	virtual void endSceneAndFlipBuffers(void);
	virtual void flipBuffers(void);

	// Buffer -----------------------------------------------------------------

	virtual void clearBuffer(bool color_buffer = true, bool z_buffer = false, bool stencil_buffer = false);

	// Framebuffer.cpp --------------------------------------------------------
	virtual void clearColor(u8 r, u8 g, u8 b, u8 a);
	virtual void clearColor(s8 r, s8 g, s8 b, s8 a);
	virtual void clearColor(u16 r, u16 g, u16 b, u16 a);
	virtual void clearColor(s16 r, s16 g, s16 b, s16 a);
	virtual void clearColor(u32 r, u32 g, u32 b, u32 a);
	virtual void clearColor(s32 r, s32 g, s32 b, s32 a);
	virtual void clearColor(f32 r, f32 g, f32 b, f32 a);
	virtual void clearColor(double r, double g, double b, double a);

	virtual void clearColor(u8 * color);
	virtual void clearColor(s8 * color);
	virtual void clearColor(u16 * color);
	virtual void clearColor(s16 * color);
	virtual void clearColor(u32 * color);
	virtual void clearColor(s32 * color);
	virtual void clearColor(f32 * color);
	virtual void clearColor(double * color);

	virtual void clearColor(color4ub & color);
	virtual void clearColor(color4b & color);
	virtual void clearColor(color4us & color);
	virtual void clearColor(color4s & color);
	virtual void clearColor(color4ui & color);
	virtual void clearColor(color4i & color);
	virtual void clearColor(color4f & color);
	virtual void clearColor(color4d & color);

	virtual void clearColor(color3ub & color);
	virtual void clearColor(color3b & color);
	virtual void clearColor(color3us & color);
	virtual void clearColor(color3s & color);
	virtual void clearColor(color3ui & color);
	virtual void clearColor(color3i & color);
	virtual void clearColor(color3f & color);
	virtual void clearColor(color3d & color);

	// -----------------------------------------------------------------------------

	// ???
	virtual void enableZBuffer(void);
	virtual void disableZBuffer(void);

	virtual void setrender_mode(enum render_mode mode);

	virtual void loadIdentityMatrix(void);

	virtual void multMatrix(const mat4f & mat);
	virtual void multMatrix(const mat4d & mat);
	virtual void multTransposeMatrix(const mat4f & mat);
	virtual void multTransposeMatrix(const mat4d & mat);

	virtual void beginPrimitive(enum Primitive primitive);
	virtual void endPrimitive(void);

	//--------------------------------------------------------------------------------

	virtual void vertex3(s16 x, s16 y, s16 z);
	virtual void vertex3(s32 x, s32 y, s32 z);
	virtual void vertex3(f32 x, f32 y, f32 z);
	virtual void vertex3(double x, double y, double z);
	virtual void vertex3(s16 * v);
	virtual void vertex3(s32 * v);
	virtual void vertex3(f32 * v);
	virtual void vertex3(double * v);

	//-------------------------------

	virtual void viewport(s32 x, s32 y, s32 width, s32 height);
	// virtual void GetViewport( Viewport & viewport );

	virtual void matrixMode(enum MatrixMode mode);

	virtual enum MatrixMode getMatrixMode(void);


	// lighting
	virtual bool enableLighting(void);
	virtual bool disableLighting(void);
	virtual void setLight(u32 index, Light & light);
	virtual void lightOn(u32 index);
	virtual void lightOff(u32 index);

	// bool SetRenderState( RD_STATE, bool value )
	// bool SetRenderState( Z_Buffer, Z_Cmp_Less );
	// bool GetRenderState( Z_Buffer );

	// virtual void Enablerenderer(u32 caps);
	// virtual void Disablerenderer(u32 caps);

private:
	/* internal state */
	enum MatrixMode _matrixMode;
};

}
}
}
}
} // ! ew::implementation::Graphics::renderer::OpenGL
