#pragma once

#include "ew/graphics/graphics.hpp"
#include "ew/graphics/gui/widget/widget.hpp"
#include "ew/graphics/gui/widget/window/window.hpp"
#include "ew/graphics/gui/event/event.hpp"
#include "ew/graphics/gui/event/push.hpp"



namespace eedit
{
using namespace ew::graphics::gui;

class scroll_area;
class text_view;

class vertical_scroll_bar : public ew::graphics::gui::widget
{
public:
	ew::graphics::gui::widget * m_parent;
	bool m_is_over = false;
	bool m_is_pressed = false;

	bool m_has_changed = false;

	float m_begin_ratio = 0.0f;
	float m_end_ratio   = 0.0f;

	bool m_show = true;

	s32 m_scroll_X = 0;
	s32 m_scroll_W = 0;
	s32 m_scroll_Y = 0;
	s32 m_scroll_H = 0;
	u64 m_last_rdr_begin   = 0;
	u64 m_last_rdr_end     = 0;
	u64 m_last_buffer_size = 0;

	s32 m_click_y_pos  = 0;
	s32 m_click_offset = 0;

	text_view * m_txt_view = nullptr;
public:
	vertical_scroll_bar(ew::graphics::gui::widget * parent)
	{
		m_parent = parent;
		set_name("vertical scroll bar");
	}

	virtual ~vertical_scroll_bar()
	{

	}

	void set_textview(text_view * txt_view)
	{
		m_txt_view = txt_view;
	}

	text_view * get_textview() const
	{
		return m_txt_view;
	}

	// TODO: transform in ratio set_ratio(float value)
	virtual bool set_begin_ratio(float ratio);
	float get_begin_ratio() const;

	virtual bool set_end_ratio(float ratio);
	float get_end_ratio() const;

	// object
	virtual const char * class_name() const
	{
		return "scroll_bar";
	}


	// actions
	virtual bool show()
	{
		m_show = true;
		return true;
	}

	virtual bool hide()
	{
		m_show = false;
		return true;
	}

	virtual bool resize(u32 width, u32 height)
	{
		return set_width(width) && set_height(height);
	}

	virtual bool on_mouse_button_press(const ew::graphics::gui::events::button_event * ev);

	virtual bool on_mouse_button_release(const ew::graphics::gui::events::button_event * ev);

	virtual bool on_pointer_motion(const ew::graphics::gui::events::pointer_event * ev);

	virtual bool on_focus_in(const ew::graphics::gui::events::widget_event * ev);
	virtual bool on_focus_out(const ew::graphics::gui::events::widget_event * ev);

	virtual bool on_key_press(const  ew::graphics::gui::events::keyboard_event * ev);

	virtual bool on_key_release(const  ew::graphics::gui::events::keyboard_event * ev);

	virtual bool on_mouse_wheel_up(const button_event * ev);

	virtual bool on_mouse_wheel_down(const button_event * ev);

	virtual bool render();

	void set_changed_flag(bool flag)
	{
		m_has_changed = flag;
	}

	bool has_changed()
	{
		return m_has_changed;
	}

};


class vscroll_bar_button1 : public ew::graphics::gui::widget
{
public:
	vscroll_bar_button1(ew::graphics::gui::widget * parent)
	{
		m_parent = parent;
		init_border_color();
		set_focus_out_color();


		set_name("vscroll_bar_button1");
	}

	void init_border_color()
	{
		border_color.r = 0x77;
		border_color.g = 0x77;
		border_color.b = 0x77;
		border_color.a = 0xff;
	}

	void set_focus_out_color()
	{
		bg.r = 0x77;
		bg.g = 0x77;
		bg.b = 0x77;
		bg.a = 0xff;
	}

	void set_focus_in_color()
	{
		bg.r = 0x00;
		bg.g = 0x00;
		bg.b = 0xff;
		bg.a = 0xff;
	}

	virtual ~vscroll_bar_button1()
	{

	}

	virtual bool resize(u32 w, u32 h)
	{

		app_log << __PRETTY_FUNCTION__ << "w = " << w << " h = " << h << "\n";

		bool ret = widget::resize(w, h);
		return ret;
	}

