#pragma once

#include <ew/core/types/types.hpp>
#include <ew/core/threading/thread.hpp>

#include <ew/graphics/gui/display/display.hpp>
#include <ew/graphics/gui/widget/widget.hpp>

#include <ew/graphics/gui/event/keys.hpp>

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

typedef   enum event_type {
	Unknow = 0,
	KeyPressEvent       ,
	KeyReleaseEvent     ,
	ButtonPressEvent    ,
	ButtonReleaseEvent  ,
	WidgetCloseEvent    ,
	WidgetResizeEvent   ,
	WidgetMotionEvent   ,
	WidgetFocusInEvent  ,
	WidgetFocusOutEvent ,
	WidgetDrawEvent     ,
	WidgetEnterEvent    ,
	WidgetLeaveEvent    ,
	WidgetCreateEvent   ,
	WidgetMapEvent      ,
	WidgetDestroyEvent  ,
	PointerMotionEvent  ,
	PointerEnterEvent   ,
	PointerLeaveEvent   ,
	//    WidgetUnmapEvent,
	//    WidgetVisibilityChangeEvent, ???
	//    Destroywidget_event, //
	//    KeymapNotify, //
	//    VisibilityNotify, //
	//    CreateNotify, //
	//    Destroywidget_event, //
	//    UnmapNotify, //
	//    MapNotify, //
	//    MapRequest, //
	//    ReparentNotify, //
	//    ConfigureNotify, //
	//    ConfigureRequest, //
	//    GravityNotify, //
	//    ResizeRequest, //
	//    CirculateNotify, //
	//    CirculateRequest, //
	//    PropertyNotify, //
	//    SelectionClear, //
	//    SelectionRequest, //
	//    SelectionNotify, //
	//    ColormapNotify, //
	//    ClientMessage, //
	//   MappingNotify, //
} event_type;


inline const char * event_type_to_c_string(event_type type)
{
	switch (type) {
	case Unknow:
		return "Unknow";
	case KeyPressEvent:
		return "KeyPressEvent";
	case KeyReleaseEvent:
		return "KeyReleaseEvent";
	case ButtonPressEvent:
		return "ButtonPressEvent";
	case ButtonReleaseEvent:
		return "ButtonReleaseEvent";
	case WidgetCloseEvent:
		return "WidgetCloseEvent";
	case WidgetResizeEvent:
		return "WidgetResizeEvent";
	case WidgetMotionEvent:
		return "WidgetMotionEvent";
	case WidgetFocusInEvent:
		return "WidgetFocusInEvent";
	case WidgetFocusOutEvent:
		return "WidgetFocusOutEvent";
	case WidgetDrawEvent:
		return "WidgetDrawEvent";
	case WidgetEnterEvent:
		return "WidgetEnterEvent";
	case WidgetLeaveEvent:
		return "WidgetLeaveEvent";
	case WidgetCreateEvent:
		return "WidgetCreateEvent";
	case WidgetMapEvent:
		return "WidgetMapEvent";
	case WidgetDestroyEvent:
		return "WidgetDestroyEvent";
	case PointerMotionEvent:
		return "PointerMotionEvent";
	case PointerEnterEvent:
		return "PointerEnterEvent";
	case PointerLeaveEvent:
		return "PointerLeaveEvent";
	default: {
		return "Unhandled";
	}
	break;
	}

	return "Unhandled";
}


struct event {
	event_type type; // we can cast by reading this
	ew::graphics::gui::widget  *  widget;
	ew::graphics::gui::display  *  display;
	u32        time;
	u32        pending;


	s32  x; // pointer coords
	s32  y; // pointer coords
	// u32  dev_idx; // pointer deveice index
};

// ------------------------------

struct keyboard_event : public event {

	keys::key_value  key;
	u32  unicode;

	bool ctrl: 1;
	bool altL: 1;
	bool altR: 1;
	bool shift: 1;
	bool capslock: 1;

	bool button1: 1;
	bool button2: 1;
	bool button3: 1;
	bool button4: 1;
	bool button5: 1;
};


struct button_event : public event {
	u32        button;
};

struct pointer_event : public event {
};

// ----------------------------------
struct widget_event_ctx {
};

struct widget_event : public event {
	u32 width;
	u32 height;
	widget_event_ctx * ctx = nullptr;
};

//==
// a user event dispatcher for a given widget ?
// or make this a private class

class EW_GRAPHICS_EXPORT event_dispatcher
{
public:
	event_dispatcher(ew::graphics::gui::display * dpy);
	virtual ~event_dispatcher();

	bool    push(ew::graphics::gui::events::event * event);
	bool    push(ew::graphics::gui::events::event ** event, u32 nrEvents);

	ew::graphics::gui::events::event * pop();

	u32  get_queue_size();
	bool dispatch_all_events();
	bool dispatch_events(u32 nr);
	bool dispatch_event(ew::graphics::gui::events::event * event);
	bool remove_widget_events(ew::graphics::gui::widget * widget);
	u32  get_nr_pending_events(event_type type) const;
	bool drop_events_before_tick(event_type type , u32 tick);

	// only for thread
	// bool start();
	bool stop();

private:
	class private_data;
	class private_data * d;
};

EW_GRAPHICS_EXPORT  ew::graphics::gui::events::event * pump_event(ew::graphics::gui::display * dpy);
EW_GRAPHICS_EXPORT  bool destroyEvent(ew::graphics::gui::events::event * event);

}
}
}
}

