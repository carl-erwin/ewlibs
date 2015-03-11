#include <d3d9.h>

#include "system/types/types.hpp"
using namespace  Ew::System::Types;

#include "system/video/win32_device.hpp"

namespace ew
{

namespace System
{

namespace Video
{

namespace Device
{

// ----------------------------------------------------------------------
void WIN32_Direct3D_Device::SetResizeableWindowFlag(bool flag)
{
	resizeable_window = flag;
}

void WIN32_Direct3D_Device::SetWindowHandler(HWND hWnd)
{
	this->hWnd = hWnd;
}

bool WIN32_Direct3D_Device::Open(void)
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

	// Create the Direct3D device. Here we are using the default adapter ( most
	// systems only have one, unless they have multiple graphics hardware cards
	// installed ) and requesting the HAL ( which is saying we want the hardware
	// device rather than a software one ). Software vertex processing is
	// specified since we know it will work on all cards. On cards that support
	// hardware vertex processing, though, we would see a big performance gain
	// by specifying hardware vertex processing.
	if (FAILED(m_pD3D->CreateDevice(
			   D3DADAPTER_DEFAULT,
			   D3DDEVTYPE_HAL,
			   hWnd,
			   D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			   &d3dpp,
			   &m_pd3dDevice))) {
		MessageBox(NULL, "Error", "Ew:System::Video::Device::WIN32_Direct3D_Device::Open( void )", MB_OK);
		ExitProcess(1);
		return (false);
	}

	// Device state would normally be set here

	std::cerr << "Ew::System::Video::Device::WIN32_Direct3d_Device::Open(  )" << " Ok." << std::endl;
	return (true);
}

bool WIN32_Direct3D_Device::Close(void)
{
	if (m_pd3dDevice != NULL)
		m_pd3dDevice->Release();
	if (m_pD3D != NULL)
		m_pD3D->Release();


	std::cerr << "Ew::System::Video::Device::WIN32_Direct3D_Device::Close(  )" << " Ok." << std::endl;

	return (true);
}

bool WIN32_Direct3D_Device::SetResolution(u32 width, u32 height, u32 bpp)
{
	return (true);
}

void WIN32_Direct3D_Device::Flip(void)
{
	HRESULT res = m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	if (res != D3D_OK) {
		EW_FUNC_ERROR;
	}
}

/// -----------------------------------------------------------------------


void WIN32_OpenGL_Device::SetResizeableWindowFlag(bool flag)
{
	resizeable_window = flag;
}

void WIN32_OpenGL_Device::SetWindowHandler(HWND hWnd_)
{
	this->hWnd = hWnd_;
}

void WIN32_OpenGL_Device::SetOpenglrendering_context(HGLRC hRC_)
{
	this->hRC = hRC_;
}

void WIN32_OpenGL_Device::SetGdiDeviceContext(HDC hDC_)
{
	this->hDC = hDC_;
}


bool WIN32_OpenGL_Device::Open(void)
{
	std::cerr << "Ew::System::Video::Device::WIN32_OpenGL_Device::Open(  )" << " Ok." << std::endl;
	return (true);
}

bool WIN32_OpenGL_Device::Close(void)
{
	std::cerr << "Ew::System::Video::Device::WIN32_OpenGL_Device::Close(  )" << " Ok." << std::endl;
	return (true);
}

bool WIN32_OpenGL_Device::SetResolution(u32 width, u32 height, u32 bpp)
{
	return (true);
}

void WIN32_OpenGL_Device::Flip(void)
{
	SwapBuffers(hDC);
}

// -----------------------------------------------------------------------

} // ! namespace ew::System::Video::Device

} // ! namespace ew::System::Video

} // ! namespace ew::System

} // ! namespace ew

