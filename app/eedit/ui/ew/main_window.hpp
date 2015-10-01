#pragma once

#include <list>
#include <condition_variable>

// api
#include "editor_message.h"
#include "editor_event_queue.h"
#include "editor_buffer.h"

// ui
#include <ew/filesystem/file.hpp>
#include <ew/graphics/graphics.hpp>
#include <ew/graphics/gui/widget/window/window.hpp>
#include <ew/graphics/gui/event/event.hpp>

#include "widget_layout.hpp"
#include "status_bar.hpp"


// internal
using namespace ew::filesystem; // TODO::remove

namespace  eedit
{


class text_view;

class main_window : public ew::graphics::gui::window
{
    class main_window_private;
    class main_window_private * m_priv;

public:
    main_window(const main_window &) = delete;
    main_window & operator = (const main_window &) = delete;

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

    virtual bool set_event_queue(struct editor_event_queue_s * q);
    virtual struct editor_event_queue_s * event_queue(void);

    virtual void process_event_queue(void);
};



} // ! namespace eedit
