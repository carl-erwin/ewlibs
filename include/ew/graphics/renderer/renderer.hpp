#pragma once

#include <ew/ew_config.hpp>

#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>

#include <ew/maths/maths.hpp>

#include <ew/graphics/color.hpp>
#include <ew/graphics/light.hpp>


namespace ew
{
namespace graphics
{
namespace rendering
{

//TODO:: remove current renderer global wraper

// TODO: move away redering.hpp
EW_GRAPHICS_EXPORT bool init();
EW_GRAPHICS_EXPORT bool quit();


using namespace ew::core::types;
using namespace ew::maths;
using namespace ew::graphics;
using namespace ew::graphics::colors;


class renderer;

enum State {
	RS_ZBUFFER,
	RS_LIGHTING,
	RS_BLEND,
	RS_CULLING
};

/* ??? */
enum StateValue {
	RSV_BLEND_ALPHA_ONE,
	RSV_CULLING_CCW,
	RSV_CULLING_CW,
	RSV_POLYGON_FILL,
	RSV_POLYGON_LINE,
	RSV_POLYGON_POINT
};

/*
void EnablerendererState( enum State );
void rendererStateOn( enum State );

void DisablerendererState( enum State );
void rendererStateOff( enum State );

bool GetrendererStateStatus( enum State );

bool SetrendererStateValue( enum rendererStateValue )
*/

//
struct Viewport {
	s32 x;
	s32 y;
	s32 width;
	s32 height;
};

enum BufferType {
	COLOR_BUFFER = 0,
	ZBUFFER_BUFFER,
	STENCIL_BUFFER
	// ACCUM_BUFFER,
	// PIXEL_BUFFER
};

enum render_mode {
	WIREFRAME_RENDER_MODE = 0,
	POINT_RENDER_MODE,
	FILL_RENDER_MODE
};

/*
enum RenderTarget {
FRAMEBUFFER,
TEXTURE
};
*/

enum Primitive {
	TRIANGLES_PRIMITIVE = 0,
	RECTANGLE_PRIMITIVE,
	POINTS_PRIMITIVE,
	LINES_PRIMITIVE
};

enum MatrixMode {
	UNDEFINED_MATRIX = -1,
	PROJECTION_MATRIX = 0,
	MODELVIEW_MATRIX,
	TEXTURE_MATRIX,
	COLOR_MATRIX
};


/*
Forward functions to current renderer
implementated as :
name(...) { current_renderer->name(...) }
this is not optimum but keep in mind simplicity

*/


// per thread renderer TLS
EW_GRAPHICS_EXPORT void setCurrentrenderer(renderer *);
EW_GRAPHICS_EXPORT renderer * getCurrentrenderer(void);

// ALL This function call current renderer -> Methods
// this is just forwarding
// in future we must do this inline

EW_GRAPHICS_EXPORT bool beginScene(void);
EW_GRAPHICS_EXPORT void endScene(void);

EW_GRAPHICS_EXPORT void endSceneAndFlipBuffers(void);
EW_GRAPHICS_EXPORT void flipBuffers(void);


EW_GRAPHICS_EXPORT void clearBuffer(bool color_buffer = true, bool z_buffer = false, bool stencil_buffer = false);

// EW_DECLSPEC void ClearBuffer( BufferType buffer ) = 0;

// clear Framebuffer

EW_GRAPHICS_EXPORT void clearColor(u8 r, u8 & g, u8 b, u8 a);
EW_GRAPHICS_EXPORT void clearColor(s8 r, s8 g, s8 b, s8 a);
EW_GRAPHICS_EXPORT void clearColor(u16 r, u16 g, u16 b, u16 a);
EW_GRAPHICS_EXPORT void clearColor(s16 r, s16 g, s16 b, s16 a);
EW_GRAPHICS_EXPORT void clearColor(u32 r, u32 g, u32 b, u32 a);
EW_GRAPHICS_EXPORT void clearColor(s32 r, s32 g, s32 b, s32 a);
EW_GRAPHICS_EXPORT void clearColor(f32 r, f32 g, f32 b, f32 a);
EW_GRAPHICS_EXPORT void clearColor(double r, double g, double b, double a);

EW_GRAPHICS_EXPORT void clearColor(u8 * color);
EW_GRAPHICS_EXPORT void clearColor(s8 * color);
EW_GRAPHICS_EXPORT void clearColor(u16 * color);
EW_GRAPHICS_EXPORT void clearColor(s16 * color);
EW_GRAPHICS_EXPORT void clearColor(u32 * color);
EW_GRAPHICS_EXPORT void clearColor(s32 * color);
EW_GRAPHICS_EXPORT void clearColor(f32 * color);
EW_GRAPHICS_EXPORT void clearColor(double * color);

EW_GRAPHICS_EXPORT void clearColor(color4ub & color);
EW_GRAPHICS_EXPORT void clearColor(color4b & color);
EW_GRAPHICS_EXPORT void clearColor(color4us & color);
EW_GRAPHICS_EXPORT void clearColor(color4s & color);
EW_GRAPHICS_EXPORT void clearColor(color4ui & color);
EW_GRAPHICS_EXPORT void clearColor(color4i & color);
EW_GRAPHICS_EXPORT void clearColor(color4f & color);
EW_GRAPHICS_EXPORT void clearColor(color4d & color);

EW_GRAPHICS_EXPORT void clearColor(color3ub & color);
EW_GRAPHICS_EXPORT void clearColor(color3b & color);
EW_GRAPHICS_EXPORT void clearColor(color3us & color);
EW_GRAPHICS_EXPORT void clearColor(color3s & color);
EW_GRAPHICS_EXPORT void clearColor(color3ui & color);
EW_GRAPHICS_EXPORT void clearColor(color3i & color);
EW_GRAPHICS_EXPORT void clearColor(color3f & color);
EW_GRAPHICS_EXPORT void clearColor(color3d & color);

// -------------------------

EW_GRAPHICS_EXPORT void enableZBuffer(void);

EW_GRAPHICS_EXPORT void disableZBuffer(void);

EW_GRAPHICS_EXPORT void setrender_mode(render_mode mode);

EW_GRAPHICS_EXPORT void loadIdentityMatrix(void);

EW_GRAPHICS_EXPORT void multMatrix(const mat4f & mat);
EW_GRAPHICS_EXPORT void multMatrix(const mat4d & mat);
EW_GRAPHICS_EXPORT void multTransposeMatrix(const mat4f & mat);
EW_GRAPHICS_EXPORT void multTransposeMatrix(const mat4d & mat);

EW_GRAPHICS_EXPORT void beginPrimitive(Primitive primitive);
EW_GRAPHICS_EXPORT void endPrimitive(void);
// ----------------------------

EW_GRAPHICS_EXPORT void vertex3(s16 x, s16 y, s16 z);
EW_GRAPHICS_EXPORT void vertex3(s32 x, s32 y, s32 z);
EW_GRAPHICS_EXPORT void vertex3(f32 x, f32 y, f32 z);
EW_GRAPHICS_EXPORT void vertex3(double x, double y, double z);
EW_GRAPHICS_EXPORT void vertex3(s16 * v);
EW_GRAPHICS_EXPORT void vertex3(s32 * v);
EW_GRAPHICS_EXPORT void vertex3(f32 * v);
EW_GRAPHICS_EXPORT void vertex3(double * v);

// ----------------------------

EW_GRAPHICS_EXPORT void viewport(s32 x, s32 y, s32 width, s32 height);

EW_GRAPHICS_EXPORT void matrixMode(enum MatrixMode mode);

EW_GRAPHICS_EXPORT enum rendering::MatrixMode getMatrixMode(void);

// ----------------------------

EW_GRAPHICS_EXPORT bool enableLighting(void);
EW_GRAPHICS_EXPORT bool disableLighting(void);
EW_GRAPHICS_EXPORT void setLight(u32 index, Light & light);
EW_GRAPHICS_EXPORT void lightOn(u32 index);
EW_GRAPHICS_EXPORT void lightOff(u32 index);

class EW_GRAPHICS_EXPORT renderer : virtual public ew::core::object
{
public:
	renderer() { }
	virtual ~renderer() { }

