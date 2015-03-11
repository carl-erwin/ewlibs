#include <list>

#include <ew/core/program/environment.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/utils/utils.hpp>

#include <ew/core/threading/condition_variable.hpp>
using namespace ew::core::threading;


#include <ew/graphics/graphics.hpp>
#include <ew/graphics/gui/widget/window/window.hpp>
#include <ew/graphics/gui/event/event.hpp>
#include <ew/graphics/gui/event/push.hpp>

// TODO: #include "<ew/graphics/opengl.hpp>"
#include "../implementation/common/graphics/renderer/opengl/renderer.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGL.hpp"
#include "../implementation/common/graphics/renderer/opengl/libGLU.hpp"
using namespace ew::implementation::graphics::rendering::opengl;

#include "core/log.hpp"


#include "scroll_bar.hpp"
#include "tab_bar.hpp"
#include "line_number.hpp"
#include "buffer_view.hpp"
#include "main_window.hpp"

// TODO: the main window does not view the application ?
#include "application/application.hpp"


namespace  eedit
{

/*
 *  TODO:
 *
 *  for better layout: add
 *  std::vector<s32> unicode_string;
 *  u32 font_string_width(ft, s32 *, size_t n); --> sum of cp widths
 *  u32 font_string_height(ft, s32 *, size_t n);  --> max height in string
 *
 *  (later) will depend on layout direction
 *
 *  if key::down on last line got to end of current ( optionnal )
 */




#if 0

void ignore_event_type_befor_tick(event_type type , u32 tick)
{
	get_application()->display()->get_event_dispatcher()->drop_events_before_tick(type , tick);
}

void push_draw_event(ew::graphics::gui::widget * widget)
{
	push_draw_event(widget);

	u32 tick = ew::core::time::get_ticks() + 20;
	ignore_event_type_befor_tick(PointerMotion_event, tick);
	ignore_event_type_befor_tick(WidgetDrawEvent, tick);
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

class main_window::main_window_private
{
public:
	//
	main_window_private(main_window * owner)
	{
		assert(owner);
		m_owner = owner;

		m_tab_bar     = new tab_bar(m_owner);
		m_buffer_view = new buffer_view(m_owner);
		m_status_bar  = new status_bar(m_owner);
	}

	~main_window_private()
	{
		delete m_event_queue;
		delete m_tab_bar;
		delete m_buffer_view;
		delete m_status_bar;
	}

	bool create_tab_bar();
	bool create_buffer_view();
	bool create_status_bar();


	////////////////////////////////////////////////
	// helpers
	bool do_resize(main_window * owner, u32 w, u32 h);

	bool loop = true;

	////////////////////////////////////////////////
	// widgets
	//
	main_window * m_owner       = nullptr;
	tab_bar   *   m_tab_bar     = nullptr;
	buffer_view * m_buffer_view = nullptr;
	status_bar  * m_status_bar  = nullptr;

	std::shared_ptr<ew::graphics::fonts::font> m_font;

	eedit::core::event_queue<eedit::core::event *> * m_event_queue = nullptr;

	// ?
	double bg_r;
	double bg_g;
	double bg_b;
	double bg_a;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

main_window::main_window(ew::graphics::gui::display * dpy, window_properties & properties)
	:
	window(dpy, nullptr /* parent */, properties)
{
	set_name("main_window");

	m_priv                = new main_window_private(this);
}

main_window::~main_window()
{
	delete m_priv;
}

bool main_window::set_event_queue(eedit::core::event_queue<eedit::core::event *> * q)
{
	m_priv->m_event_queue = q;
	return true;
}

::eedit::text_view * main_window::get_text_view()
{
	return m_priv->m_buffer_view->m_text_view;
}

eedit::core::event_queue<eedit::core::event *> * main_window::event_queue(void)
{
	return m_priv->m_event_queue;
}

////////////////////////////////////////////////////////////////////////////////

bool process_editor_new_layout_ui_event(main_window * win, eedit::core::layout_event * msg)
{
	// move notification screen to ui_widget
	auto view = win->get_text_view();     // the message must contain a private ref to the "view" widget instead of  win->get_text_view2();

	if (!msg->screen) {
		app_log << __PRETTY_FUNCTION__ << " no screen in layout_event\n";
		return true;
	}

	// replace screen
	view->screen(msg->screen);
	msg->screen = nullptr;

	screen_set_ready_flag(view->screen(), 1);

	// push redraw event
	if (view->pending_redraw == 0) {
		view->pending_redraw = 1;

		app_log << "[" << ew::core::time::get_ticks() << "] : " <<  __PRETTY_FUNCTION__ << " PUSH DRAW EVENT\n";
		widget * p = dynamic_cast<ew::graphics::gui::widget *>(win);
		ew::graphics::gui::push_draw_event(static_cast<widget *>(p));
	}

	return true;
}

bool process_editor_new_rpc_answer_ui_event(main_window * win, eedit::core::rpc_answer * msg)
{
	app_log <<  __PRETTY_FUNCTION__ << "***************\n";

	for (auto i = 0; i < msg->ac; ++i) {
		app_log <<  __PRETTY_FUNCTION__ << " msg->av["<< i << "] = " <<  msg->av[i] << "\n";
	}

	if (msg->ac ==  0) {
		return false;
	}

	std::string  cmd(msg->av[0]);

	if (cmd !=  "get_buffer_id_list")
		return false;

	// move notification screen to ui_widget
	auto * view = win-> get_text_view();     // the message must contain a private ref to the "view" widget instead of  win->get_text_view2();
	view->process_editor_new_rpc_answer_ui_event(msg);


	return true;
}


bool release_editor_ui_event(eedit::core::event * msg)
{
	delete msg;
	return true;
}


/*
 * TODO: from msg get targeted child (view,  status_bar,  menu_bar,  line_number,  ...)
 *
 */
bool process_editor_ui_event(main_window * win, eedit::core::event * msg)
{
	bool ret = false;

	switch (msg->type) {
	case EDITOR_LAYOUT_NOTIFICATION_EVENT: {
		ret = process_editor_new_layout_ui_event(win, (eedit::core::layout_event *)msg);
	}
	break;

	case EDITOR_RPC_ANSWER_EVENT: {
		ret = process_editor_new_rpc_answer_ui_event(win, (eedit::core::rpc_answer *)msg);
	}
	break;

	case EDITOR_QUIT_APPLICATION_DEFAULT:
	case EDITOR_QUIT_APPLICATION_FORCED: {
		win->quit();
	}
	break;

	default: {
		app_log << __PRETTY_FUNCTION__ << " unhandled event msg->type(" << msg->type << ")\n";
		assert(0);
	}
	break;
	}

	release_editor_ui_event(msg);
	return ret;
}

void main_window::process_event_queue(void)
{
	assert(m_priv->m_event_queue != nullptr);
	if (m_priv->m_event_queue == nullptr) {
		assert(0);
		return;
	}

	auto t0 = ew::core::time::get_ticks();

	auto q = m_priv->m_event_queue;

	static size_t default_wait_time = 20;
	size_t wait_time = default_wait_time;

	eedit::core::event * msg = nullptr;
	q->wait(wait_time);
	auto nr = q->size();
	while (nr) {
		q->get(msg);
		process_editor_ui_event(this, msg);
		--nr;
	}

	auto t1 = ew::core::time::get_ticks();
	if (0) {
		app_log << "["<<t1<<"] ui time to process event  = " << t1 - t0 << "\n";
		app_log << "["<<t1<<"] ui event queue size = " << q->size() << "\n";
	}
}

////////////////////////////////////////////////////////////////////////////////

/*

    main window
    + vertical layout
      * the menu_bar
      * the buffer view
        + vertical layout
          * the line number
          * the text_view
          * the scrool bar
      * the status bar
 */

////////////////////////////////////////////////////////////////////////////////////////////////////

// will become tab_bar::on_create(ev)
bool main_window::main_window_private::create_tab_bar()
{
	auto ft = m_owner->get_font();
	m_tab_bar->set_font(ft);

	m_tab_bar->vertical_policy().type() = alignment::fixed;
	m_tab_bar->set_height(1 + ft->pixel_width() + 1);

	m_tab_bar->horizontal_policy().type()  = alignment::relative;
	m_tab_bar->horizontal_policy().ratio() = 100;
	m_tab_bar->set_width(m_owner->width());

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


// will become status_bar::on_create
bool main_window::main_window_private::create_status_bar()
{
	// debug
	if (m_status_bar == nullptr)
		return true;

	auto ft = m_owner->get_font();
	m_status_bar->set_font(ft);

	m_status_bar->vertical_policy().type() = alignment::fixed;
	m_status_bar->set_height(1 + ft->pixel_width() + 1);

	m_status_bar->horizontal_policy().type() = alignment::relative;
	m_status_bar->horizontal_policy().ratio() = 100;
	m_status_bar->set_width(m_owner->width());

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////


// will become buffer_view::on_create(ev)

bool main_window::main_window_private::create_buffer_view()
{
	auto ft = m_owner->get_font();
	m_buffer_view->set_font(ft);

	m_buffer_view->set_width(m_owner->width());
	m_buffer_view->set_height(m_owner->height());
	m_buffer_view->vertical_policy().type()    = alignment::relative;
	m_buffer_view->vertical_policy().ratio()   = 100;
	m_buffer_view->horizontal_policy().type()  = alignment::relative;
	m_buffer_view->horizontal_policy().ratio() = 100;

	// populate buffer view
	widget * cur_parent = m_buffer_view;

	//
	cur_parent->set_layout(new horizontal_layout);

	// setup children

	m_buffer_view->m_line_number->set_font(ft);
	m_buffer_view->m_line_number->set_textview(m_buffer_view->m_text_view);
	//   assert(m_buffer_view->m_text_view);

	// line number /////////////////////////////////////////////////////////////////////
#if 1
	u32 line_number_w = 1 + (9 * ft->pixel_width()) + 1; // TODO: max 16 digits + @
	line_number_w = 12;
	app_log << "line_number_w = " << line_number_w << "\n";
	m_buffer_view->m_line_number->set_width(line_number_w);
	m_buffer_view->m_line_number->set_height(m_owner->height());
	m_buffer_view->m_line_number->horizontal_policy().type() = alignment::fixed;
	m_buffer_view->m_line_number->horizontal_policy().ratio() = 100;
	m_buffer_view->add_widget(m_buffer_view->m_line_number);
#endif


	// text view ///////////////////////////////////////////////////////////////////
	m_buffer_view->m_text_view->set_font(ft);
	m_buffer_view->m_text_view->set_width(m_owner->width());
	m_buffer_view->m_text_view->set_height(m_owner->height());
	m_buffer_view->m_text_view->horizontal_policy().type() = alignment::relative;
	m_buffer_view->m_text_view->horizontal_policy().ratio() = 100;

	// add text_view2 to buffer_view
	m_buffer_view->add_widget(m_buffer_view->m_text_view);

	// scroll bar /////////////////////////////////////////////////////////////////////
#if 1
	u32 scrool_bar_w = 12; // TODO: put this in header
	m_buffer_view->m_scrool_bar->set_height(m_owner->height());
	m_buffer_view->m_scrool_bar->set_width(scrool_bar_w);
	m_buffer_view->m_scrool_bar->horizontal_policy().type() = alignment::fixed;
	m_buffer_view->m_scrool_bar->horizontal_policy().ratio() = 100;
	m_buffer_view->m_scrool_bar->set_begin_ratio(0.0f);
	m_buffer_view->m_scrool_bar->set_end_ratio(0.0f);
	m_buffer_view->add_widget(m_buffer_view->m_scrool_bar);
	///////////////////////////////////////////////////////////////////////////////////
#endif

	// link view and scrool bar
	m_buffer_view->m_text_view->set_scroll_area( m_buffer_view->m_scrool_bar );
	m_buffer_view->m_scrool_bar->set_textview(m_buffer_view->m_text_view);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


bool main_window::on_create(const widget_event * ev)
{
	app_log << __FUNCTION__ << " main_window::on_create\n";
	app_log << __FUNCTION__ << " ev->width  = " << ev->width  << "\n";
	app_log << __FUNCTION__ << " ev->height = " << ev->height << "\n";

	set_width(ev->width);
	set_height(ev->height);

	assert(width());
	assert(height());

	// setup_depth()
	ew_glClearDepth(1.0f);

	// setup_backgroud_colors()
	m_priv->bg_r = (float)(0xf7) / 255.0;
	m_priv->bg_g = (float)(0xfe) / 255.0;
	m_priv->bg_b = (float)(0xdf) / 255.0;

	m_priv->bg_r = (float)(0xe9) / 255.0; // 233
	m_priv->bg_g = (float)(0xe9) / 255.0;
	m_priv->bg_b = (float)(0xe9) / 255.0;

	m_priv->bg_r = (float)(0xf1) / 255.0; // 233
	m_priv->bg_g = (float)(0xf1) / 255.0;
	m_priv->bg_b = (float)(0xf1) / 255.0;
	m_priv->bg_a = 100.0 / 100.0;


	////////////////
	widget * cur_parent = this;

	// setup_font()
	auto ft = std::make_shared<ew::graphics::fonts::font>(get_application()->font_file_name().c_str(),
			get_application()->font_width(),
			get_application()->font_height());
	if (ft->open() == false) {
		return false;
	}

	m_priv->m_font = ft;

	cur_parent->set_font(ft);

	// setup_layout()
	cur_parent->set_layout(new vertical_layout);

	m_priv->create_tab_bar();
	m_priv->create_status_bar();
	m_priv->create_buffer_view();

	auto ret = widget::on_create(ev);
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::on_map(const widget_event * ev)
{
	// TODO: get current position
	// or propage reparent event as on_drawEvent
	// move(0, 0);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::clear()
{

	// TODO: crash if the widow is very small
#if 0
	app_log << " clear R = " << main_window_priv->bg_r << "\n";
	app_log << " clear G = " << main_window_priv->bg_g << "\n";
	app_log << " clear B = " << main_window_priv->bg_b << "\n";
	app_log << " clear a = " << main_window_priv->bg_a << "\n";
#endif
	// set bg color for blending
	ew_glClearColor(m_priv->bg_r,
			m_priv->bg_g,
			m_priv->bg_b,
			m_priv->bg_a)
	;
	ew_debug_glGetError("ew_glClearColor", __FUNCTION__, __LINE__);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ew_debug_glGetError("ew_glClearColor", __FUNCTION__, __LINE__);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::set_projection()
{
	// move to TextView
	// use the window coordinate system
	//  (0,0) --------> (w,0)
	//   |
	//   |
	//   |
	//   |
	//  (0,h)
	//
	ew_glMatrixMode(GL_PROJECTION);
	ew_glLoadIdentity();
	ew_glOrtho((GLdouble) 0.0,      /* left   */
		   (GLdouble) width(),  /* right  */
		   (GLdouble) height(), /* bottom */
		   (GLdouble) 0.0,      /* top    */
		   (GLdouble) - 1000.0, /* near   */
		   (GLdouble) 1000.0    /* far    */);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::set_modelview()
{
	ew_glMatrixMode(GL_MODELVIEW);
	ew_glLoadIdentity();
	ew_glViewport(0, 0, width(), height());
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////


bool main_window::on_resize(const widget_event * ev)
{
	resize(ev->width, ev->height);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::on_close(const widget_event * ev)
{
	auto msg       =  new eedit::core::application_event(EDITOR_QUIT_APPLICATION_DEFAULT);
	msg->src.kind  =  EDITOR_ACTOR_UI;
	msg->src.queue =  event_queue();  //  TODO: ctx ?
	msg->dst.kind  =  EDITOR_ACTOR_CORE;
	msg->view_id =  0;

	app_log << " send quit app event : ui -> core @" << ew::core::time::get_ticks() << "\n";
	eedit::core::push_event(msg);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::on_mouse_wheel_up(const button_event * ev)
{
	app_log << __PRETTY_FUNCTION__ << " : DISABLED !!\n";
	return true;

//    main_window_priv->m_buffer_view->m_text_view->on_mouse_wheel_up(ev);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::on_mouse_wheel_down(const button_event * ev)
{
	app_log << __PRETTY_FUNCTION__ << " : DISABLED !!\n";
	return true;

//    main_window_priv->m_buffer_view->m_text_view->on_mouse_wheel_down(ev);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: update widget::on_event to do this
//
bool main_window::on_mouse_button_press(const button_event * ev)
{
	app_log << __PRETTY_FUNCTION__ << "\n";
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::on_mouse_button_release(const button_event * ev)
{
	app_log << __PRETTY_FUNCTION__ << "";

	bool ret = true;

	if (!ev) {
		// ???
		return false;
	}

	button_event new_ev = *ev;
	ew::graphics::gui::widget * wid = get_selected_child();

	if (wid == nullptr) {
		return true;
	}

	if (wid != this) {
		new_ev.x -= wid->x();
		new_ev.y -= wid->y();
		ret = wid->on_mouse_button_release(&new_ev);
	}

	select_child(nullptr);
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::on_key_press(const  ew::graphics::gui::events::keyboard_event * ev)
{
	if (m_priv->m_buffer_view) {
		return m_priv->m_buffer_view->on_key_press(ev);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::on_key_release(const ew::graphics::gui::events::keyboard_event * ev)
{
	if (m_priv->m_buffer_view) {
		return m_priv->m_buffer_view->on_key_release(ev);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::on_pointer_motion(const  ew::graphics::gui::events::pointer_event * ev)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::on_pointer_enter(const  ew::graphics::gui::events::pointer_event * ev)
{
	// scene graph
	//m_last_selected_wimain_window_priv->on_pointer_enter(ev);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::on_pointer_leave(const pointer_event * ev)
{
	// scene graph
	// m_last_selected_wimain_window_priv->on_pointer_leave(ev);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/*
    add a call back for char colors
    must have  sub routine called print_char to handle codepoint expension:
    cp(0x00) => \x00
    cp(0x01) => \x01 or ESC with special color
    cp(tab) => '    ' 4 spaces
    etc...
    move away blending setup
  */

bool main_window::on_draw(const widget_event * ev)
{
	return widget::render();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

status_bar * main_window::get_status_bar()
{
	if (m_priv) {
		return m_priv->m_status_bar;
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::quit()
{
	m_priv->loop = false;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_window::loop()
{
	return m_priv->loop;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // ! namespace eedit