	virtual bool render()
	{

		s32 X = 0;
		s32 Y = 0;
		s32 W = width();
		s32 H = height();

//        app_log << __PRETTY_FUNCTION__ << " : X("<<X<<") Y("<<Y<<") W("<<W<<") H("<<H<<")\n";

		// border
		ew_glColor4ub(border_color.r, border_color.g, border_color.b, border_color.a);
		glBegin(GL_LINES);
		{
			glVertex2i(X     , Y);           // up left
			glVertex2i(X     , Y + H - 1);   // down left

			glVertex2i(X  + W, Y);           // up right
			glVertex2i(X  + W, Y + H - 1);       // down right

			glVertex2i(X    , Y);           // up left
			glVertex2i(X + W, Y);           // up right

			glVertex2i(X     , Y + H - 1);  // down left
			glVertex2i(X  + W, Y + H - 1);  // down right
		}
		glEnd();

		ew_glColor4ub(bg.r, bg.g, bg.b, bg.a);

		glBegin(GL_TRIANGLES);
		{
			glVertex2i(X + W / 2       , Y + H / 2 - margin); // up
			glVertex2i(X + W / 2 - margin, H / 2 + margin); // down left
			glVertex2i(X + W / 2 + margin, H / 2 + margin); // down right
		}
		glEnd();

		if (anim_on == true) {

			widget * p = this->m_parent;
			ew::core::object * gp = static_cast<ew::core::object *>(p)->get_parent();
			gp = static_cast<ew::core::object *>(gp)->get_parent();
			ew::graphics::gui::push_draw_event(static_cast<widget *>(gp));

			switch (anim_dir) {

			case 0: {
			} break;

			case 1: {
				margin++;
				if (W / 2 + margin + 2 >= (s32)width())
					anim_dir = -1;
			}
			break;

			case -1: {
				margin--;
				if (margin <= (s32)2)
					anim_dir = 1;
			}
			break;

			}

		}

		return true;
	}

	virtual bool on_focus_in(const ew::graphics::gui::events::widget_event * ev)
	{

		app_log << __FUNCTION__ << "\n";

		set_focus_in_color();
		anim_on = true;

		widget * p = this->m_parent;
		ew::core::object * gp = static_cast<ew::core::object *>(p)->get_parent();
		gp = static_cast<ew::core::object *>(gp)->get_parent();
		ew::graphics::gui::push_draw_event(static_cast<widget *>(gp));

		return true;
	}

	virtual bool on_focus_out(const ew::graphics::gui::events::widget_event * ev)
	{

		app_log << __FUNCTION__ << "\n";

		set_focus_out_color();

		anim_on = false;

		widget * p = this->m_parent;
		ew::core::object * gp = static_cast<ew::core::object *>(p)->get_parent();
		gp = static_cast<ew::core::object *>(gp)->get_parent();
		ew::graphics::gui::push_draw_event(static_cast<widget *>(gp));

		return true;
	}

	virtual bool on_mouse_button_press(const ew::graphics::gui::events::button_event * ev);

	virtual bool on_mouse_button_release(const ew::graphics::gui::events::button_event * ev);


private:
	ew::graphics::gui::widget * m_parent = nullptr;
	ew::graphics::color4ub border_color;
	ew::graphics::color4ub bg;
	s32  margin = 3;
	s32  anim_dir = 1;
	bool anim_on = false;
};

class vscroll_bar_button2 : public ew::graphics::gui::widget
{
public:
	vscroll_bar_button2(ew::graphics::gui::widget * parent)
	{
		m_parent = parent;
		// init_border_color();
		// set_focus_out_color();

		set_name("vscroll_bar_button2");
	}

	virtual ~vscroll_bar_button2()
	{

	}


	virtual bool resize(u32 w, u32 h)
	{

		app_log << __PRETTY_FUNCTION__ << "w = " << w << " h = " << h << "\n";

		bool ret = widget::resize(w, h);
		return ret;
	}

