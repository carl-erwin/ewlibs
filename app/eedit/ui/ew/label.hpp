#include <vector>

#include "ew/graphics/gui/widget/widget.hpp"
using namespace ew::graphics::gui;

///
// TODO: #include "<ew/graphics/opengl.hpp>"
#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"
using namespace ew::implementation::graphics::rendering::opengl;


class label : public ew::graphics::gui::widget
{
	std::vector<s32> text; // unicode
public:

	label()
	{
		horizontal_policy().type()  = alignment::relative;
		horizontal_policy().ratio() = 100;
		vertical_policy().type()  = alignment::relative;
		vertical_policy().ratio() = 100;

		set_name("label");
	}

	virtual bool set_text(const char * str)
	{

		if (str == nullptr)
			return true;

		text.clear();
		while (*str) {
			text.push_back(*str);
			str++;
		}

		return true;
	}

	virtual const std::vector<s32> & get_text() const
	{
		return text;
	}

	virtual bool render()
	{
#if 0
		app_log << __PRETTY_FUNCTION__ << "\n";
		app_log <<  " x = " << x() << ", ";
		app_log <<  " y = " << y() << ", ";
		app_log <<  " w = " << width() << ", ";
		app_log <<  " h = " << height() << "\n";
#endif
		if (get_font() == nullptr) {
			assert(0);
			return true;
		}

		ew::graphics::colors::color3ub text_color(0xFF, 0xFF, 0xFF);

		ew::graphics::fonts::font_print(*get_font(),
						0, 0, width(), height(),
						text_color, &text[0], text.size());

		return true;
	}

	virtual bool on_mouse_button_press(const ew::graphics::gui::events::button_event * ev)
	{
		return static_cast<widget *>(get_parent())->on_mouse_button_press(ev);
	}

	virtual bool on_mouse_button_release(const ew::graphics::gui::events::button_event * ev)
	{
		return static_cast<widget *>(get_parent())->on_mouse_button_release(ev);
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

