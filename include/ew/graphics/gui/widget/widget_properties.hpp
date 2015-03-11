#pragma once

// Ew
#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::types;

// TODO: add properties mask
// prop.flags.use_coords
// prop.flags.use_dimension

class EW_GRAPHICS_EXPORT widget_properties
{
public:
	widget_properties() :
		x(0),
		y(0),
		width(1),
		height(1),
		min_width(1),
		min_height(1),
		max_width(1),
		max_height(1),
		is_resizeable(true),
		use_offscreen_buffer(false)
	{
	}

	s32 x;
	s32 y;
	u32 width;
	u32 height;

	/* TODO: { */
	u32 min_width;
	u32 min_height;
	u32 max_width;
	u32 max_height;
	/* } */

	bool is_resizeable;
	bool use_offscreen_buffer;
};

}
}
}