	//
	virtual bool beginScene(void) = 0;
	virtual void endScene(void) = 0;

	virtual void endSceneAndFlipBuffers(void) = 0;
	virtual void flipBuffers(void) = 0;

	// Buffer -----------------------------------------------------------------

	virtual void clearBuffer(bool color_buffer = true, bool z_buffer = false, bool stencil_buffer = false) = 0;

	// Framebuffer.cpp --------------------------------------------------------
	virtual void clearColor(u8 r, u8 g, u8 b, u8 a) = 0;
	virtual void clearColor(s8 r, s8 g, s8 b, s8 a) = 0;
	virtual void clearColor(u16 r, u16 g, u16 b, u16 a) = 0;
	virtual void clearColor(s16 r, s16 g, s16 b, s16 a) = 0;
	virtual void clearColor(u32 r, u32 g, u32 b, u32 a) = 0;
	virtual void clearColor(s32 r, s32 g, s32 b, s32 a) = 0;
	virtual void clearColor(f32 r, f32 g, f32 b, f32 a) = 0;
	virtual void clearColor(double r, double g, double b, double a) = 0;

	virtual void clearColor(u8 * color) = 0;
	virtual void clearColor(s8 * color) = 0;
	virtual void clearColor(u16 * color) = 0;
	virtual void clearColor(s16 * color) = 0;
	virtual void clearColor(u32 * color) = 0;
	virtual void clearColor(s32 * color) = 0;
	virtual void clearColor(f32 * color) = 0;
	virtual void clearColor(double * color) = 0;

