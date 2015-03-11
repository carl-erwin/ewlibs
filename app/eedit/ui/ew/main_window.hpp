#pragma once

#include <list>
#include <ew/filesystem/file.hpp>

#include <ew/core/threading/condition_variable.hpp>

#include <ew/graphics/graphics.hpp>
#include <ew/graphics/gui/widget/window/window.hpp>
#include <ew/graphics/gui/event/event.hpp>

#include "widget_layout.hpp"
#include "status_bar.hpp"

#include "../../core/event/event.hpp"
#include "../../core/message_queue.hpp"

#include "editor_buffer.h"

using namespace ew::core::threading; // TODO remove
using namespace ew::filesystem; // TODO::remove

namespace  eedit
{


class text_view;

class main_window : public ew::graphics::gui::window
{
	class main_window_private;
	class main_window_private * m_priv;

public:
	main_window(ew::graphics::gui::display * dpy,
		    ew::graphics::gui::window_properties & properties);

	virtual ~main_window();

	virtual bool on_create(const ew::graphics::gui::events::widget_event * ev);

	virtual bool on_map(const ew::graphics::gui::events::widget_event * ev) ;
	virtual bool on_resize(const ew::graphics::gui::events::widget_event * ev);
	virtual bool on_close(const ew::graphics::gui::events::widget_event * ev);
	virtual bool on_mouse_wheel_up(const ew::graphics::gui::events::button_event * ev);
	virtual bool on_mouse_wheel_down(const ew::graphics::gui::events::button_event * ev);
	virtual bool on_mouse_button_press(const ew::graphics::gui::events::button_event * ev);
	virtual bool on_mouse_button_release(const ew::graphics::gui::events::button_event * ev);
	virtual bool on_key_press(const ew::graphics::gui::events::keyboard_event * ev);
	virtual bool on_key_release(const ew::graphics::gui::events::keyboard_event * ev);
	virtual bool on_pointer_motion(const ew::graphics::gui::events::pointer_event * ev);
	virtual bool on_pointer_enter(const ew::graphics::gui::events::pointer_event * ev);
	virtual bool on_pointer_leave(const ew::graphics::gui::events::pointer_event * ev);
	virtual bool on_draw(const ew::graphics::gui::events::widget_event * ev);

	//)
	virtual bool quit();  // move to app
	virtual bool loop(); // move to app

	virtual bool clear();
	virtual bool set_projection();
	virtual bool set_modelview();

	virtual status_bar * get_status_bar();

	virtual   ::eedit::text_view * get_text_view();

	///////

	virtual bool set_event_queue(eedit::core::event_queue<eedit::core::event *> * q);
	virtual eedit::core::event_queue<eedit::core::event *> * event_queue(void);

	virtual void process_event_queue(void);
};



} // ! namespace eedit
