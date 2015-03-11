#include <iostream>
#include <list>

#include <ew/graphics/gui/event/event.hpp>
#include <ew/core/object/object_locker.hpp>

// base of gui objects
#include <ew/graphics/gui/widget/window/window.hpp>

#include "../window_private_data.hpp"


namespace ew
{
namespace graphics
{
namespace gui
{
namespace EVENT
{

using namespace ew::core::types;
using namespace ew::core::threading;
using namespace ew::core::object;


Event * pumpEvent(Display * dpy)
{
	return dpy->pumpEvent();
}

bool destroyEvent(Event * event)
{
	if (event)
		delete event;
	return true;
}

class event_dispatcher_data::private_data
{
public:
	std::list<Event *> event_list;
};

event_dispatcher_data::event_dispatcher_data(Display * dpy)
	: d(new private_data())
{
	std::cerr << " event_dispatcher_data::event_dispatcher_data(dpy)";
}

event_dispatcher_data::~event_dispatcher_data()
{
	std::cerr << "event_dispatcher_data::~event_dispatcher_data() : BEGIN" << "\n";
	std::cerr << " nr Event = " << d->event_list.size() << "\n";

	std::list<Event *>::iterator end = d->event_list.end();
	std::list<Event *>::iterator it = d->event_list.begin();
	for (; it != end; ++it) {
		std::list<Event *>::iterator cur = it;
		++it;

		Event * ev = *cur;
		std::cerr << " delete event" << "\n";
		destroyEvent(ev);
		d->event_list.erase(cur);
	}

	delete d;

	std::cerr << "event_dispatcher_data::~event_dispatcher_data() : END" << "\n";
}

bool event_dispatcher_data::push(Event * event)
{
	std::cerr << "event_dispatcher_data::push(Event * event)\n";
	d->event_list.push_back(event);
	return true;
}

Event * event_dispatcher_data::pop()
{
	if (! d->event_list.size())
		return 0;

	Event * ev = *(d->event_list.begin());
	d->event_list.pop_front();
	return ev;
}

u32 event_dispatcher_data::get_queue_size()
{
	return d->event_list.size();
}

bool event_dispatcher_data::dispatch_events(u32 nr)
{
	if (nr > get_queue_size())
		nr = get_queue_size();

	std::list<Event *>::iterator end = d->event_list.end();
	for (u32 count = nr; count; --count) {
		Event * ev = d->event_list.front();
		if (ev) {
			dispatch_event(ev);
			destroyEvent(ev);
		}
		d->event_list.pop_front();
	}

	return false;
}

bool event_dispatcher_data::dispatch_event(Event * event)
{
	if (event == 0)
		return false;

	static u32 nrCalls = 0;
	std::cerr << "event_dispatcher_data::dispatch_event(Event *ev) [" << ++nrCalls << "]\n";

	ew::graphics::gui::window * widget = (ew::graphics::gui::window *)event->widget;

	if (widget && ! widget->isAvailable()) {
		// set unavailable widget flag
		return false;
	}

	ObjectLocker widget_lock(widget);

	switch (event->type) {
	case KeyPressEvent :
	case KeyReleaseEvent : {
		if (! widget->isAvailable())
			break ;

		keyboard_event * keyb_ev = static_cast<keyboard_event *>(event);

		ObjectLocker lock_win_rdr_ctx(widget->renderingContext());
		if (event->type == KeyPressEvent) {
			//              std::cerr << "KeyPressEvent\n";
			widget->on_key_press((u32) keyb_ev->key);
		} else {
			//             std::cerr << "KeyReleaseEvent\n";
			widget->on_key_release((u32) keyb_ev->key);
		}
	}
	break ;

	case ButtonPressEvent :
	case ButtonReleaseEvent : {
		if (! widget->isAvailable())
			break ;

		button_event * button_event = static_cast<button_event *>(event);

		ObjectLocker lock_win_rdr_ctx(widget->renderingContext());
		if (event->type == ButtonPressEvent) {
			std::cerr << "ButtonPressEvent\n";
			widget->on_mouse_button_press(button_event->button, button_event->x, button_event->y);
		} else {
			std::cerr << "ButtonReleaseEvent\n";
			widget->on_mouse_button_release(button_event->button, button_event->x, button_event->y);
		}
	}
	break ;

	case WidgetCloseEvent: {
		if (! widget->isAvailable())
			break ;

		ObjectLocker lock_win_rdr_ctx(widget->renderingContext());
		if (widget->on_close() == true) {
			// the widget is ok with closing
		}
	} break;

	case WidgetResizeEvent: {
		if (! widget->isAvailable())
			break ;

		std::cerr << "RESIZE EVENT\n";
		ObjectLocker lock_win_rdr_ctx(widget->renderingContext());

		struct widget_event * ev = static_cast<struct widget_event *>(event);
		widget->d->_properties.width =  ev->width;
		widget->d->_properties.height = ev->height;
		widget->on_resize(widget->d->_properties.width, widget->d->_properties.height);
	}
	break;

	case WidgetMotion_event: {
		if (! widget->isAvailable())
			break ;

		std::cerr << "MOTION EVENT\n";
		ObjectLocker lock_win_rdr_ctx(widget->renderingContext());

		struct widget_event * ev = static_cast<struct widget_event *>(event);
		widget->d->_properties.x = ev->x;
		widget->d->_properties.y = ev->y;
		widget->on_move(widget->d->_properties.x, widget->d->_properties.y);
	}
	break;

	default:
		;
	}

	return true;
}

}
}
}
}
