#pragma once

#include <ew/core/threading/thread.hpp>

#include "mywindow.hpp"

using namespace ew::core::threading;

namespace test
{
namespace graphics
{

class myMenu :  public ew::graphics::gui::window
{
public:
	myMenu(ew::graphics::gui::display * dpy, widget * parent, window_properties & properties);

	virtual ~myMenu();

	virtual bool on_draw(const widget_event * ev);

	virtual bool on_key_press(const keyboard_event * ev);
	virtual bool on_key_release(const keyboard_event * ev);

	virtual bool on_mouse_button_press(const button_event * ev);

	virtual bool on_mouse_button_release(const button_event * ev);

	virtual bool on_close(const widget_event * ev);

	virtual bool show();

	virtual bool hide();

	bool isActive;

	thread * renderthread;

	static void renderFunc(myMenu * menu);
};

} // ! namespace graphics
} // ! namespace test
