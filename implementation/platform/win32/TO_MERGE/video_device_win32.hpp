#ifndef __EW_SYSTEM_VIDEO_DEVICE_WIN32_DEVICE_HPP__
#define __EW_SYSTEM_VIDEO_DEVICE_WIN32_DEVICE_HPP__

#include <windows.h>
#include <d3d9.h>

#include "system/types/types.hpp"
using namespace  ew::System::Types;

#include "device.hpp"

namespace ew
{
namespace system
{
namespace device
{


/* Common class for openGL Direct3D  */
class EW_DECLSPEC WIN32_VideoDevice : public  ew::System::Device::IVideoDevice
{
public:
	WIN32_VideoDevice(HWND hWnd,  HDC             _hDC;)

	virtual void setWindowHandler(HWND hWnd)
	{
		_hWnd = hWnd;
	};
	virtual HWND getWindowHandler(void)
	{
		return _hWnd;
	};

	virtual void setGdiDeviceContext(HDC hDC);

	virtual bool open(void) = 0;
	virtual bool close(void) = 0;
	virtual bool setResolution(u32 width, u32 height, u32 bpp) = 0;
	virtual void flip(void) = 0;

protected:
	// device context
	HDC             _hDC;       // Private GDI Device Context
	HWND            _hWnd;       // Holds Our Window Handle, comes from CreateWindow
};


class EW_DECLSPEC WIN32_OpenGL_VideoDevice : public WIN32_VideoDevice
{
public:



	virtual void setrendering_context(HGLRC hRC);
	virtual void setrendering_context(HGLRC hRC);


	virtual bool open(void);
	virtual bool close(void);
	virtual bool setResolution(u32 width, u32 height, u32 bpp);
	virtual void flip(void);

protected:
	HGLRC           _hRC;       // Permanent Rendering Context
};

class EW_DECLSPEC WIN32_Direct3D_VideoDevice : public WIN32_VideoDevice
{
public:
	virtual bool open(void);
	virtual bool close(void);
	virtual bool setResolution(u32 width, u32 height, u32 bpp);
	virtual void flip(void);

	// for the dx renderer
public:
	LPDIRECT3D9      GetD3DInterface(void)
	{
		return (m_pD3D);
	}
	LPDIRECT3DDEVICE9     GetD3DRenderingDevice(void)
	{
		return (m_pd3dDevice);
	}



protected:
	LPDIRECT3D9             m_pD3D;       // Used to create the D3DDevice
	LPDIRECT3DDEVICE9       m_pd3dDevice; // Our rendering device
};

} // ! namespace ew::System::Video

} // ! namespace ew::System

}

#endif // ! __EW_SYSTEM_VIDEO_DEVICE_SDL_DEVICE_HPP__
