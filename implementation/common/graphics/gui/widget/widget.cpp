#include <assert.h>
#include <memory>
#include <vector>

// Ew
#include <ew/ew_config.hpp>

// Ew Core
#include <ew/core/types/types.hpp>
#include <ew/core/time/time.hpp>

#include <ew/core/object/object.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/thread.hpp>
#include <ew/graphics/gui/gui.hpp>
#include <ew/graphics/gui/display/display.hpp>

// gui events
#include <ew/graphics/gui/event/event.hpp>
#include <ew/graphics/gui/event/push.hpp>


// base of gui objects
#include <ew/graphics/gui/widget/widget.hpp>
#include <ew/graphics/gui/widget/widget_properties.hpp>

// font
#include <ew/graphics/font/font.hpp>

// move to gl_widget.cpp
// renderer
#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"

// TODO: common code to call getProgAddr
#include "../implementation/platform/unix/graphics/gui/x11_libGLX.hpp"

using namespace ew::core;
using namespace ew::core::types;
using namespace ew::graphics::gui;
using namespace ew::graphics::gui::events;
using namespace ew::implementation::graphics::rendering::opengl;



namespace ew
{
namespace graphics
{
namespace gui
{

using namespace ew::core::types;
using namespace ew::graphics::rendering;
using namespace ew::graphics::gui::events;

///////////////////////////////////////////////////////////////////////////////////////////////////

//
class widget;

struct event_context {
	widget * selected_child;
	widget * focused_child;

	widget * wid;
	widget_event wid_ev;

	bool need_redraw = false;
};

class widget::private_data
{
public:
	~private_data()
	{
	}
public:
	bool m_enable = true;
	s32 _x;
	s32 _y;
	u32 _w;
	u32 _h;

	//
	layout * m_layout = nullptr;

	alignment::policy m_horizontal_policy;
	alignment::policy m_vertical_policy;

	//
	widget * last_selected_child = nullptr;
	widget * last_focused_child  = nullptr;

