#pragma once


// NEW
#include <ew/graphics/gui/widget/window/window.hpp>
#include <ew/graphics/gui/widget/window/window_properties.hpp>


#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"

using namespace ew::implementation::graphics::rendering::opengl;
using namespace ew::graphics::gui;

namespace test
{
namespace graphics
{

class main_window : public ew::graphics::gui::window
{
public:
	bool loop;

public:
	main_window(ew::graphics::gui::display * dpy, window_properties & properties);

	virtual ~main_window();

	virtual bool on_key_press(const keyboard_event * ev);
	virtual bool on_mouse_button_press(const button_event * ev);
	virtual bool on_mouse_button_release(const button_event * ev);
	virtual bool on_resize(const widget_event * ev);
	virtual bool on_close(const widget_event * ev);

	window * _menu0;
};

}
}
