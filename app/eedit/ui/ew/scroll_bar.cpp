#include <iostream>
#include <unistd.h>


//
#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"
using namespace ew::implementation::graphics::rendering::opengl;

#include "core/log/log.hpp"

#include "scroll_bar.hpp"

#include "text_view.hpp"


using namespace ew::graphics::gui::events;



namespace eedit
{

/*
  TODO:

   - draw a bg
   - buttons
   - scrool bar
*/

bool vertical_scroll_bar::on_pointer_motion(const pointer_event * ev)
{
    // app_log << __PRETTY_FUNCTION__ << " ev->x = " << ev->x << "\n";
    // app_log << __PRETTY_FUNCTION__ << " ev->y = " << ev->y << "\n";

    // app_log << "m_scroll_X = " << m_scroll_X << "\n";
    // app_log << "m_scroll_Y = " << m_scroll_Y << "\n";
    // app_log << "m_scroll_W = " << m_scroll_W << "\n";
    // app_log << "m_scroll_H = " << m_scroll_H << "\n";

    // app_log << "m_is_pressed = " << m_is_pressed << "\n";

    // move to widget handling
    if (((ev->x >= m_scroll_X) && (ev->x <= (m_scroll_X + m_scroll_W)))
        && ((ev->y >= m_scroll_Y) && (ev->y <= (m_scroll_Y + m_scroll_H)))) {
        m_is_over = true;
    } else {
        m_is_over = false;
    }


    if (m_is_pressed == true) {

        // app_log << __PRETTY_FUNCTION__ << " height() = " << height() << "\n";

        // TODO: replace 10 by scroll bar minimum height
        float new_ratio = (float)(ev->y - m_click_offset) / ((float)height() - 10 + 1);

        new_ratio = ew::maths::max<float>(new_ratio, 0.0f);
        new_ratio = ew::maths::min<float>(new_ratio, 1.0f);

        // app_log << __PRETTY_FUNCTION__ << " ** new ratio = " << new_ratio << "\n";

        set_begin_ratio(new_ratio);
        m_txt_view->set_start_offset_by_ratio(new_ratio);
        m_has_changed = true;
    }

    return true;
}

bool vertical_scroll_bar::set_begin_ratio(float ratio)
{
    m_begin_ratio = ew::maths::max<float>(0.0f, ratio);
    m_begin_ratio = ew::maths::min<float>(m_begin_ratio, 1.0f);

    assert(m_begin_ratio <= 1.0f);

    return true;
}

float vertical_scroll_bar::get_begin_ratio() const
{
    // assert(m_end_ratio >= m_begin_ratio);
    return m_begin_ratio;
}

bool vertical_scroll_bar::set_end_ratio(float ratio)
{
    m_end_ratio  = ew::maths::max<float>(m_begin_ratio, m_end_ratio);
    m_end_ratio  = ew::maths::min<float>(ratio, 1.0f);

    assert(m_end_ratio >= m_begin_ratio);

    return true;
}

float vertical_scroll_bar::get_end_ratio() const
{
    // assert(m_end_ratio >= m_begin_ratio);
    return m_end_ratio;
}


bool vertical_scroll_bar::on_key_press(const  ew::graphics::gui::events::keyboard_event * ev)
{

    // forward to view
    if (m_txt_view == nullptr)
        return true;


    ew::graphics::gui::events::keyboard_event new_ev = *ev;
    new_ev.x -= m_txt_view->x();
    return m_txt_view->on_key_press(&new_ev);
}

bool vertical_scroll_bar::on_key_release(const  ew::graphics::gui::events::keyboard_event * ev)
{
    // forward to view
    if (m_txt_view == nullptr)
        return true;

    ew::graphics::gui::events::keyboard_event new_ev = *ev;
    new_ev.x -= m_txt_view->x();
    return m_txt_view->on_key_release(&new_ev);
}

bool vertical_scroll_bar::on_mouse_wheel_up(const button_event * ev)
{
    if (m_txt_view == nullptr)
        return true;

    return m_txt_view->on_mouse_wheel_up(ev);
}

bool vertical_scroll_bar::on_mouse_wheel_down(const button_event * ev)
{
    if (m_txt_view == nullptr)
        return true;

    return m_txt_view->on_mouse_wheel_down(ev);
}


bool vertical_scroll_bar::render()
{
    int32_t X     = 0;
    uint32_t W     = width();
    uint32_t minH  = 10;

    uint32_t maxH  = height() - minH + 1;
    int32_t Y     = m_begin_ratio * (maxH);

    assert(m_end_ratio >= m_begin_ratio);

    uint32_t H    = (m_end_ratio - m_begin_ratio) * (maxH);

    H = ew::maths::max<uint32_t>(H, minH);

    // save scrool bar pos
    m_scroll_X = X;
    m_scroll_Y = Y;
    m_scroll_W = W;
    m_scroll_H = H;

    // info
    if (0) {
        app_logln(-1, "---------------------------");
        app_logln(-1, "m_click_y_pos       = %u", m_click_y_pos);
        app_logln(-1, "scroll_bar x()      = %u", x());
        app_logln(-1, "scroll_bar y()      = %u", y());
        app_logln(-1, "scroll_bar width()  = %u", width());
        app_logln(-1, "scroll_bar height() = %u", height());
        app_logln(-1, "m_begin_ratio       = %u", m_begin_ratio);
        app_logln(-1, "m_end_ratio         = %u", m_end_ratio);
        app_logln(-1, "m_scroll_X          = %u", m_scroll_X);
        app_logln(-1, "m_scroll_Y          = %u", m_scroll_Y);
        app_logln(-1, "minH                = %u", minH);
        app_logln(-1, "maxH                = %u", maxH);
        app_logln(-1, "X                   = %u", X);
        app_logln(-1, "Y                   = %u", Y);
        app_logln(-1, "W                   = %u", W);
        app_logln(-1, "H                   = %u", H);
    }

    // bg : d6 d2 d0
    // TODO: widget->set_background_color(color4ub)
    ew::graphics::color4ub bg(0xf1, 0xf1, 0xf1, 0xf1);
    ew_glColor4ub(bg.r, bg.g, bg.b, bg.a);
    glBegin(GL_QUADS);
    {
        int32_t wid_Y = 0;
        glVertex2i(X    , wid_Y);           // up left
        glVertex2i(X    , wid_Y + height());    // down left
        glVertex2i(X + W, wid_Y + height());    // down right
        glVertex2i(X + W, wid_Y);           // up right
    }
    glEnd();

    // the scrool bar
    if (m_is_over == true) {
        ew_glColor4ub(0xde, 0xde, 0xde, 0xff); // blue
    } else {
        ew_glColor4ub(0xbc, 0xbc, 0xbc, 0xff);
    }

    if (m_is_pressed == true) {
//		ew_glColor4ub(0xff, 0x00, 0x00, 0xff); // red
    }

    glBegin(GL_QUADS);
    {
        // border
        glVertex2i(X     , Y);           // up left
        glVertex2i(X     , Y + H - 1);   // down left

        glVertex2i(X  + W, Y);           // up right
        glVertex2i(X  + W, Y + H - 1);       // down right

        glVertex2i(X    , Y);           // up left
        glVertex2i(X + W, Y);           // up right

        glVertex2i(X     , Y + H - 1);  // down left
        glVertex2i(X  + W, Y + H - 1);  // down right

        // 2,3 bar
        glVertex2i(X + W / 2 - 4, Y + H / 2 - 1); // left
        glVertex2i(X + W / 2 + 4, Y + H / 2 - 1); // right
        // glVertex2i(X + W/2 - 4, Y + H/2 - 0);  // left
        // glVertex2i(X + W/2 + 4, Y + H/2 - 0);  // right
        glVertex2i(X + W / 2 - 4, Y + H / 2 + 1); // left
        glVertex2i(X + W / 2 + 4, Y + H / 2 + 1); // right
    }
    glEnd();


    return true;
}


bool vertical_scroll_bar::on_focus_in(const widget_event * ev)
{
    // move to common
    m_is_over = false;
    if (((ev->x >= m_scroll_X) && (ev->x <= (m_scroll_X + m_scroll_W)))
        && ((ev->y >= m_scroll_Y) && (ev->y <= (m_scroll_Y + m_scroll_H)))) {
        m_is_over = true;
        return true;
    }

    return false;
}


bool vertical_scroll_bar::on_focus_out(const widget_event * ev)
{
    if (m_is_over == true) {
        m_is_over = false;
        return true;
    }

    return false;
}

// TODO: remember the click offset for move
bool vertical_scroll_bar::on_mouse_button_press(const ew::graphics::gui::events::button_event * ev)
{
    app_logln(-1, "%s", __PRETTY_FUNCTION__);


    m_click_y_pos = ev->y;

    app_logln(-1, "ev->x = %u", ev->x);
    app_logln(-1, "ev->y = %u", ev->y);
    app_logln(-1, "m_scroll_X = %u", m_scroll_X);
    app_logln(-1, "m_scroll_Y = %u", m_scroll_Y);
    app_logln(-1, "m_scroll_W = %u", m_scroll_W);
    app_logln(-1, "m_scroll_H = %u", m_scroll_H);

    if ((m_click_y_pos >= m_scroll_Y)
        && (m_click_y_pos <= (m_scroll_Y + m_scroll_H))) {

        m_is_pressed = true;
        m_click_offset = ev->y - m_scroll_Y;
        app_log(-1, " m_click_offset = %u", m_click_offset);
        app_log(-1, " m_is_pressed   = %u", m_is_pressed);
    }

    return true;
}

bool vertical_scroll_bar::on_mouse_button_release(const ew::graphics::gui::events::button_event * ev)
{
    m_click_y_pos = 0;
    m_click_offset = 0;
    m_is_pressed = false;

    // move to common
    m_is_over = false;
    if (((ev->x >= m_scroll_X) && (ev->x <= (m_scroll_X + m_scroll_W)))
        && ((ev->y >= m_scroll_Y) && (ev->y <= (m_scroll_Y + m_scroll_H)))) {
        m_is_over = true;
    }


    return true;
}

} // ! namespace eedit
