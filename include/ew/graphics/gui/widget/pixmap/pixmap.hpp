#pragma once

// Ew
#include <ew/ew_config.hpp>

// base of gui objects
#include <ew/graphics/gui/widget/widget.hpp>
#include <ew/graphics/gui/widget/widget_properties.hpp>

#include <ew/graphics/gui/widget/window/window.hpp>

// renderer
#include <ew/graphics/renderer/renderer.hpp>
#include <ew/graphics/renderer/rendering_context.hpp>

namespace ew
{
namespace graphics
{
namespace gui
{

class window;

class EW_GRAPHICS_EXPORT pixmap   /* : public ew::graphics::gui::Widget */
{
public:
	pixmap(ew::graphics::gui::window * window);
	virtual ~pixmap();

	virtual bool resize(u32 width, u32 height);

private:
	class private_data;
	class private_data * d;

	friend class ew::graphics::gui::gui;
	friend class ew::graphics::gui::display;
	friend class ew::graphics::gui::window;
	friend class ew::graphics::rendering::rendering_context;
};
}
}
}