	virtual bool render()
	{

		// TODO: widget->set_background_olor(color4ub)
		ew::graphics::color4ub bg(0x77, 0x77, 0x77, 0xff);
		ew_glColor4ub(bg.r, bg.g, bg.b, bg.a);

		s32 X = 0;
		s32 Y = 0;
		s32 W = width();
		s32 H = height();

		// app_log << __PRETTY_FUNCTION__ << " : X("<<X<<") Y("<<Y<<") W("<<W<<") H("<<H<<")\n";


		// border
		glBegin(GL_LINES);
		{
			glVertex2i(X     , Y);           // up left
			glVertex2i(X     , Y + H - 1);   // down left

			glVertex2i(X  + W, Y);           // up right
			glVertex2i(X  + W, Y + H - 1);       // down right

			glVertex2i(X    , Y);           // up left
			glVertex2i(X + W, Y);           // up right

			glVertex2i(X     , Y + H - 1);  // down left
			glVertex2i(X  + W, Y + H - 1);  // down right
		}
		glEnd();

		s32 margin = 5;
		glBegin(GL_TRIANGLES);
		{
			glVertex2i(W - margin, margin);     // up right
			glVertex2i(X + margin, margin);     // up left
			glVertex2i(X + W / 2,  H - margin); // down
		}
		glEnd();

		return true;
	}


	virtual bool on_mouse_button_press(const ew::graphics::gui::events::button_event * ev);

	virtual bool on_mouse_button_release(const ew::graphics::gui::events::button_event * ev);


private:
	ew::graphics::gui::widget * m_parent = nullptr;
};


/* vertical scroll area
   encapsulates:
   button to scrool upper
   the scroll bar
   button to scrool upper
   button to scrool lower
 */

class scroll_area : public ew::graphics::gui::widget
{
public:
	bool m_animate = false;
	bool m_new_anim = false;
	enum { scroll_none, scroll_up, scroll_down } scroll_dir = scroll_none;
public:
	scroll_area(ew::graphics::gui::widget * parent)
	{
		m_parent = parent;
		set_name("vertical scroll area");

		m_button1    = new vscroll_bar_button1(this);
		m_button2    = new vscroll_bar_button2(this);
		m_scroll_bar = new vertical_scroll_bar(this);
	}

	virtual ~scroll_area()
	{
	}

	virtual bool on_create(const widget_event * ev)
	{

		app_log << __PRETTY_FUNCTION__ << "\n";

		ew::graphics::gui::vertical_layout * vert_layout = new vertical_layout;
		this->set_layout(vert_layout);

//#define SCROLL_BAR_WITH_BUTTON 1

#ifdef  SCROLL_BAR_WITH_BUTTON
		m_button1->set_y(0);
		m_button1->set_width(this->width());
		m_button1->set_height(this->width());
		m_button1->vertical_policy().type() = alignment::fixed;
		this->add_widget(m_button1);
#endif

		m_scroll_bar->set_y(0);
		m_scroll_bar->set_width(this->width());
		m_scroll_bar->vertical_policy().type() = alignment::relative;
		m_scroll_bar->vertical_policy().ratio() = 100;
		this->add_widget(m_scroll_bar);

#ifdef SCROLL_BAR_WITH_BUTTON


		vscroll_bar_button1     *    m_button3    = new vscroll_bar_button1(this);
		m_button3->set_y(0);
		m_button3->set_width(this->width());
		m_button3->set_height(this->width());
		m_button3->vertical_policy().type() = alignment::fixed;
		this->add_widget(m_button3);

		m_button2->set_y(0);
		m_button2->set_width(this->width());
		m_button2->set_height(this->width());
		m_button2->vertical_policy().type() = alignment::fixed;
		this->add_widget(m_button2);
#endif

		return true;
	}

	// TODO: transform in ratio set_ratio(float value)
	virtual bool set_begin_ratio(float ratio)
	{
		if (m_scroll_bar) {
			return m_scroll_bar->set_begin_ratio(ratio);
		}
		return false;
	}

	float get_begin_ratio() const
	{
		if (m_scroll_bar) {
			return m_scroll_bar->get_begin_ratio();
		}
		return 0.0f;
	}

	virtual bool set_end_ratio(float ratio)
	{
		if (m_scroll_bar) {
			return m_scroll_bar->set_end_ratio(ratio);
		}
		return false;
	}

	float get_end_ratio() const
	{
		if (m_scroll_bar) {
			return m_scroll_bar->get_end_ratio();
		}
		return false;
	}


	void set_changed_flag(bool flag)
	{

		if (m_scroll_bar) {
			m_scroll_bar->m_has_changed = flag;
		}
	}

	bool has_changed()
	{

		if (m_scroll_bar) {
			return m_scroll_bar->m_has_changed;
		}

		return false;
	}


	void set_textview(text_view * txt_view)
	{
		if (m_scroll_bar) {
			m_scroll_bar->set_textview(txt_view);
		}
	}

