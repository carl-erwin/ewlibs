#pragma once

#include <memory>

// Ew
#include <ew/ew_config.hpp>

// Ew Core
#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/thread.hpp>

//
#include <ew/unit/unit.hpp>
#include <ew/graphics/gui/gui.hpp>
#include <ew/graphics/gui/display/display.hpp>


//
#include <ew/graphics/gui/widget/widget_properties.hpp>

// font
#include <ew/graphics/font/font.hpp>

// renderer
#include <ew/graphics/renderer/renderer.hpp>
#include <ew/graphics/renderer/rendering_context.hpp>


// move this in a particulat header gui/event_forward.hpp
namespace ew
{
namespace graphics
{
namespace gui
{
namespace events
{

class  event_dispatcher;
struct widget_event;
struct pointer_event;
struct keyboard_event;
struct button_event;
}
}
}
}


namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::types;
using namespace ew::graphics::rendering;
using namespace ew::graphics::gui::events;

using namespace ew::units;

class widget;


/* TODO: move to core ??? */
class bindable_object
{
public:
	virtual      ~bindable_object() { }
	virtual bool bind()   = 0;
	virtual bool unbind() = 0;
};

namespace alignment
{

//public:
enum type {
	// horizontal
	left,
	right,

	// vertical
	top,
	bottom,

	// common
	centered,
	fixed,
	relative,
	undefined
};

//public:
class policy
{
public:
	alignment::type & type()
	{
		return m_policy;
	}
	u32 & ratio()
	{
		return m_percent;
	};
private:
	enum alignment::type m_policy = alignment::relative;
	u32 m_percent = 100;
};

}



// widget->horizontal_policy() = XXX;
// widget->veritical_policy() = XXX;
// widget->width_policy() = XXX;
// widget->height_policy() = XXX;

class resizeable_2d_object
{
public:
	virtual      ~resizeable_2d_object() { }
	virtual bool resize(u32 width, u32 height)  = 0;

	virtual bool set_width(u32 w)  = 0;
	virtual u32  width() const = 0;

	virtual bool set_height(u32 h) = 0;
	virtual u32  height() const = 0;
};

/* implement */
class texture : public bindable_object , resizeable_2d_object
{
public:
	virtual      ~texture() { }
};

/* TODO: move to proper header ew::graphics:gui:: offscreen_buffer.hpp */
class offscreen_buffer : public bindable_object, public resizeable_2d_object
{
public:
	virtual ~offscreen_buffer() { }

	virtual bool resize(u32 width, u32 height) = 0;
	texture * get_texture();


	/* TODO: remove this replace by ??? bind_texture()/unbind_texture() ?;
	   provide a
	   class texture {
	    bind() = 0;
	    unbind() = 0;
	   };
	*/
	virtual bool texture_id(u32 * id) = 0;
};


class positionable_2d_object
{
public:
	virtual ~positionable_2d_object() {}
	virtual s32  x()  = 0;
	virtual bool set_x(s32 x)  = 0;

	virtual s32  y()  = 0;
	virtual bool set_y(s32 y)  = 0;
};

class widget_event_callbacks
{
public:
	virtual ~widget_event_callbacks() { }

	// the default dispatcher, it will call the specific callbacks for U
	virtual bool on_event(event * ev) = 0;

	// events callbacks
	virtual bool on_close(const widget_event * ev)
	{
		return false;
	}
	virtual bool on_destroy(const widget_event * ev)
	{
		return false;
	}
	virtual bool on_map(const widget_event * ev)
	{
		return false;
	}
	virtual bool on_draw(const widget_event * ev)
	{
		return false;
	}
	virtual bool on_move(const widget_event * ev)
	{
		return false;
	}
	virtual bool on_resize(const widget_event * ev)
	{
		return false;
	}
	virtual bool on_key_press(const keyboard_event * ev)
	{
		return false;
	}
	virtual bool on_key_release(const keyboard_event * ev)
	{
		return false;
	}
	virtual bool on_mouse_button_press(const button_event * ev)
	{
		return false;
	}
	virtual bool on_mouse_button_release(const button_event * ev)
	{
		return false;
	}
	virtual bool on_mouse_wheel_up(const button_event * ev)
	{
		return false;
	}
	virtual bool on_mouse_wheel_down(const button_event * ev)
	{
		return false;
	}
	virtual bool on_pointer_motion(const pointer_event * ev)
	{
		return false;
	}
	virtual bool on_pointer_enter(const pointer_event * ev)
	{
		return false;
	}
	virtual bool on_pointer_leave(const pointer_event * ev)
	{
		return false;
	}
	virtual bool on_focus_in(const widget_event * ev)
	{
		return false;
	}
	virtual bool on_focus_out(const widget_event * ev)
	{
		return false;
	}
};


