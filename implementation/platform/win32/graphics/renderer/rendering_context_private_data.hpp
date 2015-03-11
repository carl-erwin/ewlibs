#pragma once

#include <windows.h>

// Ew
#include <ew/graphics/gui/widget/window/window.hpp>

#include "../gui/window_private_data.hpp"


namespace ew
{
namespace graphics
{
namespace rendering
{

class rendering_context::private_data   // add lock ?
{
public:
	ew::graphics::gui::window * widget; // attached to

	HGLRC hglRC;

	friend class ew::graphics::gui::Widget;
	friend class ew::graphics::gui::window;
};



}
}
}
