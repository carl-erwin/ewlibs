#include "ew/graphics/gui/widget/widget.hpp"

#include "core/log.hpp"

#include "label.hpp"

namespace eedit
{

class button_label : public ew::graphics::gui::widget
{
	label * m_label = nullptr;
public:


	button_label(ew::graphics::gui::widget * parent)
	{
		m_label = new label;
		add_widget(m_label);

		set_name("button_label");
	}

	virtual ~button_label()
	{
		delete m_label;
	}

	virtual bool set_font(std::shared_ptr<ew::graphics::fonts::font> ft)
	{
		widget::set_font(ft);

		m_label->set_font(ft);
		return true;
	}

	virtual bool set_text(const char * str)
	{
		return m_label->set_text(str);
	}

	virtual const std::vector<s32> & get_text() const
	{
		return m_label->get_text();
	}


	virtual bool set_width(u32 w)
	{
		app_log << __PRETTY_FUNCTION__ << " : w " << w << "\n";
		assert(w != 0);
		// TODO: compute border
		widget::set_width(w);
		return m_label->set_width(w);
	}

	virtual bool set_height(u32 h)
	{
		app_log << __PRETTY_FUNCTION__ << " : h " << h << "\n";

		// TODO: compute border
		widget::set_height(h);
		return m_label->set_height(h);
	}

	virtual bool on_mouse_button_press(const ew::graphics::gui::events::button_event * ev)
	{
		app_log << __PRETTY_FUNCTION__ << "\n";
		return static_cast<widget *>(get_parent())->on_mouse_button_press(ev);
		return false;
	}

	virtual bool on_mouse_button_release(const ew::graphics::gui::events::button_event * ev)
	{
		app_log << __PRETTY_FUNCTION__ << "\n";
		return static_cast<widget *>(get_parent())->on_mouse_button_release(ev);
		return false;
	}

};

}
