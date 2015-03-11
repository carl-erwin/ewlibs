#include <iostream>

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>


#include "win32_video_device.hpp"

namespace ew
{
namespace implementation
{
namespace Platform
{
namespace Win32
{
namespace DEVICE
{
namespace OUTPUT
{
namespace Video
{
namespace GRAPHIC_CARD
{

using namespace  ew::core::types;

/* Common class for openGL Direct3D  */
Win32_GraphicCard::Win32_GraphicCard() {};
Win32_GraphicCard::Win32_GraphicCard(u32) {};


u32   Win32_GraphicCard::getSystemIndex(void)
{
	return 0;
}
bool  Win32_GraphicCard::is_opened(void)
{
	return false;
}
bool  Win32_GraphicCard::is_closed(void)
{
	return false;
}

bool Win32_GraphicCard::open(void)
{
	return false;
}
bool Win32_GraphicCard::close(void)
{
	return false;
}

bool Win32_GraphicCard::setResolution(u32 width, u32 height, u32 bpp)
{
	return false;
}
void Win32_GraphicCard::flip(void) { }
void Win32_GraphicCard::setResizeableWindowFlag(bool flag) {}
void Win32_GraphicCard::setFullScreenWindowFlag(bool flag) {}

void Win32_GraphicCard::setWindowHandler(HWND hWnd)
{
	_hWnd = hWnd;
}
HWND Win32_GraphicCard::getWindowHandler(void)
{
	return _hWnd;
}
void Win32_GraphicCard::setGdiDeviceContext(HDC hDC)
{
	this->_hDC = hDC;
}

//////////////////////////////  OpenGL //////////////////////////////////////
WIN32_OpenGL_GraphicCard::WIN32_OpenGL_GraphicCard() {};
WIN32_OpenGL_GraphicCard::WIN32_OpenGL_GraphicCard(u32) {};
void WIN32_OpenGL_GraphicCard::setOpenGlrendering_context(HGLRC hRC)
{
	this->_hRC = hRC;
}


bool WIN32_OpenGL_GraphicCard::open(void)
{
	std::cerr << "ew::System::Video::DEVICE::WIN32_OpenGL_DEVICE::Open()" << " Ok." << std::endl;
	return (true);
}

bool WIN32_OpenGL_GraphicCard::close(void)
{
	std::cerr << "ew::System::Video::DEVICE::WIN32_OpenGL_DEVICE::Close()" << " Ok." << std::endl;
	return (true);
}

bool WIN32_OpenGL_GraphicCard::setResolution(u32 width, u32 height, u32 bpp)
{
	return (true);
}
void WIN32_OpenGL_GraphicCard::flip(void)
{
	/*SwapBuffers(_hDC);*/
}


////////////////////////////////// Direct 3D ///////////////////////////////
WIN32_Direct3D_GraphicCard::WIN32_Direct3D_GraphicCard() {};
WIN32_Direct3D_GraphicCard::WIN32_Direct3D_GraphicCard(u32) {};

LPDIRECT3D9       WIN32_Direct3D_GraphicCard::GetD3DInterface(void)
{
	return (m_pD3D);
}
LPDIRECT3DDEVICE9      WIN32_Direct3D_GraphicCard::GetD3DRenderingDevice(void)
{
	return (m_pd3dDevice);
}


// ----------------------------------------------------------------------

bool WIN32_Direct3D_GraphicCard::open(void)
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if (NULL == (m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return (false);

	// Set up the structure used to create the D3DDevice. Most parameters are
	// zeroed out. We set Windowed to TRUE, since we want to do D3D in a
	// window, and then set the SwapEffect to "discard", which is the most
	// efficient method of presenting the back buffer to the display.  And
	// we request a back buffer format that matches the current desktop display
	// format.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Create the Direct3D device. Here we are using the default adapter (most
	// systems only have one, unless they have multiple graphics hardware cards
	// installed) and requesting the HAL (which is saying we want the hardware
	// device rather than a software one). Software vertex processing is
	// specified since we know it will work on all cards. On cards that support
	// hardware vertex processing, though, we would see a big performance gain
	// by specifying hardware vertex processing.
	if (FAILED(m_pD3D->CreateDevice(
			   D3DADAPTER_DEFAULT,
			   D3DDEVTYPE_HAL,
			   _hWnd,
			   D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			   &d3dpp,
			   &m_pd3dDevice))) {
		// MessageBox(NULL, "Error", "Ew:System::Video::DEVICE::WIN32_Direct3D_DEVICE::Open(void)", MB_OK);
		ExitProcess(1);
		return (false);
	}

	// Device state would normally be set here

	std::cerr << "ew::System::Video::DEVICE::WIN32_Direct3d_DEVICE::Open()" << " Ok." << std::endl;
	return (true);
}

bool WIN32_Direct3D_GraphicCard::close(void)
{
	if (m_pd3dDevice != NULL)
		m_pd3dDevice->Release();
	if (m_pD3D != NULL)
		m_pD3D->Release();


	std::cerr << "ew::System::Video::DEVICE::WIN32_Direct3D_DEVICE::Close()" << " Ok." << std::endl;

	return (true);
}

bool WIN32_Direct3D_GraphicCard::setResolution(u32 width, u32 height, u32 bpp)
{
	return (true);
}

void WIN32_Direct3D_GraphicCard::flip(void)
{
	HRESULT res = m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	if (res != D3D_OK) {
		std::cerr << "error : in WIN32_Direct3D_GraphicCard::flip(void)\n" ;
		//EW_FUNC_ERROR;
	}
}

}
}
}
}
}
}
}
} // ! ew::core::Platform::Win32::DEVICE::Output::Video::GraphicCard



namespace ew
{
namespace Core
{
namespace Platform
{
namespace Win32
{
namespace Video
{

using namespace ew::Video::DEVICE;
/* -------------------------------------------------------------- */

u32  Win32_IVideo::getNumberOfVideoDevices(void)
{
	/* detect number of video cards and returns it */
	return (1); /* :-) */
}

IGraphicCard * Win32_IVideo::getGraphicCard(u32 index)
{
	// me must chec what type of render is declared
	IGraphicCard * ivideoDev = 0;// new WIN32_OpenGL_GraphicCard(index);
	return ivideoDev;
}

void   Win32_IVideo::releaseGraphicCard(IGraphicCard * igraphicCard)
{
	/* destroy allocs ,  close ... */
	delete igraphicCard;
}

}
}
}
}
} // ! ew::core::Platform::Win32::Video