	virtual void clearColor(color4ub & color) = 0;
	virtual void clearColor(color4b & color) = 0;
	virtual void clearColor(color4us & color) = 0;
	virtual void clearColor(color4s & color) = 0;
	virtual void clearColor(color4ui & color) = 0;
	virtual void clearColor(color4i & color) = 0;
	virtual void clearColor(color4f & color) = 0;
	virtual void clearColor(color4d & color) = 0;

	virtual void clearColor(color3ub & color) = 0;
	virtual void clearColor(color3b & color) = 0;
	virtual void clearColor(color3us & color) = 0;
	virtual void clearColor(color3s & color) = 0;
	virtual void clearColor(color3ui & color) = 0;
	virtual void clearColor(color3i & color) = 0;
	virtual void clearColor(color3f & color) = 0;
	virtual void clearColor(color3d & color) = 0;

	// -----------------------------------------------------------------------------

	// ???
	virtual void enableZBuffer(void) = 0;
	virtual void disableZBuffer(void) = 0;

	virtual void setrender_mode(render_mode mode) = 0;

	virtual void loadIdentityMatrix(void) = 0;

	virtual void multMatrix(const mat4f & mat) = 0;
	virtual void multMatrix(const mat4d & mat) = 0;
	virtual void multTransposeMatrix(const mat4f & mat) = 0;
	virtual void multTransposeMatrix(const mat4d & mat) = 0;

	virtual void beginPrimitive(rendering::Primitive primitive) = 0;
	virtual void endPrimitive(void) = 0;

	//--------------------------------------------------------------------------------

	virtual void vertex3(s16 x, s16 y, s16 z) = 0;
	virtual void vertex3(s32 x, s32 y, s32 z) = 0;
	virtual void vertex3(f32 x, f32 y, f32 z) = 0;
	virtual void vertex3(double x, double y, double z) = 0;
	virtual void vertex3(s16 * v) = 0;
	virtual void vertex3(s32 * v) = 0;
	virtual void vertex3(f32 * v) = 0;
	virtual void vertex3(double * v) = 0;

	//-------------------------------

	virtual void viewport(s32 x, s32 y, s32 width, s32 height) = 0;
	// virtual void GetViewport( Viewport & viewport ) = 0;

	virtual void matrixMode(enum MatrixMode mode) = 0;

	virtual enum MatrixMode getMatrixMode(void) = 0;


	// lighting
	virtual bool enableLighting(void) = 0;
	virtual bool disableLighting(void) = 0;
	virtual void setLight(u32 index, Light & light) = 0;
	virtual void lightOn(u32 index) = 0;
	virtual void lightOff(u32 index) = 0;

	// bool SetRenderState( RD_STATE, bool value )
	// bool SetRenderState( Z_Buffer, Z_Cmp_Less );
	// bool GetRenderState( Z_Buffer );

	// virtual void Enablerenderer(u32 caps) = 0;
	// virtual void Disablerenderer(u32 caps) = 0;

private:
	// internal state : active render mode , active matrix
	// such has caps
	// num light
	// num texture unit
	// extensions
};

}
}
} // ! ew::graphics::renderer
