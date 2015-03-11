#pragma once

// Ew
#include <ew/ew_config.hpp>

// Ew Core
#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/thread.hpp>

#include <ew/graphics/gui/gui.hpp>

// base of gui objects
#include <ew/graphics/gui/widget/widget.hpp>
#include <ew/graphics/gui/widget/widget_properties.hpp>

#include <ew/graphics/gui/widget/window/window_properties.hpp>

// pixmap
#include <ew/graphics/gui/widget/pixmap/pixmap.hpp>

// renderer
#include <ew/graphics/renderer/renderer.hpp>
#include <ew/graphics/renderer/rendering_context.hpp>

namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::types;
using namespace ew::graphics::rendering;

/**
\class Window
\brief system native window
\todo  add setBackgroundColor(Color4 & color);<br>
      add setClearColor(Color4 & color);
*/
class EW_GRAPHICS_EXPORT window : public ew::graphics::gui::widget
{
public:
	window(ew::graphics::gui::display * dpy, ew::graphics::gui::window * parent, ew::graphics::gui::window_properties & properties);
	virtual ~window();

	//ew::core::object
	virtual const char * class_name() const;
	virtual const char * getName() const;

	// ILockableObject
	virtual bool lock();
	virtual bool trylock();
	virtual bool unlock();

	// env info
	// display
	virtual ew::graphics::gui::display * display() const;

	// rendering
	virtual ew::graphics::rendering::rendering_context * renderingContext() const;
	virtual ew::graphics::rendering::renderer      *     renderer() const ;

	// renderring context
	virtual bool trylockDrawingContext();
	virtual bool lockDrawingContext();
	virtual bool unlockDrawingContext();
	virtual bool swapBuffers();

	// position
	virtual s32 xPos();
	virtual s32 yPos();
	virtual bool setXPos(s32 x);
	virtual bool setYPos(s32 y);

	// size
	virtual u32  width() const;
	virtual u32  height() const;
	virtual bool setWidth(u32 w);
	virtual bool setHeight(u32 h);

	virtual bool isAvailable(); // ?

	// actions
	virtual bool show();
	virtual bool hide();

	virtual bool move(s32 x, s32 y);
	virtual bool resize(u32 width, u32 height);

	// events callbacks
	virtual bool on_create(const widget_event * ev);
	virtual bool on_close(const widget_event * ev);
	virtual bool on_destroy(const widget_event * ev);

	virtual bool on_draw(const widget_event * ev);

	virtual bool on_move(const widget_event * ev);
	virtual bool on_resize(const widget_event * ev);

	virtual bool on_key_press(const keyboard_event * ev);
	virtual bool on_key_release(const keyboard_event * ev);

	virtual bool on_mouse_button_press(const button_event * ev);
	virtual bool on_mouse_button_release(const button_event * ev);

	virtual bool on_pointer_motion(const pointer_event * ev);
	virtual bool on_pointer_enter(const pointer_event * ev);
	virtual bool on_pointer_leave(const pointer_event * ev);

	virtual bool on_focus_in(const widget_event * ev);
	virtual bool on_focus_out(const widget_event * ev);

protected:
	class window_private_data;
	class window_private_data * d;

	friend class ew::graphics::gui::gui;
	friend class ew::graphics::gui::display;
	friend class ew::graphics::gui::pixmap;
	friend class ew::graphics::rendering::rendering_context;
	friend class ew::graphics::gui::events::event_dispatcher;
};

}
}
}
