#include <assert.h>

#include <iostream>
#include <list>

#include <ew/graphics/gui/event/event.hpp>
#include <ew/core/object/object_locker.hpp>

#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/mutex_locker.hpp>
#include <ew/core/threading/condition_variable.hpp>

#include <ew/core/time/time.hpp>


#include <ew/console/console.hpp>

// base of gui objects
#include <ew/graphics/gui/widget/widget.hpp>

// now not portable
#include "implementation/platform/unix/graphics/gui/window_private_data.hpp"
#include "implementation/platform/unix/graphics/gui/display_private_data.hpp"
#include "implementation/platform/unix/graphics/gui/x11_libX11.hpp"



namespace ew
{
namespace graphics
{
namespace gui
{
namespace events
{

using namespace ew::core::types;
using namespace ew::core::threading;
using namespace ew::core::objects;

using ew::console::cerr;

ew::graphics::gui::events::event * pump_event(ew::graphics::gui::display * dpy)
{
	return dpy->pump_event();
}

bool destroyEvent(event * event)
{
	if (event)
		delete event;
	return true;
}

static const size_t __nr_events = 32;

class event_dispatcher::private_data
{
public:

	private_data()
		:
		event_list_mutex(0),
		have_event_cond(0),
		timeout(100),
		dpy(0)
	{
		event_list_mutex = new mutex(); // (Mutex::RECURSIVE_MUTEX);
		have_event_cond = new condition_variable(event_list_mutex);

		for (size_t count = 32; count > 0; --count) {
			pending_event_counters[ count - 1 ] = 0;
			pending_event_ticks_filter[ count - 1 ] = 0;
		}
	}

	~private_data()
	{
		delete have_event_cond;
		delete event_list_mutex;
	}

	mutex * event_list_mutex;
	std::list<event *> event_list;
	u32 pending_event_counters[ __nr_events ];
	u32 pending_event_ticks_filter[ __nr_events ];

