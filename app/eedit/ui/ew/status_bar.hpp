#pragma once


#include <ew/graphics/graphics.hpp>
#include <ew/graphics/gui/widget/window/window.hpp>
#include <ew/graphics/gui/event/event.hpp>

#include "widget_layout.hpp"

namespace  eedit
{

class status_bar : public ew::graphics::gui::widget
{
public:
	status_bar(ew::graphics::gui::widget * parent);

	virtual ~status_bar();

	bool set_content(const s32 * str, size_t nr_char = 0);

	virtual bool render();
private:
	s32 * m_content = nullptr;
	size_t m_content_sz = 0;
};


}
