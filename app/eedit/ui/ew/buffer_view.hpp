#include "scroll_bar.hpp"
#include "text_view.hpp"
#include "line_number.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace  eedit
{

class buffer_view : public ew::graphics::gui::widget
{
public:
	buffer_view(ew::graphics::gui::widget * parent)
	{
		set_name("buffer_view");
		set_parent(parent);
		m_line_number = new line_number_view(this);
		m_text_view  = new text_view(this);
		m_scrool_bar  = new scroll_area(this);
	}

	virtual ~buffer_view()
	{
		delete m_line_number;
		delete m_text_view;
		delete m_scrool_bar;
	}


	virtual bool set_font(std::shared_ptr<ew::graphics::fonts::font> ft)
	{
		m_line_number->set_font(ft);
		m_text_view->set_font(ft);
		m_scrool_bar->set_font(ft);

		return true;
	}

	////////

	virtual bool resize(u32 w, u32 h)
	{
		bool ret = widget::resize(w, h);
		return ret;
	}

	virtual bool render()
	{
		bool ret = widget::render();
		return ret;
	}

	virtual bool on_key_press(const  ew::graphics::gui::events::keyboard_event * ev)
	{
		m_text_view->on_key_press(ev); // TODO: translate gui pointer coords
		return true;
	}

	virtual bool on_key_release(const  ew::graphics::gui::events::keyboard_event * ev)
	{
		m_text_view->on_key_release(ev); // TODO: translate gui pointer coords
		return true;
	}


	line_number_view   *  m_line_number = nullptr;
	text_view      *     m_text_view  = nullptr;
	scroll_area     *     m_scrool_bar  = nullptr;

};


} // ! namespace  eedit
