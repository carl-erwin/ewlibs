#include <iostream>
#include <unistd.h>


//
#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"
using namespace ew::implementation::graphics::rendering::opengl;

#include "core/log.hpp"

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
	s32 X     = 0;
	u32 W     = width();
	u32 minH  = 10;

	u32 maxH  = height() - minH + 1;
	s32 Y     = m_begin_ratio * (maxH);

	assert(m_end_ratio >= m_begin_ratio);

	u32 H    = (m_end_ratio - m_begin_ratio) * (maxH);

	H = ew::maths::max<u32>(H, minH);

	// save scrool bar pos
	m_scroll_X = X;
	m_scroll_Y = Y;
	m_scroll_W = W;
	m_scroll_H = H;

	// info
	if (0) {
		app_log << "---------------------------\n";
		app_log << "m_click_y_pos       = " << m_click_y_pos << "\n";
		app_log << "scroll_bar x()      = " << x() << "\n";
		app_log << "scroll_bar y()      = " << y() << "\n";
		app_log << "scroll_bar width()  = " << width() << "\n";
		app_log << "scroll_bar height() = " << height() << "\n";
		app_log << "m_begin_ratio       = " << m_begin_ratio << "\n";
		app_log << "m_end_ratio         = " << m_end_ratio << "\n";
		app_log << "m_scroll_X          = " << m_scroll_X << "\n";
		app_log << "m_scroll_Y          = " << m_scroll_Y << "\n";

		app_log << "minH                = " << minH << "\n";
		app_log << "maxH                = " << maxH << "\n";
		app_log << "X                   = " << X << "\n";
		app_log << "Y                   = " << Y << "\n";
		app_log << "W                   = " << W << "\n";
		app_log << "H                   = " << H << "\n";
	}

	// bg : d6 d2 d0
	// TODO: widget->set_background_color(color4ub)
	ew::graphics::color4ub bg(0xf1, 0xf1, 0xf1, 0xf1);
	ew_glColor4ub(bg.r, bg.g, bg.b, bg.a);
	glBegin(GL_QUADS);
	{
		s32 wid_Y = 0;
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
	app_log << __PRETTY_FUNCTION__ << "\n";

	m_click_y_pos = ev->y;

	app_log << "ev->x = " << ev->x << "\n";
	app_log << "ev->y = " << ev->y << "\n";

	app_log << "m_scroll_X = " << m_scroll_X << "\n";
	app_log << "m_scroll_Y = " << m_scroll_Y << "\n";
	app_log << "m_scroll_W = " << m_scroll_W << "\n";
	app_log << "m_scroll_H = " << m_scroll_H << "\n";

	if ((m_click_y_pos >= m_scroll_Y)
	    && (m_click_y_pos <= (m_scroll_Y + m_scroll_H))) {

		m_is_pressed = true;
		m_click_offset = ev->y - m_scroll_Y;
		app_log << " m_click_offset = " << m_click_offset << "\n";
		app_log << " m_is_pressed   = " << m_is_pressed << "\n";
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
