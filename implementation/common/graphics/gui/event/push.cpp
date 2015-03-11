#include <assert.h>

#include <ew/graphics/graphics.hpp>
#include <ew/core/time/time.hpp>
#include <ew/graphics/gui/event/event.hpp>

namespace ew
{
namespace graphics
{

namespace gui
{

using namespace ew::graphics::gui::events;

EW_GRAPHICS_EXPORT void    push_draw_event(widget * widget)
{
	struct widget_event * ev =  new struct widget_event();
	ev->type = WidgetDrawEvent;
	ev->widget = widget;
	ev->display = widget->display();
	assert(ev->display);
	ev->time = ew::core::time::get_ticks();
	ev->display->push_events((events::event **)&ev, 1);
}

}
}
}