	text_view * get_textview() const
	{
		if (m_scroll_bar == nullptr) {
			return nullptr;
		}

		return m_scroll_bar->get_textview();
	}

///////
	virtual bool resize(u32 w, u32 h)
	{

		app_log << __PRETTY_FUNCTION__ << "w = " << w << " h = " << h << "\n";

		bool ret = widget::resize(w, h);
		return ret;
	}

	virtual bool animate()
	{

		// for now scroll up 1 line :-)
		// TODO: enable timer for periodic push draw event

		bool need_redraw = false;

		switch (scroll_dir) {
		case scroll_up: {
			app_log << __PRETTY_FUNCTION__ << " auto sroll\n";
			assert(0);
			//          auto txtv = get_textview();


//           txtv->scroll_up(1, true);

			need_redraw = true;
			if (m_new_anim == true) {
				// first delay
				// ew::core::time::sleep(anim_sleep);
			} else {
				// ew::core::time::sleep(anim_sleep);
			}

			m_new_anim = false;

		}
		break;

		case scroll_down: {
			app_log << __PRETTY_FUNCTION__ << " auto scroll down\n";
			assert(0);
//            auto txtv = get_textview();
//            txtv->scroll_down(1, true);
			need_redraw = true;
			if (m_new_anim == true) {
				// first delay
				// ew::core::time::sleep(anim_sleep);
			} else {
				// ew::core::time::sleep(anim_sleep);
			}

			m_new_anim = false;

		}
		break;

		default: {

		} break;
		}

		if (need_redraw == true) {
			widget * p = this->m_parent;
			ew::core::object * gp = static_cast<ew::core::object *>(p)->get_parent();
			ew::graphics::gui::push_draw_event(static_cast<widget *>(gp));
		}

		return true;
	}

	virtual bool render()
	{

		animate();

		//   app_log << __PRETTY_FUNCTION__ << " : X("<<x()<<") Y("<<y()<<") W("<<width()<<") H("<<height()<<")\n";
		//   app_log << __PRETTY_FUNCTION__ << " scroll area = " << this << "\n";

		ew::graphics::color4ub bg(0x77, 0x77, 0x77, 0xff);
		ew_glColor4ub(bg.r, bg.g, bg.b, bg.a);

		s32 X = 0;
		s32 Y = 0;
		s32 W = width();
		s32 H = height();

		// borders
		glBegin(GL_LINES);
		{
			glVertex2i(X    , Y);    // up
			glVertex2i(X    , H);    // down

			glVertex2i(X + W, Y);    // up
			glVertex2i(X + W, H);    // down
		}
		glEnd();

		bool ret = widget::render();
		return ret;
	}

private:
	ew::graphics::gui::widget  * m_parent     = nullptr;
	vscroll_bar_button1     *    m_button1    = nullptr;
	vscroll_bar_button2     *    m_button2    = nullptr;
	vertical_scroll_bar     *    m_scroll_bar = nullptr;
// u32 anim_sleep = 0;
};

inline bool vscroll_bar_button1::on_mouse_button_press(const ew::graphics::gui::events::button_event * ev)
{
	auto sc   = dynamic_cast<scroll_area *>(m_parent);
	sc->m_animate = true;
	sc->m_new_anim = true;
	sc->scroll_dir = scroll_area::scroll_up;
	return true;
}

inline bool vscroll_bar_button1::on_mouse_button_release(const ew::graphics::gui::events::button_event * ev)
{
	auto sc   = dynamic_cast<scroll_area *>(m_parent);
	sc->m_animate = false;
	sc->m_new_anim = false;
	sc->scroll_dir = scroll_area::scroll_none;
	return true;
}

inline bool vscroll_bar_button2::on_mouse_button_press(const ew::graphics::gui::events::button_event * ev)
{
	auto sc   = dynamic_cast<scroll_area *>(m_parent);
	sc->m_animate = true;
	sc->m_new_anim = true;
	sc->scroll_dir = scroll_area::scroll_down;
	return true;
}

inline bool vscroll_bar_button2::on_mouse_button_release(const ew::graphics::gui::events::button_event * ev)
{
	auto sc   = dynamic_cast<scroll_area *>(m_parent);
	sc->m_animate = false;
	sc->m_new_anim = false;
	sc->scroll_dir = scroll_area::scroll_none;
	return true;
}



} // ! namespace eedit