	condition_variable  * have_event_cond;
	u32 timeout;
	display * dpy;
};

event_dispatcher::event_dispatcher(display * dpy)
	: d(new private_data())
{
	d->dpy = dpy;
}

event_dispatcher::~event_dispatcher()
{
	std::list<event *>::iterator end = d->event_list.end();
	std::list<event *>::iterator it = d->event_list.begin();

	for (; it != end;) {
		std::list<event *>::iterator cur = it;
		++it;

		event * ev = *cur;
		destroyEvent(ev);
		d->event_list.erase(cur);
	}

	delete d;
}

bool event_dispatcher::push(event ** event, u32 nrEvents)
{
	{
		mutex_locker lock(d->event_list_mutex);
		for (u32 i = 0; i < nrEvents; i++) {
			if (event[i]) {
				event[i]->pending = d->pending_event_counters[ event[i]->type ];
				d->pending_event_counters[ event[i]->type ]++;
				d->event_list.push_back(event[i]);
			}
		}
		d->have_event_cond->signal();
	}

	return true;
}

u32 event_dispatcher::get_nr_pending_events(event_type type) const
{
	u32 ret;

	if (d) {
		mutex_locker lock(d->event_list_mutex);
		ret = d->pending_event_counters[ type ];
	} else {
		ret = 0;
	}
	return ret;
}

bool event_dispatcher::drop_events_before_tick(event_type type , u32 tick)
{
	d->pending_event_ticks_filter[ type ] = tick;
	return true;
}

bool event_dispatcher::push(event * event)
{
	return push(&event, 1);
}

event * event_dispatcher::pop()
{
	mutex_locker lock(d->event_list_mutex);

	if (! d->event_list.size())
		return 0;

	event * ev = *(d->event_list.begin());
	d->event_list.pop_front();

	return ev;
}

u32 event_dispatcher::get_queue_size()
{
	mutex_locker lock(d->event_list_mutex);
	u32 sz = d->event_list.size();
	return sz;
}

bool  event_dispatcher::remove_widget_events(widget * widget)
{
	mutex_locker lock(d->event_list_mutex);

	std::list<event *>::iterator it = d->event_list.begin();
	while (it != d->event_list.end()) {
		std::list<event *>::iterator cur = it;
		++it;

		event * ev = *cur;
		if (ev->widget == widget) {
			destroyEvent(ev);
			d->event_list.erase(cur);
		}
	}

	return true;
}


bool event_dispatcher::stop()
{
	d->have_event_cond->signal();
	return true;
}

bool event_dispatcher::dispatch_all_events()
{
	{
		mutex_locker lock(d->event_list_mutex);
		d->have_event_cond->timed_wait(d->timeout);
	}

	u32 nr = get_queue_size();
	if (nr) {
		dispatch_events(nr);
		d->timeout = 0;
	} else {
		d->timeout = 1 * 1000;
	}

	return true;
}

bool event_dispatcher::dispatch_events(u32 nr)
{
	assert(nr != 0);

	u32 sz = get_queue_size();
	if (nr > sz)
		nr = sz;

//    std::cerr << __FUNCTION__ << " queue size = " << sz << "\n";

	while (nr--) {

		// if (!get_queue_size())
		//     break ;

		event * ev = 0;

		// filter events
		d->event_list_mutex->lock();
		{
			ev = d->event_list.front();
			assert(ev);
			d->pending_event_counters[ ev->type ]--;
			d->event_list.pop_front();
		}
		d->event_list_mutex->unlock();

#if 0
		std::cerr << " pending_event_counters[" << ev->type << "] == " << d->pending_event_counters[ ev->type ] << "\n";
		std::cerr << " pending_event_ticks_filter[ ev->type ] <= ev->time\n";
		std::cerr << d->pending_event_ticks_filter[ ev->type ] << " <= " << ev->time << " ?\n";
		std::cerr << " ev->time  == " << ev->time << "\n";
#endif


		if (ev->time >= d->pending_event_ticks_filter[ ev->type ]) {
			dispatch_event(ev);
		} else {
			// drop event
			assert(0);
		}

		destroyEvent(ev);
	}

//    std::cerr << __FUNCTION__ << " LEAVE : queue size = " << get_queue_size() << "\n";

	return true;
}

//Todo : split into
//ll_dispatch_event and dispatch_event(event, widget)
// provide default widget->on_event(event);
// with all ctx locked ...

bool event_dispatcher::dispatch_event(event * event)
{
	assert(event != nullptr);

	ew::graphics::gui::window * window = static_cast<ew::graphics::gui::window *>(event->widget);

	if (window == nullptr)
		return false;

	object_locker<ew::graphics::gui::display> guiLock(event->display);

	if (! event->display->d->have_widget(event->display->d, window)) {
		// cerr << "widget has disappeared" << "\n";
		return false;
	}

	object_locker<ew::graphics::gui::window> widget_lock(window);

	if (! window->isAvailable()) {
		// TODO: set unavailable widget flag
		return false;
	}

	object_locker<ew::graphics::rendering::rendering_context> lock_win_rdr_ctx(window->renderingContext());

	// handle low level widget
	switch (event->type) {
	case WidgetResizeEvent: {

		// ignore all resize events except the last one
		if (get_nr_pending_events(WidgetResizeEvent) != 0) {
			return true;
		}

		// handle last resize event
		widget_event * ev = static_cast<struct widget_event *>(event);

		window->d->_properties.width  = ev->width;
		window->d->_properties.height = ev->height;
		window->d->_properties.x  = window->x();
		window->d->_properties.y = window->y();

		// TODO: add helper for internal state updates
		// window->d->internal_on_resize();
		if (window->d->_offscreen_pixmap) {
			window->renderingContext()->unlock();
			window->d->_offscreen_pixmap->resize(ev->width, ev->height);
			window->renderingContext()->lock();
		}

		window->on_resize(ev);
#if 0
		// TODO: hints : if (redraw_after_resize) {
		struct widget_event evdraw = *ev;
		evdraw.type = WidgetDrawEvent;
		if (window->on_draw(&evdraw)) {
			window->swapBuffers();
		}
		// }
#endif
		return true;
	}
	break;

	case WidgetMotionEvent: {

		struct widget_event * ev = static_cast<struct widget_event *>(event);

		window->d->_properties.x = ev->x;
		window->d->_properties.y = ev->y;
		window->d->_properties.width = window->width();
		window->d->_properties.height =  window->height();
		window->on_move(ev);

		return true;
	}
	break;

	case WidgetCreateEvent: {

		struct widget_event * ev = static_cast<struct widget_event *>(event);

		window->d->_properties.x = ev->x;
		window->d->_properties.y = ev->y;
		window->d->_properties.width = window->width();
		window->d->_properties.height =  window->height();
		window->on_create(ev);

		return true;
	}
	break;

	case WidgetCloseEvent: {

		struct widget_event * ev = static_cast<struct widget_event *>(event);

		window->d->_properties.x = ev->x;
		window->d->_properties.y = ev->y;
		window->d->_properties.width = window->width();
		window->d->_properties.height =  window->height();
		window->on_close(ev);

		return true;
	}
	break;



	default: {
		/* do nothing */
	}
	break;

	}

	return window->on_event(event);
}

}
}
}
}
