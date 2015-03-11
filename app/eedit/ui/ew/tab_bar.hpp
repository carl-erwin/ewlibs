#include <list>
#include <list>
#include <vector>
#include "ew/graphics/gui/widget/widget.hpp"
#include "ew/graphics/Font.hpp"
#include "font_utils.hpp"


#include "button_label.hpp"

///
// TODO: #include "<ew/graphics/opengl.hpp>"
#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"
using namespace ew::implementation::graphics::rendering::opengl;

namespace eedit
{

/*
  TODO: create api

  add_entry("text");
 */
class tab_bar : public ew::graphics::gui::widget
{
public:
	std::list <button_label *> button_list;

	horizontal_layout * hori_layout = nullptr;
	ew::core::object  * m_context = nullptr;

public:
	tab_bar(ew::graphics::gui::widget * parent)
	{
		set_name("tab_bar");

		assert(parent);
		parent->add_widget(this);
	}

	virtual ~tab_bar()
	{
		auto it_end = button_list.end();
		auto it = button_list.begin();
		while (it != it_end) {
			delete *it;
			++it;
		}

		delete hori_layout;
	}

	virtual bool add_tab(const char * text, ew::core::object * context = nullptr)
	{
		auto ft = get_font();

		button_label   *  btn_label = new button_label(this);

		m_context = context;

		//std::vector<char> buffer;
		//buffer.insert((char*)text, (char*)text + strlen(text));
		//btn_label->set_name( &buffer[0]);
		btn_label->set_name("button_label:text");
		btn_label->set_font(ft);
		btn_label->set_text(text);

		u32 text_width = ::ft_compute_text_width(*ft, &btn_label->get_text()[0], btn_label->get_text().size());
		assert(text_width);

		u32 border_width  = 4;
		btn_label->set_width(text_width   + 2 * border_width);
		btn_label->set_height(this->height());

		btn_label->horizontal_policy().type() = alignment::fixed;
		btn_label->horizontal_policy().ratio() = 100;
		btn_label->vertical_policy().type() = alignment::relative;
		btn_label->vertical_policy().ratio() = 100;

		//
		button_list.push_back(btn_label);

		assert(btn_label->width());
		assert(btn_label->height());

		add_widget(btn_label);

		return true;
	}


	/*
	 *      TODO: load the font that fits the menu_bar height
	 *
	 *     provide an api to populate tab_bar
	 *

	    -------------------------------------
	    [  tab1 | tab2 | ... | tab(n) ][<][>]
	    -------------------------------------
	    // add widget tab(tab_bar)
	*/

	virtual bool on_create(const widget_event * ev)
	{

#ifndef NDEBUG
		auto p = dynamic_cast<widget *>(get_parent());
		assert(p);
		assert(p->width());
		assert(p->height());
#endif

		hori_layout = new horizontal_layout;
		set_layout(hori_layout);

		return widget::on_create(ev);
	}

	virtual bool render()
	{

		// delim
#if 1
		u8 r = 0x2D;
		u8 g = 0x2D;
		u8 b = 0x2D;
		u8 a = 0xFF;
		ew_glColor4ub(r, g, b, a);
		glBegin(GL_QUADS);
		{
			//up
			glVertex2i(0          , 0);    // left
			glVertex2i(0 + width(), 0);    // right

			//down
			glVertex2i(0 + width(), height());    // right
			glVertex2i(0          , height());    // left
		}
		glEnd();
#endif

		// render children
		return widget::render();
	}


	virtual bool on_key_press(const ew::graphics::gui::events::keyboard_event * ev)
	{

		app_log << __PRETTY_FUNCTION__ << "";

		widget * p = dynamic_cast<ew::graphics::gui::widget *>(get_parent());
		if (p) {
			return p->on_key_press(ev);
		}
		return false;
	}

	virtual bool on_key_release(const ew::graphics::gui::events::keyboard_event * ev)
	{

		app_log << __PRETTY_FUNCTION__ << "";


		widget * p = dynamic_cast<ew::graphics::gui::widget *>(get_parent());
		if (p) {
			return p->on_key_release(ev);
		}
		return false;
	}


	virtual bool on_mouse_button_press(const ew::graphics::gui::events::button_event * ev)
	{

		app_log << __PRETTY_FUNCTION__ << "\n";

		return false;
	}

	virtual bool on_mouse_button_release(const ew::graphics::gui::events::button_event * ev)
	{
		app_log << __PRETTY_FUNCTION__ << "\n";
		return false;
	}


	virtual bool set_width(u32 w)
	{
		app_log << __PRETTY_FUNCTION__ << " : w " << w << "\n";
		return widget::set_width(w);
	}

	virtual bool set_height(u32 h)
	{
		app_log << __PRETTY_FUNCTION__ << " : h " << h << "\n";
		return widget::set_height(h);
	}

};

} // ! namespace eedit