class layout
{
public:
	layout() { }
	virtual ~layout() { }

	typedef enum { left_to_right, right_to_left } direction;
	typedef enum { top, left, center, right, bottom } align;

	virtual bool add_widget(widget * parent, widget * w) = 0;
	virtual bool resize(widget * parent, u32 w, u32 h) = 0;

};

// left   margin  ( x() )
// right  margin  ( Pw - ( x() + w() ) )
// top    margin  ( y() )
// bottom margin  ( ph - y() - h() )

class EW_GRAPHICS_EXPORT no_layout : public layout
{
public:
	virtual ~no_layout();
	virtual bool add_widget(widget * parent, widget * w);
	virtual bool resize(widget * parent, u32 w, u32 h);
};

class EW_GRAPHICS_EXPORT horizontal_layout : public layout
{
public:
	horizontal_layout();
	virtual ~horizontal_layout();
	virtual bool add_widget(widget * parent, widget * w);
	virtual bool resize(widget * parent, u32 w, u32 h);
};


class EW_GRAPHICS_EXPORT vertical_layout : public layout
{
public:
	vertical_layout();
	virtual ~vertical_layout();
	virtual bool add_widget(widget * parent, widget * w);
	virtual bool resize(widget * parent, u32 w, u32 h);

};


class EW_GRAPHICS_EXPORT widget :
	public ew::core::object,
	public ew::core::objects::lock_interface,
	public widget_event_callbacks,
	public positionable_2d_object,
	public resizeable_2d_object
{
public:
	//
	widget();
	virtual ~widget();

	// object
	virtual const char * class_name() const
	{
		return "";
	}
	virtual const char * getName() const
	{
		return "";
	}

	// ILockableObject
	virtual bool lock()
	{
		return false;
	}
	virtual bool trylock()
	{
		return false;
	}
	virtual bool unlock()
	{
		return false;
	}

	virtual bool is_enabled();
	virtual void enable();
	virtual void disable();

	// env info
	// display
	virtual ew::graphics::gui::display * display() const
	{
		return nullptr;
	}

	// rendering
	virtual ew::graphics::rendering::rendering_context * renderingContext() const
	{
		return nullptr;
	}
	virtual ew::graphics::rendering::renderer      *     renderer() const
	{
		return nullptr;
	}

	// rendering context :: remove ???
	virtual bool trylockDrawingContext()
	{
		return false;
	}
	virtual bool lockDrawingContext()
	{
		return false;
	}
	virtual bool unlockDrawingContext()
	{
		return false;
	}

	virtual bool swapBuffers()
	{
		return false;
	}

	//
	virtual bool on_create(const widget_event * ev);

	virtual bool on_event(ew::graphics::gui::events::event * event);

	// position
	virtual s32  x();
	virtual s32  global_x();

	virtual bool set_x(s32 x);

	virtual s32  y();
	virtual s32  global_y();

	virtual bool set_y(s32 y);

	// size
	virtual bool set_width(u32 w);
	virtual u32  width() const;

	virtual bool set_height(u32 h);
	virtual u32  height() const;

	// actions
	virtual bool show()
	{
		return false;
	}
	virtual bool hide()
	{
		return false;
	}

	virtual bool move(s32 x, s32 y)
	{
		return false;
	}

	virtual bool resize(u32 width, u32 height);

	/* offscreen buffer management */
	virtual bool have_offscreen_buffer()
	{
		return false;
	}
	virtual offscreen_buffer * get_offscreen_buffer()
	{
		return nullptr;
	}

	virtual bool selected_at(s32 x, s32 y)
	{
		return false;
	}

	/**/
	virtual bool clear();
	virtual bool set_projection();
	virtual bool set_modelview();
	virtual bool render();

	virtual bool     add_widget(widget * w);

	virtual widget * get_child(u64 n) const
	{
		return dynamic_cast<widget *>(object::get_child(n));
	}

	//
	virtual widget * find_widget_under_coords(s32 x, s32 y);
	virtual bool     select_child(widget * w);
	virtual widget * get_selected_child();

	virtual bool     focus_on_child(widget * w);
	virtual widget * get_focused_child();

	// simple font
	virtual bool set_font(std::shared_ptr<ew::graphics::fonts::font> ft);
	virtual std::shared_ptr<ew::graphics::fonts::font> get_font() const;

	// layout
	virtual alignment::policy & horizontal_policy();
	virtual alignment::policy & vertical_policy();

	virtual bool set_layout(layout * l);
	virtual layout * get_layout();


private:
	class private_data;
	private_data * d = nullptr;
};


}
}
}
