#ifndef _EW_GRAPHICS_rendering_DIRECT3D_rendering_HH
#define _EW_GRAPHICS_rendering_DIRECT3D_rendering_HH

#include "ew_config.hpp"


#include "maths/forward_declaration.hpp"
using namespace ew::maths;

#include "graphics/forward_declaration.hpp"
using namespace ew::graphics;

#include "graphics/color.hpp"
#include "graphics/light.hpp"

#include "graphics/renderer/renderer.hpp"

#include "system/video/win32_device.hpp"

namespace ew
{

namespace graphics
{

namespace renderer
{

namespace Direct3D
{

class EW_DECLSPEC renderer : public renderer
{
public:

	virtual bool BeginScene(void);
	virtual void EndScene(void);
	virtual void EndSceneAndFlipBuffers(void);
	virtual void FlipBuffers(void);

	virtual void ClearBuffer(bool color_buffer = true, bool z_buffer = false, bool stencil_buffer = false);

	// -----------------------------------------------------------------------------------------------------

	virtual void ClearColor(u8   r, u8   g, u8   b, u8   a) ;
	virtual void ClearColor(s8   r, s8   g, s8   b, s8   a) ;
	virtual void ClearColor(u16  r, u16  g, u16  b, u16  a) ;
	virtual void ClearColor(s16  r, s16  g, s16  b, s16  a) ;
	virtual void ClearColor(u32  r, u32  g, u32  b, u32  a) ;
	virtual void ClearColor(s32  r, s32  g, s32  b, s32  a) ;
	virtual void ClearColor(f32  r, f32  g, f32  b, f32  a) ;
	virtual void ClearColor(double  r, double  g, double  b, double  a) ;

	virtual void ClearColor(u8  *  color) ;
	virtual void ClearColor(s8  *  color) ;
	virtual void ClearColor(u16  * color) ;
	virtual void ClearColor(s16  * color) ;
	virtual void ClearColor(u32  * color) ;
	virtual void ClearColor(s32  * color) ;
	virtual void ClearColor(f32  * color) ;
	virtual void ClearColor(double  * color) ;

	virtual void ClearColor(color4ub & color) ;
	virtual void ClearColor(color4b  & color) ;
	virtual void ClearColor(color4us & color) ;
	virtual void ClearColor(color4s  & color) ;
	virtual void ClearColor(color4ui & color) ;
	virtual void ClearColor(color4i  & color) ;
	virtual void ClearColor(color4f  & color) ;
	virtual void ClearColor(color4d  & color) ;

	virtual void ClearColor(color3ub & color) ;
	virtual void ClearColor(color3b  & color) ;
	virtual void ClearColor(color3us & color) ;
	virtual void ClearColor(color3s  & color) ;
	virtual void ClearColor(color3ui & color) ;
	virtual void ClearColor(color3i  & color) ;
	virtual void ClearColor(color3f  & color) ;
	virtual void ClearColor(color3d  & color) ;

	// -----------------------------------------------------------------------------------------------------

	virtual void EnableZBuffer(void);
	virtual void DisableZBuffer(void);

	virtual void Setrender_mode(enum render_mode mode);

	virtual void LoadIdentityMatrix(void);

	virtual void MultMatrix(const mat4f & mat);
	virtual void MultMatrix(const mat4d & mat);
	virtual void MultTransposeMatrix(const mat4f & mat);
	virtual void MultTransposeMatrix(const mat4d & mat);

	virtual void BeginPrimitive(enum Primitive primitive);

	//-------------------------------

	virtual void Vertex3(s16 x, s16 y, s16 z);
	virtual void Vertex3(s32 x, s32 y, s32 z);
	virtual void Vertex3(f32 x, f32 y, f32 z);
	virtual void Vertex3(double x, double y, double z);
	virtual void Vertex3(s16 * v);
	virtual void Vertex3(s32 * v);
	virtual void Vertex3(f32 * v);
	virtual void Vertex3(double * v);

	//-------------------------------

	virtual void EndPrimitive(void);

	virtual void Viewport(s32 x, s32 y, s32 width, s32 height);
	virtual void MatrixMode(enum MatrixMode mode);
	virtual enum MatrixMode GetMatrixMode(void);


	virtual bool EnableLighting(void);
	virtual bool DisableLighting(void);
	virtual void SetLight(u32 index, Light & light);
	virtual void LightOn(u32 index);
	virtual void LightOff(u32 index);

	virtual void SetD3Ddevice(ew::System::Video::Device::WIN32_Direct3D_Device * EwD3Ddevice)
	{
		m_EwD3Ddevice = EwD3Ddevice;
		m_D3Ddevice = EwD3Ddevice->GetD3DRenderingDevice();
	}

	virtual ew::System::Video::Device::WIN32_Direct3D_Device * GetD3Ddevice()
	{
		return (m_EwD3Ddevice);
	}

	// ------------------------------

private:

	// ------ Direct3d Device -------
	ew::System::Video::Device::WIN32_Direct3D_Device * m_EwD3Ddevice;
	IDirect3DDevice9 * m_D3Ddevice;

	// ---- put here renderer state ----------
	color4ub m_ClearColor;

	// matrix
	enum MatrixMode m_matrix_mode;

	D3DMATRIX m_projection_matrix;
	D3DMATRIX m_world_matrix;
	D3DMATRIX m_view_matrix;
	D3DMATRIX m_texture_matrix;
	D3DMATRIX m_color_matrix;    // ?

	D3DMATRIX * m_current_matrix;
	// ----------------------------------------


};

} // ! ew::graphics::renderer::Direct3D

} // ! ew::graphics::renderer

} // ! ew::graphics

} // ! Ew



#endif
