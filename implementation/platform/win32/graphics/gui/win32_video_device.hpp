#pragma once

#include <windows.h>
#include <d3d9.h>

#include <ew/ew_config.hpp>


#include <ew/core/device/idevice.hpp>
#include <ew/video/device/graphic_card/igraphic_card.hpp>

// --------------------------------------------------------------------------------------------------------

namespace ew
{
namespace implementation
{
namespace platform
{
namespace win32
{
namespace devices
{
namespace output
{
namespace video
{

using namespace ew::core::DEVICE;

/* Common class for openGL Direct3D  */
class EW_GRAPHICS_EXPORT Win32_GraphicCard : public ew::Video::DEVICE::IGraphicCard
{
public:

	Win32_GraphicCard();
	Win32_GraphicCard(u32);

	const char * class_name(void) const
	{
		return "Graphic Card";
	};
	const char * getName() const
	{
		return 0;
	};

	virtual u32  getSystemIndex(void);
	virtual bool is_opened(void);
	virtual bool is_closed(void);

	virtual bool open(void);
	virtual bool close(void);

	virtual bool setResolution(u32 width, u32 height, u32 bpp);
	virtual void flip(void);
	virtual void setResizeableWindowFlag(bool flag);
	virtual void setFullScreenWindowFlag(bool flag);

	// move this below
protected:
	u32      _systemIndex;  // system index
	bool     _wasOpened; // open/close flag
	//
public:

	virtual void setWindowHandler(HWND hWnd);
	virtual HWND getWindowHandler(void);
	virtual void setGdiDeviceContext(HDC hDC);

protected:
	// device context
	HDC             _hDC;       // Private GDI Device Context
	HWND            _hWnd;      // Holds Our Window Handle, comes from CreateWindow
};


class EW_PLATFORM_EXPORT WIN32_OpenGL_GraphicCard : public Win32_GraphicCard
{
public:
	WIN32_OpenGL_GraphicCard();
	WIN32_OpenGL_GraphicCard(u32);
	void setOpenGlrendering_context(HGLRC hRC);
	virtual bool open(void);
	virtual bool close(void);
	virtual bool setResolution(u32 width, u32 height, u32 bpp);
	virtual void flip(void);

protected:
	HGLRC           _hRC;       // Permanent Rendering Context
};

class EW_PLATFORM_EXPORT WIN32_Direct3D_GraphicCard : public Win32_GraphicCard
{
public:
	WIN32_Direct3D_GraphicCard();
	WIN32_Direct3D_GraphicCard(u32);

	virtual bool open(void);
	virtual bool close(void);
	virtual bool setResolution(u32 width, u32 height, u32 bpp);
	virtual void flip(void);

	// for the dx renderer
public:
	LPDIRECT3D9      GetD3DInterface(void);
	LPDIRECT3DDEVICE9     GetD3DRenderingDevice(void);

protected:
	LPDIRECT3D9             m_pD3D;       // Used to create the D3DDevice
	LPDIRECT3DDEVICE9       m_pd3dDevice; // Our rendering device
};


}
}
}
}
}
}
} // ! ew::core::Platform::Win32::DEVICE::Output::Video::GraphicCard

// --------------------------------------------------------------------------------------------------------


namespace ew
{
namespace core
{
namespace platform
{
namespace win32
{
namespace video
{

using namespace ew::Video::DEVICE;

// put this else where : interface != device
class EW_PLATFORM_EXPORT Win32_IVideo   // : public ew::Video::IVideoInterface
{
public:

	const char * class_name(void) const
	{
		return 0;
	};
	const char * getName() const
	{
		return 0;
	};


	virtual u32    getNumberOfVideoDevices(void);
	virtual IGraphicCard * getGraphicCard(u32 index);
	virtual void   releaseGraphicCard(IGraphicCard * igraphicCard);
};

}
}
}
}
} // ! ew::core::Platform::Win32::Video
