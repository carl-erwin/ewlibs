#pragma once

// ew
#include <ew/ew_config.hpp>

// ew core
#include <ew/core/types/types.hpp>

#include <ew/graphics/gui/widget/widget_properties.hpp>

namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::types;

class EW_GRAPHICS_EXPORT window_properties : public widget_properties
{
public:
	window_properties() : is_fullscreen(false), have_decoration(true)
	{

	}

	bool is_fullscreen;
	bool have_decoration;
	color4ub clear_color;
};

}
}
}