	//
	std::shared_ptr<ew::graphics::fonts::font> m_ft;

//helpers
	bool on_event(ew::graphics::gui::widget * self, ew::graphics::gui::events::event * event);
	bool get_focused_widget(ew::graphics::gui::widget * self, ew::graphics::gui::events::event * event, event_context & event_ctx);
	bool dispatch_event(ew::graphics::gui::widget * self, ew::graphics::gui::events::event * event, event_context & event_ctx);
	bool widget_contains(widget * wid, s32 x, s32 y) const;
	bool update_selected_child(ew::graphics::gui::widget * self, ew::graphics::gui::events::event * event, event_context & event_ctx);
	bool check_redraw(ew::graphics::gui::widget * self, const event_context & event_ctx);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

widget::widget()
{
	d = new widget::private_data();
}

widget::~widget()
{
	assert(d);
	delete d;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::add_widget(widget * w)
{
	bool ret;

	widget * parent = this;

	// std::cerr << __PRETTY_FUNCTION__ << "\n";

	if (d->m_layout == nullptr) {
		ew::core::object * p = static_cast<object *>(parent);

#ifndef NDEBUG
		auto n_prev = p->number_of_children();
#endif

		ret = p->add_child(static_cast<object *>(w));

#ifndef NDEBUG
		auto n_next = p->number_of_children();
#endif
		assert(n_prev < n_next);
	} else {
		ret =  d->m_layout->add_widget(parent, w);
	}
	assert(ret == true);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

s32 widget::x()
{
	return d->_x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

s32 widget::global_x()
{
	s32 x = d->_x;
	widget * p = dynamic_cast<widget *>(this->get_parent());
	while (p) {
		x += p->x();
		p = dynamic_cast<widget *>(p->get_parent());
	}
	return x;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::set_x(s32 x)
{
	// TODO: check policy
	d->_x = x;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

s32 widget::y()
{
	return d->_y;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

s32 widget::global_y()
{
	s32 y = d->_y;
	widget * p = dynamic_cast<widget *>(this->get_parent());
	while (p) {
		y += p->y();
		p = dynamic_cast<widget *>(p->get_parent());
	}
	return y;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::set_y(s32 y)
{
	// TODO: check policy
	d->_y = y;
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

u32  widget::width() const
{
	return d->_w;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::set_width(u32 w)
{
	// TODO: check policy
	d->_w = w;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

u32  widget::height() const
{
	return d->_h;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::set_height(u32 h)
{
	// TODO: check policy
	d->_h = h;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

alignment::policy & widget::horizontal_policy()
{
	return d->m_horizontal_policy;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

alignment::policy & widget::vertical_policy()
{
	return d->m_vertical_policy;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::set_layout(layout * l)
{
	d->m_layout = l;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

layout * widget::get_layout()
{
	return d->m_layout;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::resize(u32 width, u32 height)
{
	layout * l = get_layout();
	if (l) {
		return l->resize(this, width, height);
	} else {
		// nothing
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::is_enabled()
{
	return d->m_enable;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void widget::enable()
{
	d->m_enable = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void widget::disable()
{
	d->m_enable = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

widget * widget::find_widget_under_coords(s32 x, s32 y)
{
	// find
	u32 n = number_of_children();

	for (u32 i = 0; i < n; ++i) {
		widget * wid =  get_child(i);

		if ((y >= wid->y()) && (y <= (wid->y() + (s32)wid->height()))) {
			if ((x >= wid->x()) && (x <= (wid->x() + (s32)wid->width()))) {

				if (wid->is_enabled() == false) {
					continue;
				}


				if (wid->number_of_children() == 0) {
					//// std::cerr << __PRETTY_FUNCTION__ << " : wid = " << wid << "\n";
					return wid;
				}

				x -= wid->x();
				y -= wid->y();
				return wid->find_widget_under_coords(x, y);
			}
		}
	}

	return this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

widget * widget::get_selected_child()
{
// // std::cerr << __PRETTY_FUNCTION__ << " w = " << d->last_selected_child << "\n";

	return d->last_selected_child;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::select_child(widget * w)
{
	d->last_selected_child = w;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::focus_on_child(widget * w)
{
	d->last_focused_child = w;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

widget * widget::get_focused_child()
{
	return d->last_focused_child;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::clear()
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::set_projection()
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::set_modelview()
{
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

// TREE WALK
bool widget::on_create(const widget_event * ev)
{
	u32 n = object::number_of_children();

//   // std::cerr << __FUNCTION__ << " : number_of_children = " << n << "\n";

	for (u32 i = 0; i < n; ++i) {
		widget * w = get_child(i);
		assert(w != nullptr); // dynamic cast
		if (w == nullptr)
			continue;

		w->on_create(ev);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::render()
{
	// only if no parent ?
	std::cerr << "[" << ew::core::time::get_ticks() << "]" << __PRETTY_FUNCTION__ << "widget->name() " << this->name() << "\n";

	this->clear();

	//
	this->set_projection();
	this->set_modelview();

	u32 n = number_of_children();

	std::cerr << "number_of_children = " << n << "\n";

	for (u32 i = 0; i < n; ++i) {

		widget * w = dynamic_cast<widget *>(get_child(i));
		if (w == nullptr)
			continue;

		if (w->is_enabled() == false)
			continue;

		// slow ....
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		{
			glTranslatef(+w->x(), +w->y(), 0.0f);
			//           // std::cerr << "render child " << i << "...\n";
			w->render();
			glTranslatef(-w->x(), -w->y(), 0.0f);
		}
		glPopAttrib();

	}

	return true;

	// if !offscreen buffers
	// set_bg
	// this->set_projection();
	// this->set_modelview();

	// if offscreen buffers
	// render children
	for (u32 i = 0; i < n; ++i) {
		widget * w = dynamic_cast<widget *>(get_child(i));
		if (w == nullptr)
			continue;

		w->render();
	}

	this->set_projection();
	this->set_modelview();

	bool use_opengl_coord_system = !true;

	// render children textures
	glEnable(GL_TEXTURE_2D);
	for (u32 i = 0; i < n; ++i) {
		widget * w = dynamic_cast<widget *>(get_child(i));
		if (w == nullptr)
			continue;

		offscreen_buffer * offscreen = w->get_offscreen_buffer();
		u32 texid;

		// w->get_offscreen_texture_id(&texid);
		offscreen->texture_id(&texid);
		{
			ew_glBindTexture(GL_TEXTURE_2D, texid);
			// glNormal3f(0, 0, 1);
			// glColor4f(.0f, 1.0f, 1.0f, .0f);

			s32 x = w->x();
			s32 y = w->y();
			s32 max_width  = x + w->width();
			s32 max_height = y + w->height();

			if (use_opengl_coord_system) {
				// OpenGL coords
				glBegin(GL_QUADS);
				{
					static float T = 1.0f;

					glTexCoord2f(T, T);
					glVertex2i(max_width , max_height);

					glTexCoord2f(0, T);
					glVertex2i(x         , max_height);

					glTexCoord2f(0, 0);
					glVertex2i(x     , y);

					glTexCoord2f(T, 0);
					glVertex2i(max_width, y);
				}
				glEnd();

			} else {

				// windowing system coords
				glBegin(GL_QUADS);
				{
					static float T = 1.0f;

					glTexCoord2f(0, T);
					glVertex2i(x, y);

					glTexCoord2f(0, 0);
					glVertex2i(x, max_height);

					glTexCoord2f(T, 0);
					glVertex2i(max_width, max_height);

					glTexCoord2f(T, T);
					glVertex2i(max_width, y);
				}
				glEnd();
			}
		}
	}
	glDisable(GL_TEXTURE_2D);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::private_data::get_focused_widget(ew::graphics::gui::widget * self, ew::graphics::gui::events::event * event, event_context & event_ctx)
{
	static int debug = 0;

	widget * selected_child = self->get_selected_child();
	widget * focused_child  = self->get_focused_child();

	if (event->type == WidgetDrawEvent)
		return true;

	if (debug) {
		std::cerr << " -----------------------------------------------------\n";
		std::cerr << __FUNCTION__ << " : " << ew::core::time::get_ticks() << " : main widget " << self << "\n";
		std::cerr << __FUNCTION__ << " : event->type = " << event_type_to_c_string(event->type) << "\n";
		std::cerr << __FUNCTION__ << " : PASS1\n";
		std::cerr << __FUNCTION__ << " : selected_child " << selected_child << "\n";
		std::cerr << __FUNCTION__ << " : focused_child  " << focused_child << "\n";
	}

	ew::graphics::gui::events::widget_event & wid_ev = event_ctx.wid_ev;

	// the last selected widget has higher priority
	ew::graphics::gui::widget * wid             = selected_child;
	ew::graphics::gui::widget * wid_under_coord = nullptr;

	// translate coords

	if (debug) {
		std::cerr << " global event->x == " << event->x << "\n";
		std::cerr << " global event->y == " << event->y << "\n";
	}

	wid_ev.x      = event->x;
	wid_ev.y      = event->y;
	//wid_ev.width  = TODO
	//wid_ev.height = TODO

	if (selected_child == nullptr) {
		// no child previously selected...
		wid_under_coord = self->find_widget_under_coords(event->x, event->y /* &new_child_event */);
		wid = wid_under_coord;

		if (debug) {
			std::cerr << " looking for widget at (" << wid_ev.x << ", " << wid_ev.y << ")\n";
			std::cerr << __FUNCTION__ << " : no child previously selected...\n";
			std::cerr << __FUNCTION__ << " : find widget by coords\n";
			std::cerr << __FUNCTION__ << " : wid_under_coord  " << wid_under_coord << "\n";
		}
	}

	// to local coordinates

	// translate global coords to local coordinates
	// get_event_local_coord(wid, event, wid_ev);
	wid_ev.x      = event->x - wid->global_x();
	wid_ev.y      = event->y - wid->global_y();

	if (debug) {
		std::cerr << " local event->x == " << wid_ev.x << "\n";
		std::cerr << " local event->y == " << wid_ev.y << "\n";
	}

	// select the final widget
	event_ctx.wid = wid;

	// create update_focus() fn
	if (focused_child) {
//       update_focused_child(wid_ev, event_ctx);
		if (focused_child != wid) {
			/* focus out on previous widget */
			self->focus_on_child(nullptr);

			bool ret = focused_child->on_focus_out(&wid_ev);
			if (ret == true) {
			}

			/* focus in on current widget */
			self->focus_on_child(wid);
			ret = wid->on_focus_in(&wid_ev);
			if (ret == true) {
			}

		} else {
			/* current widget already focused */
		}

	} else {
		/* focus in on current widget */
		self->focus_on_child(wid);
		bool ret = wid->on_focus_in(&wid_ev);
		if (ret == true) {
		}
	}

	event_ctx.focused_child  = self->get_focused_child();
	event_ctx.selected_child = self->get_selected_child();

	if (event_ctx.focused_child) {
		// std::cerr << __FUNCTION__ << " : focused_child  = " << event_ctx.focused_child->name() << "\n";
	}

	if (event_ctx.selected_child) {
		// std::cerr << __FUNCTION__ << " : selected_child = " << event_ctx.selected_child->name() << "\n";
	}

	return true;
}


bool widget::private_data::widget_contains(widget * wid, s32 x, s32 y) const
{
	if (x < wid->x()) {
		return false;
	}

	if (x >= (s32)(wid->x() + wid->width())) {
		return false;
	}

	if (y < wid->y()) {
		return false;
	}

	if (y >= (s32)(wid->y() + wid->height())) {
		return false;
	}

	return true;
}


bool widget::private_data::update_selected_child(ew::graphics::gui::widget * self, ew::graphics::gui::events::event * event, event_context & event_ctx)
{
	if (event->type == WidgetDrawEvent)
		return true;

	auto selected_child  = self->get_selected_child();

	if (selected_child == nullptr) {
		return true;
	}

	widget * wid = event_ctx.wid;
	widget_event & wid_ev = event_ctx.wid_ev;

	if (selected_child != wid) {

		bool contains = widget_contains(wid, wid_ev.x, wid_ev.y);
		if (contains == true) {
			return true;
		}

		bool ret = selected_child->on_focus_out(&wid_ev);
		if (ret == true) {
			event_ctx.need_redraw = true;
		}
//                  self->focus_on_child(nullptr); // ????
	}

	return true;
}

/* TODO:
  this is a big hack
  split function

  0) maintain pointer coords (cache)
  1) check type of event
  2) get focused widget
  3) create new translated event (focused widget coord system)

  update focused widget in relevant event type handlers : pointer motion , enter/leave, focus in/out events (gui system)

*/

bool widget::private_data::dispatch_event(ew::graphics::gui::widget * self, ew::graphics::gui::events::event * event, event_context & event_ctx)
{
	bool need_redraw        = false;
	bool force_redraw        = false;
	widget * wid            = event_ctx.wid;
	widget * focused_child  = event_ctx.focused_child;
	widget * selected_child = event_ctx.selected_child;
	widget_event & wid_ev   = event_ctx.wid_ev;

//    std::cerr << __FUNCTION__ << " : event->type = " << event_type_to_c_string(event->type) << "\n";

	switch (event->type) {

	case KeyPressEvent : {
		keyboard_event keyb_ev = *static_cast<keyboard_event *>(event);
		keyb_ev.x = wid_ev.x;
		keyb_ev.y = wid_ev.y;
		bool ret = wid->on_key_press(&keyb_ev);
		if (ret == true) {
			//force_redraw = true;
		}
	}
	break ;

	case KeyReleaseEvent: {
		keyboard_event keyb_ev = *static_cast<keyboard_event *>(event);
		keyb_ev.x = wid_ev.x;
		keyb_ev.y = wid_ev.y;
		bool ret = wid->on_key_release(&keyb_ev);
		if (ret == true) {
			//force_redraw = true;
		}
	}
	break ;

	case ButtonPressEvent: {
		button_event button_ev = *static_cast<button_event *>(event);
		button_ev.x = wid_ev.x;
		button_ev.y = wid_ev.y;

		switch (button_ev.button) {
		case 4: {
			bool ret = wid->on_mouse_wheel_up(&button_ev);
			if (ret == true) {
				force_redraw = true;
			}
		}
		break;

		case 5: {
			bool ret = wid->on_mouse_wheel_down(&button_ev);
			if (ret == true) {
				force_redraw = true;
			}
		}
		break;

		default: {
			bool ret = wid->on_mouse_button_press(&button_ev);
			if (ret == true) {
				force_redraw = true;
			}

			self->select_child(wid);
		}
		break;
		}
	}
	break ;

	case ButtonReleaseEvent : {
		button_event button_ev = *static_cast<button_event *>(event);
		button_ev.x = wid_ev.x;
		button_ev.y = wid_ev.y;

		switch (button_ev.button) {
		case 4:
			break;

		case 5:
			break;

		default:
			bool ret = wid->on_mouse_button_release(&button_ev);
			if (ret == true) {
				force_redraw = true;
			}

			self->select_child(nullptr);
			break;
		}
	}
	break ;

	// TODO: do not lock widget HERE
	case WidgetCloseEvent: {
		widget_event * ev = static_cast<struct widget_event *>(event);
		bool ret = wid->on_close(ev);
		if (ret == true) {
			// the widget is ok with closing
			// set flags
		}

	} break;

	case WidgetResizeEvent: {
		widget_event * ev = static_cast<struct widget_event *>(event);
		wid->on_resize(ev);

		// user hint : paint_after_resize ?
		struct widget_event evdraw = *ev;
		evdraw.type = WidgetDrawEvent;
		force_redraw = true;
	}
	break;

	case WidgetMotionEvent: {
		wid->set_x(wid_ev.x);
		wid->set_y(wid_ev.y);
		wid->on_move(&wid_ev);
	}
	break;

	case PointerMotionEvent: {
		pointer_event pointer_ev = *static_cast<pointer_event *>(event);
		pointer_ev.x = wid_ev.x;
		pointer_ev.y = wid_ev.y;
		bool ret = wid->on_pointer_motion(&pointer_ev);
		if (ret == true) {
			force_redraw = true;
		}
		//
	}
	break;

	case PointerEnterEvent: {

		std::cerr << " PointerEnterEvent\n";

		assert(focused_child);

		pointer_event pointer_ev = *static_cast<pointer_event *>(event);
		pointer_ev.x = wid_ev.x;
		pointer_ev.y = wid_ev.y;

		bool ret = wid->on_pointer_enter(&pointer_ev);
		if (ret == true) {
			force_redraw = true;
		}
		//

	}
	break ;

	case PointerLeaveEvent: {

		std::cerr << " final ... wid(" << wid->name() << ") -> PointerEnterEvent\n";

		pointer_event pointer_ev = *static_cast<pointer_event *>(event);
		pointer_ev.x = wid_ev.x; // TODO:
		pointer_ev.y = wid_ev.y; // TODO:

		bool ret = wid->on_pointer_leave(&pointer_ev);
		if (ret == true) {
			force_redraw = true;
		}
		//

		if (!selected_child) {
			if (focused_child) {
				std::cerr << " focus_out... wid(" << focused_child->name() << ")\n";
				ret = focused_child->on_focus_out(&wid_ev);
			}

			self->focus_on_child(nullptr);
		}
	}
	break ;

	case WidgetFocusInEvent: {
		bool ret = wid->on_focus_in(&wid_ev);
		if (ret == true) {
			force_redraw = true;
		}
		//
	}
	break ;

	case WidgetFocusOutEvent: {
		bool ret = wid->on_focus_out(&wid_ev);
		if (ret == true) {
			force_redraw = true;
		}
		//

		ret = self->focus_on_child(nullptr);
		if (ret == true) {
			force_redraw = true;
		}
		//

	}
	break ;

	case WidgetCreateEvent: {
		bool ret = wid->on_create(&wid_ev);
		if (ret == true) {
			force_redraw = true;
		}
		//

	}
	break ;

	case WidgetDrawEvent: {
		need_redraw = true;
	}
	break ;

	case WidgetMapEvent: {
		bool ret = wid->on_map(&wid_ev);
		if (ret == true) {
			need_redraw = true;
		}
	}
	break;

	default: {
		// std::cerr << " handled EVENT type " << event->type << ", not handled\n";
		assert(0);
	}
	break;
	}

	//
	if (force_redraw)
		need_redraw = true;

	if (need_redraw == true) {
		bool ret = wid->on_draw(nullptr);
		if (ret == true) {
			wid->swapBuffers();
		}
	}


	return true;
}

bool widget::private_data::check_redraw(ew::graphics::gui::widget * self, const event_context & event_ctx)
{
	if (event_ctx.need_redraw == true) {
		ew::graphics::gui::push_draw_event(self);
	}

	return true;
}

bool widget::private_data::on_event(ew::graphics::gui::widget * self, ew::graphics::gui::events::event * event)
{
	struct event_context event_ctx;
	event_ctx.wid            = self;
	event_ctx.selected_child = self->get_selected_child();
	event_ctx.focused_child  = self->get_focused_child();

	get_focused_widget(self, event, event_ctx);
	dispatch_event(self, event, event_ctx);
	update_selected_child(self, event, event_ctx);
	check_redraw(self, event_ctx);

	return true;
}

bool widget::on_event(ew::graphics::gui::events::event * event)
{
	return this->d->on_event(this, event);
}


///////////////////////////////////////////////////////////////////////////////////////////////////

bool widget::set_font(std::shared_ptr<ew::graphics::fonts::font> ft)
{
	d->m_ft = ft;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ew::graphics::fonts::font> widget::get_font() const
{
	return d->m_ft;
}

///////////////////////////////////////////////////////////////////

horizontal_layout::horizontal_layout()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////

horizontal_layout::~horizontal_layout()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool horizontal_layout::add_widget(widget * parent, widget * w)
{
	object * p = static_cast<object *>(parent);
	return p->add_child(static_cast<object *>(w));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

static u32 get_len_from_policy(alignment::policy & policy, u32 current_offset, u32 current_len, u32 parent_len, u32 pixel_per_relative)
{
	u32 len = current_len;

	switch (policy.type()) {
	case alignment::fixed: {
		// std::cerr << " alignment::fixed:\n";
	}
	break;

	case alignment::relative: {
		// std::cerr << " alignment::relative:\n";

		u32 ratio = policy.ratio();
		len = (parent_len * ratio) / 100;

		// std::cerr << " ratio = " << ratio << "\n";
		// std::cerr << " parent_len = " << parent_len << "\n";
		// std::cerr << " pixel_per_relative = " << pixel_per_relative << "\n";
		// std::cerr << " LEN = " << len << "\n";

		if (len > pixel_per_relative) {
			len = pixel_per_relative;
		}
	}
	break;

	default: {
		// TODO: add offset to set_x
		assert(0);
	}
	break;
	}

	return len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool horizontal_layout::resize(widget * parent, u32 w, u32 h)
{
#if 0
	std::cerr << "***************************************************************************************\n";
	std::cerr << __PRETTY_FUNCTION__ << " *** DO RESIZE  W = " << w << " H = " << h << "\n";
#endif

	if (!w || !h)
		return true;

#if 1
	parent->set_width(w);
	parent->set_height(h);
	parent->clear();
	parent->set_projection();
	parent->set_modelview();
#endif

	// LEFT TO RIGHT
	u32 nr_fixed = 0;
	u32 nr_relative = 0;

	u32 remain_width_with_no_fixed = parent->width();

	// TODO: function helper HERE !!
	// count relative/fixed/ etc ... counter_array[policy.type()]++;
	for (u32 count = 0; count < parent->number_of_children(); count++) {
		widget * c = parent->get_child(count);

		switch (c->horizontal_policy().type()) {
		case alignment::fixed: {
			remain_width_with_no_fixed -= c->width();
			nr_fixed++;
		}
		break;

		case alignment::relative: {
			nr_relative++;
		}
		break;

		default: {

		}
		break;
		}

		c->set_y(0);
		u32 len = get_len_from_policy(c->vertical_policy(), 0, c->height(), parent->height(), parent->height());
		assert(len);
		c->set_height(len);
	}

	// std::cerr << " nr_fixed = " << nr_fixed << "\n";
	// std::cerr << " remain_width_with_no_fixed = " << remain_width_with_no_fixed << "\n";

	// std::cerr << " nr_relative = " << nr_relative << "\n";
	u32 pixel_per_relative = 0;
	if (nr_relative) {
		pixel_per_relative = remain_width_with_no_fixed / nr_relative;
	}


	// std::cerr << " pixel_per_relative = " << pixel_per_relative << "\n";

	if (pixel_per_relative > parent->width()) {
		return true;
	}

	s32 x = 0;
	u32 remain_for_relative_width = remain_width_with_no_fixed;

	//    auto nr_children = parent->number_of_children();
	// std::cerr << " nr_children = " << nr_children << "\n";
	for (u32 count = 0; count < parent->number_of_children(); count++) {

		widget * c = parent->get_child(count);

		// std::cerr << " child(" << count << ")->name() = " << c->name() << "\n";

		s32 child_x  = x;
		s32 x_offset = 0;

		// std::cerr << "remain_for_relative_width = " << remain_for_relative_width << "\n";
		u32 len = get_len_from_policy(c->horizontal_policy(), x, c->width(), remain_for_relative_width, pixel_per_relative);
		// std::cerr << " set child(" << count << ") width  = " << len << "\n";

		c->set_width(len);
		c->set_x(child_x + x_offset);
		x += c->width();
		if (c->horizontal_policy().type() == alignment::relative) {
			remain_for_relative_width -= c->width();
		}

		c->resize(c->width(), c->height());
	}


	// std::cerr << "***************************************************************************************\n";

	return true;
}

///////////////////////////////////////////////////////////////////

vertical_layout::vertical_layout()
{

}

vertical_layout::~vertical_layout()
{

}

bool vertical_layout::add_widget(widget * parent, widget * w)
{
	object * p = static_cast<object *>(parent);
	return p->add_child(static_cast<object *>(w));
}


bool vertical_layout::resize(widget * parent, u32 w, u32 h)
{
	if (!w || !h)
		return true;

#if 1
	parent->set_width(w);
	parent->set_height(h);
	parent->clear();
	parent->set_projection();
	parent->set_modelview();
#endif

	// LEFT TO RIGHT
	u32 nr_fixed = 0;
	u32 nr_relative = 0;

	u32 remain_height_with_no_fixed = parent->height();

	for (u32 count = 0; count < parent->number_of_children(); count++) {

		widget * c = parent->get_child(count);

		switch (c->vertical_policy().type()) {
		case alignment::fixed: {
			remain_height_with_no_fixed -= c->height();
			nr_fixed++;
		}
		break;

		case alignment::relative: {
			nr_relative++;
		}
		break;

		default: {

		}
		break;
		}

		c->set_x(0);

		u32 len = get_len_from_policy(c->horizontal_policy(), 0, c->width(), parent->width(), parent->width());
		assert(len);
		c->set_width(len);
	}

	u32 pixel_per_relative = 0;
	if (nr_relative) {
		pixel_per_relative = remain_height_with_no_fixed / nr_relative;
	}

	if (pixel_per_relative > parent->height()) {
		return true;
	}

	s32 y = 0;
	u32 remain_for_relative_height = remain_height_with_no_fixed;
	for (u32 count = 0; count < parent->number_of_children(); count++) {
		widget * c = parent->get_child(count);
		c = parent->get_child(count);

		s32 child_y  = y;
		s32 y_offset = 0;

		u32 len = get_len_from_policy(c->vertical_policy(), y, c->height(), remain_for_relative_height, pixel_per_relative);
		c->set_height(len);

		c->set_y(child_y + y_offset);

		y += c->height();
		if (c->vertical_policy().type() == alignment::relative) {
			remain_for_relative_height -= c->height();
		}

		c->resize(c->width(), c->height());
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

no_layout::~no_layout()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool no_layout::resize(widget * parent, u32 w, u32 h)
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool no_layout::add_widget(widget * parent, widget * w)
{
	object * p = static_cast<object *>(parent);
	return p->add_child(static_cast<object *>(w));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

}
}
}
