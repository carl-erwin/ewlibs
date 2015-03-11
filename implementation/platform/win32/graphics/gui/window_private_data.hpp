#pragma once

#include <windows.h>

#include <ew/ew_config.hpp>
#include <ew/graphics/gui/widget/window/window.hpp>

#include <iostream>
#include <list>

#include <ew/maths/maths.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/thread.hpp>

#include <ew/graphics/renderer/rendering_context.hpp>


namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::types;
using namespace ew::core::threading;

extern std::list<Widget *> widget_list;
typedef std::list<Widget *>::iterator widget_listIt;

void widget_eventHandler(void);

class Window::private_data
{
public:
	private_data()
		:
		_showOk(false),
		_event_thread(0),
		hglRC(0),
		hDC(0),
		hWnd(0) {}

	bool _showOk;

	thread * _event_thread;
	RECT   windowRect;
	HGLRC hglRC;
	HDC   hDC;
	HWND  hWnd;

	WidgetProperties _properties;
	ew::graphics::rendering::rendering_context * rendering_context;

public:

	static LRESULT CALLBACK Win32_MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Window::private_data::build_window()
	static void widgetthread(Window * widget);
};


/// ---

inline
void  Window::private_data::widgetthread(Window * widget)
{
	static bool _wasRegistered = false;

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW /* | CS_OWNDC */,
		Win32_MsgProc ,
		0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"ew::graphics::gui::window", NULL
	};

	if (_wasRegistered == false) {
		if (!RegisterClassEx(&wc)) {        // Attempt To Register The Window Class
			MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			ExitProcess(1);
		}
		_wasRegistered = true;
	}

	/*
	win->d->windowRect.top =  ( GetSystemMetrics(0) / 2 ) - ( win->d->_height / 2 );
	win->d->windowRect.left = ( GetSystemMetrics(1) / 2 ) - ( win->d->_width / 2 );
	win->d->windowRect.right = win->d->_width;
	win->d->windowRect.bottom = win->d->_height + 24;
	*/

	widget->d->windowRect.top = 0;
	widget->d->windowRect.left = 0;
	widget->d->windowRect.right = widget->d->_properties.width;
	widget->d->windowRect.bottom = widget->d->_properties.height;

	if (AdjustWindowRect(&widget->d->windowRect, WS_OVERLAPPEDWINDOW, FALSE) == FALSE) {
		std::cerr << "AdjustWindowRect() : Error" << std::endl;
		ExitProcess(1);
	}

	widget->d->windowRect.top = ew::maths::max((s32) 0, (s32) widget->d->windowRect.top);
	widget->d->windowRect.left = ew::maths::max((s32) 0, (s32) widget->d->windowRect.left);

	std::cerr << "window rect "
		  << " " << widget->d->windowRect.top
		  << " " << widget->d->windowRect.left
		  << " " << widget->d->windowRect.right
		  << " " << widget->d->windowRect.bottom
		  << std::endl;

	// Create the application's window
	widget->d->hWnd =
		CreateWindow("ew::graphics::gui::window",
			     "Ew OGL",
			     WS_OVERLAPPEDWINDOW,
			     widget->d->windowRect.top,
			     widget->d->windowRect.left ,
			     widget->d->windowRect.right,
			     widget->d->windowRect.bottom ,
			     NULL,   // GetDesktopWindow(), /* parent */
			     NULL,
			     wc.hInstance,
			     NULL);

	if (! widget->d->hWnd) {
		MessageBox(NULL, "Can't Create Window", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		ExitProcess(1);
	}

	// choose pixel format
	static PIXELFORMATDESCRIPTOR pfd = {  // pfd Tells Windows How We Want Things To Be
		sizeof(PIXELFORMATDESCRIPTOR),         // Size Of This Pixel Format Descriptor
		1,                 // Version Number
		// Format Must Support Window,
		// Format Must Support OpenGL
		// Must Support Double Buffering
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,         // Request An RGBA Format
		24,          // Select Our Color Depth
		0, 0, 0, 0, 0, 0,   // Color Bits Ignored
		0,          // No Alpha Buffer
		0,          // Shift Bit Ignored
		0,          // No Accumulation Buffer
		0, 0, 0, 0,         // Accumulation Bits Ignored
		16,          // 16Bit Z-Buffer (Depth Buffer)
		0,          // No Stencil Buffer
		0,          // No Auxiliary Buffer
		PFD_MAIN_PLANE,    // Main Drawing Layer
		0,          // Reserved
		0, 0, 0      // Layer Masks Ignored
	};


	if (!(widget->d->hDC = GetDC(widget->d->hWnd))) {             // Did We Get A Device Context?
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		ExitProcess(1);
	}

	int PixelFormat;
	if (!(PixelFormat = ChoosePixelFormat(widget->d->hDC, &pfd))) {          // Did Windows Find A Matching Pixel Format?
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		ExitProcess(1);
	}

	if (!SetPixelFormat(widget->d->hDC, PixelFormat, &pfd)) {        // Are We Able To Set The Pixel Format?
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		ExitProcess(1);
	}

	// new rendering_context
	widget->d->rendering_context = new rendering_context(widget);

	if (widget->lockDrawingContext() == false) {
		MessageBox(NULL, "Can't use the OpenGL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		ExitProcess(1);
	}

	if (widget->unlockDrawingContext() == false) {
		MessageBox(NULL, "Can't use the OpenGL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		ExitProcess(1);
	}

	ShowWindow(widget->d->hWnd, SW_SHOW);   // Show The Window
	SetForegroundWindow(widget->d->hWnd);   // Slightly Higher Priority
	SetFocus(widget->d->hWnd);              // Sets Keyboard Focus To The Window
	UpdateWindow(widget->d->hWnd);          //

	widget->d->_showOk = true;
	widget_list.push_back(widget);

	widget_eventHandler();
	thread_exit(0);
}


inline
LRESULT CALLBACK Window::private_data::Win32_MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// the window is identified with hWnd
	Window * win = 0;
	Window * widTmp = 0;
	for (widget_listIt it = widget_list.begin(); it != widget_list.end(); ++it) {
		widTmp = (Window *)(*it);
		if (widTmp->d->hWnd == hWnd) {
			win = widTmp;
			break ;
		}
	}

	// no window found
	/*
	WM_CREATE est envoye à une fenêtre au moment de sa creation.

	WM_PAINT est envoye lorsqu'une partie de la zone client doit être redessinee.

	WM_ERASEBKGND est utilise pour demander l'effacement d'une partie de la zone client.

	WM_SYSCOMMAND est envoye pour le traitement de differents evènements pouvant survenir (fenêtre minimisee, restauree, ...).

	WM_ACTIVATE est envoye lorsqu'une fenêtre est activee ou desactivee.

	WM_MOVE est envoye lorsque la fenêtre a ete deplacee.

	WM_SIZE est envoye lorsque la taille de la fenêtre à ete modifiee.

	WM_CLOSE indique que l'utilisateur demande la fermeture de l'application (en cliquant sur la 'croix' ou en pressant ALT+F4).

	WM_DESTROY indique que la fenêtre est detruite.
	*/

	std::cerr << "Win32_MsgProc :: message == " << msg  << std::endl;
	switch (msg) {
	case WM_CREATE: {
		std::cerr << "Win32_MsgProc :: WM_CREATE" << std::endl;
		return 0;
	}
	break;

	case WM_DESTROY: {
		widget_list.remove(win);
		std::cerr << "Win32_MsgProc ::WM_DESTROY" << std::endl;
		return 0;
	}
	break ;

	case WM_CLOSE: {
		std::cerr << "Win32_MsgProc ::WM_CLOSE" << std::endl;
		if (win) {
			win->lock();
			if (win->on_close() == true) {
				widget_list.remove(win);
				PostQuitMessage(0);
			}
			win->unlock();
		}
		return 0;

	}
	break ;

	case WM_PAINT: {
		std::cerr << "Win32_MsgProc ::WM_PAINT" << std::endl;
		if (win) {
			win->lockDrawingContext();
			ValidateRect(hWnd, NULL);
			win->unlockDrawingContext();
		}

		return 0;
	}
	break ;

	case WM_KEYDOWN: {
		std::cerr << "Win32_MsgProc ::WM_KEYDOWN" << std::endl;
		return 0;
	}
	break;
	case WM_KEYUP: {
		std::cerr << "Win32_MsgProc :: WM_KEYUP" << std::endl;
		return 0;
	}
	break;

	case WM_SIZE: {
		std::cerr << "Win32_MsgProc :: WM_SIZE" << "\n";
		if (win /* (wParam == SIZE_RESTORED)*/) {
			win->lock();
			win->lockDrawingContext();
			win->d->_properties.width = (u32) lParam & 0xFFFF;
			win->d->_properties.height = ((u32) lParam >> 16) & 0xFFFF;
			win->on_resize(win->d->_properties.width, win->d->_properties.height);
			win->unlockDrawingContext();
			win->unlock();
		}
		return 0;
	}
	break ;


	case WM_MOVE: {
		std::cerr << "Win32_MsgProc :: WM_MOVE" << "\n";
		if (win /* (wParam == SIZE_RESTORED)*/) {
			std::cerr << "x = " << ((u32) lParam & 0xFFFF) << "\n";
			std::cerr << "y = " << (((u32) lParam >> 16) & 0xFFFF) << "\n";
			win->lock();
			win->lockDrawingContext();
			win->d->_properties.x = ((u32) lParam & 0xFFFF);
			win->d->_properties.y = (((u32) lParam >> 16) & 0xFFFF);
			win->on_move(win->d->_properties.x, win->d->_properties.y);
			win->unlockDrawingContext();
			win->unlock();
		}
		return 0;
	}
	break ;

	case WM_MOUSEMOVE: {
		std::cerr << "Win32_MsgProc :: WM_MOUSEMOVE\n";
		std::cerr << "x = " << ((u32) lParam & 0xFFFF) << "\n";
		std::cerr << "y = " << (((u32) lParam >> 16) & 0xFFFF) << "\n";
		win->lock();
	}
	break ;

	default: {
		std::cerr << "Win32_MsgProc :: default ( " << msg << ")\n";
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	}

	return 0;

}
}
}
}
