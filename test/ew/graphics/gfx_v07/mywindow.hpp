#pragma once

#include <ew/graphics/gui/widget/window/window.hpp>

using namespace ew::core::types;
using namespace ew::graphics::gui;

namespace test
{
namespace graphics
{

class main_window : public window
{
public:
	bool loop;

public:
	main_window(window_properties & properties);

	virtual ~main_window();

	virtual bool on_mouse_button_press(const button_event * ev);
	virtual bool on_mouse_button_release(const button_event * ev);
	virtual bool on_resize(const widget_event * ev);
	virtual bool on_close(const widget_event * ev);
	virtual bool on_key_press(const keyboard_event * ev);
	virtual bool on_move(const widget_event * ev);

	window * _menu0;

	s32 _x;
	s32 _y;
	s32 last_x;
	s32 last_y;
};


} // ! namespace graphics
} // ! namespace test
