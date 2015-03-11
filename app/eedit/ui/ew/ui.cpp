#include <iostream>


#include "ew/graphics/gui/event/push.hpp"
#include "ew/graphics/gui/widget/widget_properties.hpp"
#include "ew/graphics/gui/widget/window/window_properties.hpp"

#include "ui.h"
#include "main_window.hpp"

namespace eedit
{

// move to ui/ew
class ew_ui_interface : public user_interface
{
public:
	ew::graphics::gui::display * get_display()
	{
		return gui_dpy;
	}

	virtual void release()
	{
	}

	bool setup(application * app)
	{
		////////////////////////
		// setup display
		////////////////////////

		if (ew::graphics::gui::init() == false) {
			app_log << "ew::graphics::gui::init() :: error" << "\n";
			return false;
		}

		// set event polling thread model :-)
		ew::graphics::gui::setSinglethreadEventPollingMode();

		// alloc display before creating any widget
		gui_dpy = new ew::graphics::gui::display();
		if (gui_dpy->open() == false) {
			return false;
		}

		if (ew::graphics::rendering::init() == false) {
			app_log << "ew::graphics::gui::init() :: error" << "\n";
			return false;
		}

		////////////////////////
		// create main window //
		////////////////////////

		ew::graphics::gui::window_properties properties;

		properties.x = 0;
		properties.y = 0;
		properties.width = app_width;
		properties.height = app_height;
		properties.have_decoration = true;

		properties.use_offscreen_buffer = app->offscreen_buffer_flag();
		properties.clear_color = ew::graphics::color4ub(0xe9, 0xe9, 0xe9);

		bool ret = gui_dpy->lock();
		assert(ret);
		if (ret == false) {
			// ...
		}

		m_main_window = new main_window(gui_dpy, properties);

		// setup core event queue : the core send event here
		m_main_window->set_event_queue(new eedit::core::event_queue<eedit::core::event *>);

		ret = gui_dpy->unlock();
		assert(ret);
		if (ret == false) {
			// ...
		}

		return true;
	}


	void process_events()
	{
		bool block   = !true;
		u32  timeout = 10;

		gui_dpy->poll_events(block, timeout);
		u32 nr = gui_dpy->get_event_dispatcher()->get_queue_size();
		if (nr == 0)
			return;

		gui_dpy->get_event_dispatcher()->dispatch_events(nr);
	}


	bool main_loop()
	{
		bool ret = true;

		main_window * win = m_main_window;

		win->show();
		while (win->loop() == true) {
			// TODO: abstract for all registered ui
			// here application is tied to ew/ui
			win->process_event_queue();
			process_events();
		}

		gui_dpy->lock();
		delete win;
		gui_dpy->unlock();

		gui_dpy->close();
		delete gui_dpy;
		gui_dpy = nullptr;

		return ret;
	}

private:
	ew::graphics::gui::display * gui_dpy = nullptr;

	class main_window * m_main_window;

	// move to user configuration: init file
	u32 app_width  = 800;
	u32 app_height = 600;
};

eedit::user_interface * new_ew_ui()
{
	return new ew_ui_interface();
}


}
