#pragma once

#include <memory> /* for shared_ptr<> */
#include <mutex>

#include "ew/graphics/gui/event/event.hpp"

#include "core/core.hpp"
#include "ui/ew/main_window.hpp"

#include "scroll_bar.hpp"


#include "../../api/include/screen.h"

/*
TODO: scroll bar handling

    if (main_window_priv->m_buffer_view) {

        main_window_priv->m_buffer_view->m_text_view->txt_buffer()->lock();

        float s = main_window_priv->m_buffer_view->m_text_view->text_size();
        float b = main_window_priv->m_buffer_view->m_text_view->rdr_begin_offset();
        float e = main_window_priv->m_buffer_view->m_text_view->rdr_end_offset();

        main_window_priv->m_buffer_view->m_text_view->txt_buffer()->unlock();

        if (0) {
            app_log << " text_size = " << s << "\n";
            app_log << " text_b = " << b << "\n";
            app_log << " text_e = " << e << "\n";
        }
        if (s) {
            b /= s;

            e /= s;
        } else {
            b = 0.0f;
            e = 1.0f;
        }

        if (main_window_priv->m_buffer_view->m_scrool_bar) {

            main_window_priv->m_buffer_view->m_scrool_bar->set_begin_ratio( b );
            main_window_priv->m_buffer_view->m_scrool_bar->set_end_ratio( e );
        }
    }

*/

namespace  eedit
{

////////////////////////////////////////////////////////////////////////////////

class text_view : public ew::graphics::gui::widget
{
public:

	text_view(ew::graphics::gui::widget * parent)
	{
		m_parent = parent;
		set_name("text_view");
		m_screen_id = (u64)this;
		app_log << __PRETTY_FUNCTION__ << " m_screen_id = " << m_screen_id << "\n";
	}

	void set_scroll_area(eedit::scroll_area * scroll_a)
	{
		m_scrool_bar = scroll_a;
	}

	virtual ~text_view()
	{
		screen_release(m_screen);
		m_screen = nullptr;
	}

	bool send_rpc_event(const int ac,  const char ** av,  editor_buffer_id_t ebid, byte_buffer_id_t bid, editor_view_id_t screen_id, const screen_dimension_t & screen_dim)
	{
		auto msg       =  new eedit::core::rpc_call(ac, av);
		msg->src.kind  =  EDITOR_ACTOR_UI;
		msg->src.queue =  get_main_window()->event_queue();  //  TODO: ctx ?
		msg->dst.kind  =  EDITOR_ACTOR_CORE;

		msg->editor_buffer_id  =  ebid;
		msg->byte_buffer_id  =  0;
		msg->view_id  =  screen_id;
		msg->screen_dim =  screen_dim;

		assert(screen_dim.w);
		assert(screen_dim.h);

		app_log << " send_rcp_event : ui -> core @" << ew::core::time::get_ticks() << "\n";

		eedit::core::push_event(msg);
		return true;
	}


	bool on_create(const ew::graphics::gui::widget_event * ev)
	{
		assert(m_screen_id == (u64)this);

		m_screen_dim.w = width();
		m_screen_dim.h = height();
		m_screen_dim.c = width();
		m_screen_dim.l = height();

		app_log << __PRETTY_FUNCTION__ << "\n";
		app_log << __PRETTY_FUNCTION__ << " width() = " << width() << "\n";
		app_log << __PRETTY_FUNCTION__ << " height() = " << height() << "\n";
		app_log << __PRETTY_FUNCTION__ << " send_rpc_call\n";

		const char * func = "get_buffer_id_list";
		send_rpc_event(1,  &func, 0, 0, m_screen_id, m_screen_dim);
		view_state = request_buffer_id_list;

		return true;
	}

	std::vector<ew::graphics::gui::fonts::font_grid> ft_grid_array;

	u32 max_line_index()
	{

		if (screen() == nullptr) {
			return 0;
		}

		return screen_get_number_of_used_lines(screen()) + 1;
	}

	u64 line_num_by_index(u64 idx)
	{
		return 0;

		if (screen() == nullptr) {
			return 0;
		}

		return 0;
	}

	bool line_first_cp_index(u32 line_idx, u64 * cp_index)
	{
		//        app_log << __PRETTY_FUNCTION__ << " : line_idx = " << line_idx << "\n";
		*cp_index = u64(-1);
		if (screen() == nullptr) {
			return false;
		}

		const screen_line_t * l;
		const codepoint_info_s * cpi;

		int ret = screen_get_codepoint_info(screen(),line_idx, 0, &l, &cpi, screen_line_hint_no_column_fix);
		if (ret == 0) {
			return false;
		}

		*cp_index = cpi->cp_index; //
		return true;
	}


	bool line_start_offset_by_index(u32 line_idx, u64 * offset)
	{
		//        app_log << __PRETTY_FUNCTION__ << " : line_idx = " << line_idx << "\n";
		*offset = 0;
		if (screen() == nullptr) {
			return false;
		}

		const screen_line_t * l;
		const codepoint_info_t * cpi;

		bool ret = screen_get_codepoint_info(screen(), line_idx, 0, &l, &cpi, screen_line_hint_no_column_fix);
		if (ret == false) {
			return false;
		}

		*offset = cpi->offset;

		return true;
	}

	bool line_last_offset_by_index(u32 line_idx, u64 * offset)
	{
		//   app_log << __PRETTY_FUNCTION__ << " : line_idx = " << line_idx << "\n";
		*offset = 0;
		if (screen() == nullptr) {
			return false;
		}

		const screen_line_t * l;
		const codepoint_info_t * cpi;

		int ret = screen_get_codepoint_info(screen(), line_idx, 0xff, &l, &cpi, screen_line_hint_fix_column_overflow);
		if (ret == 0) {
			return false;
		}

		*offset = cpi->offset;
		return true;
	}


	s32 line_top_y_pos_by_index(u64 line_idx)
	{
		// app_log << __PRETTY_FUNCTION__ << " : line_idx = " << line_idx << "\n";

		if (screen() == nullptr) {
			return -1;
		}

		const screen_line_t * l;
		const codepoint_info_t * cpi;

		bool ret = screen_get_codepoint_info(screen(), line_idx, 0, &l, &cpi, screen_line_hint_no_column_fix);
		if (ret == false) {
			return false;
		}

		return cpi->y - cpi->h;
	}

	bool update_scroll_bar()
	{

		if (screen() == nullptr) {
			return true;
		}

		u64 sz   = screen_get_buffer_size(screen());

		const codepoint_info_t * first_cpinfo;
		const codepoint_info_t * last_cpinfo;
		screen_get_first_and_last_cpinfo(screen(), &first_cpinfo, &last_cpinfo);

		u64 boff = first_cpinfo->offset;
		u64 eoff = last_cpinfo->offset;

		if (0) {
			app_log << " text_size = " << sz << "\n";
			app_log << " b off = " << boff << "\n";
			app_log << " e off = " << eoff << "\n";
		}

		float b = 0.0;
		float e = 1.0;

		if (sz) {
			b = (float)boff / (float)sz;
			e = (float)eoff / (float)sz;
		} else {
			b = 0.0f;
			e = 1.0f;
		}

		if (0) {
			app_log << " text_size = " << sz << "\n";
			app_log << " b ratio = " << b << "\n";
			app_log << " e ratio = " << e << "\n";
		}


		if (m_scrool_bar) {
			m_scrool_bar->set_begin_ratio( b );
			m_scrool_bar->set_end_ratio( e );
		}


		return true;
	}


	bool render()
	{
		auto t0 = ew::core::time::get_ticks();

		if (screen() == nullptr) {
			app_log << __PRETTY_FUNCTION__ << "screen is null !!\n";
			return true;
		}

		// app_log << __PRETTY_FUNCTION__ << "screen OK !!\n";

		//        dump_screen_ref();


		if (screen_get_ready_flag(screen()) == 0) {
			assert(0);
			return true;
		}

		update_scroll_bar();


		u32 max_l;
		u32 max_c;
		screen_get_max_line_and_column(screen(), &max_l, &max_c);

		if (!max_l) {
			assert(0);
			return true;
		}

		if (!max_c) {
			assert(0);
			return true;
		}

		// TODO: cache this later :-) std::vector<ew::graphics::gui::fonts::font_grid> ?
		auto & ft_grid = ft_grid_array; // new ew::graphics::gui::fonts::font_grid[max_l * max_c];
		ft_grid.resize(max_l * max_c);
		u32 ft_index = 0;

		auto ft = get_font();
		assert(ft);


		// 2 pass rendering

		// TODO: if offset match a cursor + push a rect{x,y,w,h,col} in an array
		// for a 3rd pass with blending


		// FIXME: in future version the screen will provide a list marks



		struct selection_info {
			s32 x = 0;
			s32 y = 0;
			s32 w = 0;
			s32 h = 0;

			float r = 100.0f / 255.0f;
			float g = 180.0f / 255.0f;
			float b = 210.0 / 255.0f;
			float a = 1.0f;
		};

		// cache
		std::vector<selection_info> selections;
		selections.reserve(1024); // screen()->nr_selected_cp()

		// cache this in screen
		ew::graphics::fonts::font_glyph_info space_glyph_info;
		bool bret = ft->get_codepoint_glyph_info(' ', space_glyph_info);
		if (bret == false) {
			assert(0);
			return false;
		}

		// 1st pass render glyph
		// app_log << __PRETTY_FUNCTION__ << " screen_get_number_of_used_lines(screen()) = " << screen_get_number_of_used_lines(screen()) << "\n";

		for (u32 l = 0; l < screen_get_number_of_used_lines(screen()); l++) {

			// app_log << __PRETTY_FUNCTION__ << " l = " << l << "\n";

			const screen_line_t * lptr = nullptr;
			bool ret = screen_get_line(screen(), l, &lptr);
			if (ret == false) { // the line does not exist
				continue;
				// break;
			}

			for (u32 c = 0; c < screen_line_get_number_of_used_columns(lptr); c++) {

				// app_log << __PRETTY_FUNCTION__ << " c = " << c << "\n";
				const codepoint_info_t * cpi = nullptr;
				int ret = screen_line_get_cpinfo(lptr, c, &cpi, screen_line_hint_no_column_fix);
				assert(ret == 1);
				if (ret == 0)
					break;

				// add function compute selection
				// FIXME: this is a hack. let the core compute the marks
				// whith englobing box/line/interline
				int  build_selection = 0; // 1 sel 2 mark
				if (cpi->is_selected) {
					build_selection = 1;
				}

				selection_info selection;

				// FIXME: the font coordinate system is
				// inverted
				// must add enums
				// or better
				// save the (x,y) from the layout builder
				// and add a list of attributes
				// to codepoint_info
				//
				selection.w = space_glyph_info.hori_advance + 1;
				selection.h = space_glyph_info.vert_advance;

				selection.x = cpi->x;
				selection.y = cpi->y;

				if (selection.y)
					selection.y -= selection.h - 2;

				if (build_selection == 1) {
					selections.push_back(selection);
				}

				ft->renderGlyph(cpi->codepoint,
						&ft_grid[ft_index],
						cpi->x, cpi->y);

				float r = cpi->r;
				float g = cpi->g;
				float b = cpi->b;
				float a = cpi->a;

				// FIXME: hack
				// move to syntaxycr
				if (cpi->real_codepoint == '\t') {
					r = 0.0f;
					g = 1.0f;
					b = 0.0f;
				}

				if (cpi->real_codepoint == '\r') {
					r = 1.0f;
					g = 0.0f;
					b = 0.0f;
				}

				if (cpi->real_codepoint == '\n') {
					a = 0.45f;
				}

				if (cpi->real_codepoint == ';') {
					a = 0.45f;
				}

				// set color
				ft_grid[ft_index].v0.r = r;
				ft_grid[ft_index].v0.g = g;
				ft_grid[ft_index].v0.b = b;
				ft_grid[ft_index].v0.a = a;


				ft_grid[ft_index].v1.r = r;
				ft_grid[ft_index].v1.g = g;
				ft_grid[ft_index].v1.b = b;
				ft_grid[ft_index].v1.a = a;

				ft_grid[ft_index].v2.r = r;
				ft_grid[ft_index].v2.g = g;
				ft_grid[ft_index].v2.b = b;
				ft_grid[ft_index].v2.a = a;

				ft_grid[ft_index].v3.r = r;
				ft_grid[ft_index].v3.g = g;
				ft_grid[ft_index].v3.b = b;
				ft_grid[ft_index].v3.a = a;

#if 0
				app_log << "cpi->x = " << cpi->x << "\n";
				app_log << "cpi->y = " << cpi->y << "\n";
				app_log << "cpi->r = " << cpi->r << "\n";
				app_log << "cpi->g = " << cpi->g << "\n";
				app_log << "cpi->b = " << cpi->b << "\n";
				app_log << "cpi->a = " << cpi->a << "\n";


				app_log << "ft_grid[ft_index].v0.s " << ft_grid[ft_index].v0.s << "\n";
				app_log << "ft_grid[ft_index].v0.t " << ft_grid[ft_index].v0.t << "\n";
				app_log << "ft_grid[ft_index].v1.s " << ft_grid[ft_index].v1.s << "\n";
				app_log << "ft_grid[ft_index].v1.t " << ft_grid[ft_index].v1.t << "\n";
				app_log << "ft_grid[ft_index].v2.s " << ft_grid[ft_index].v2.s << "\n";
				app_log << "ft_grid[ft_index].v2.t " << ft_grid[ft_index].v2.t << "\n";
				app_log << "ft_grid[ft_index].v3.s " << ft_grid[ft_index].v3.s << "\n";
				app_log << "ft_grid[ft_index].v3.t " << ft_grid[ft_index].v3.t << "\n";
#endif

				++ft_index;
			}
		}

		/////////////////////////////////////////////////////////////////////


		// 2nd pass send array to gl
		ew_glEnable(GL_BLEND);
		ew_glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		ew_glEnable(GL_TEXTURE_2D);
		{
			ft->renderGrid(&ft_grid[0], ft_index);
		}
		ew_glDisable(GL_TEXTURE_2D);
		ew_glDisable(GL_BLEND);


		for (auto & sel : selections) {

			s32 & cX = sel.x;
			s32 & cY = sel.y;
			s32 & cW = sel.w;
			s32 & cH = sel.h;

			// ew_glColor4f(1.0f, 0.0, 0, 0.50f);
			ew_glEnable(GL_BLEND);

			ew_glColor4f(sel.r, sel.g, sel.b, sel.a);
			ew_glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_DST_ALPHA);

			// if overwrite : shape lines
			glBegin(GL_POLYGON);
			{
				glVertex2i(cX     , cY);       // up left
				glVertex2i(cX     , cY + cH);  // down-left
				glVertex2i(cX + cW, cY + cH);  // down-right
				glVertex2i(cX + cW, cY);       // up-right
			}
			glEnd();

			ew_glDisable(GL_BLEND);

		}

		auto t1 = ew::core::time::get_ticks();
		app_log << " TIME TO RENDER SCREEN (ft_index == "<< ft_index << "): " << std::dec << (t1 - t0) << "\n";

		pending_redraw = false;
		return true;
	}

	bool send_build_layout_event(u32 w, u32 h) const
	{
		app_log << __PRETTY_FUNCTION__ << " ui -> core @" << ew::core::time::get_ticks() << "\n";

		if (m_have_buffer_id ==  false) {
			app_log << __PRETTY_FUNCTION__ << " m_have_screen_id ==  false" << "\n";
			return false;
		}

		// ask for new layout
		auto msg             = new eedit::core::layout_event(EDITOR_BUILD_LAYOUT_EVENT);
		msg->id = 7;
		msg->src.kind        = EDITOR_ACTOR_UI;
		msg->src.queue       = get_main_window()->event_queue();
		msg->dst.kind        = EDITOR_ACTOR_CORE;

		assert(m_have_buffer_id);
		msg->editor_buffer_id  = this->m_ebuffer_id; //
		msg->byte_buffer_id  = this->m_buffer_id; //
		msg->view_id         = this->m_screen_id; //

		msg->screen_dim.w = width();
		msg->screen_dim.h = height();
		msg->screen_dim.c = width();
		msg->screen_dim.l = height();

		app_log << " send_build_layout_event : ui -> core @" << ew::core::time::get_ticks() << "\n";

		eedit::core::push_event(msg);
		return true;
	}

	bool resize(u32 w, u32 h)
	{
		app_log << __PRETTY_FUNCTION__ << "\n";
		app_log << __PRETTY_FUNCTION__ << " width() = " << width() << "\n";
		app_log << __PRETTY_FUNCTION__ << " height() = " << height() << "\n";
		set_width(w);
		set_height(h);

		m_screen_dim.w = width();
		m_screen_dim.h = height();

		send_build_layout_event(w, h);

		return true;
	}


	void screen(screen_t * scr)
	{
		screen_release(m_screen);
		m_screen = scr;
	}

	screen_t * screen() const
	{
		return m_screen;
	}

	//
	// move to
	main_window * get_main_window() const
	{
		widget * p = this->m_parent;
		ew::core::object * gp = static_cast<ew::core::object *>(p)->get_parent();
		main_window * main_win = static_cast<main_window *>(gp);
		return main_win;
	}

	//
	void setup_event_common_part(eedit::core::event * msg)
	{

		msg->id            = m_last_msg_id++;

		msg->editor_buffer_id = m_ebuffer_id;
		msg->byte_buffer_id = m_buffer_id;
		msg->view_id     = this->m_screen_id;             //
		assert(msg->view_id);

		msg->screen_dim.w = width();
		msg->screen_dim.h = height();
		msg->screen_dim.c = width();
		msg->screen_dim.l = height();

		msg->src.kind  = EDITOR_ACTOR_UI;
		msg->src.queue = get_main_window()->event_queue();
		msg->dst.kind  = EDITOR_ACTOR_CORE;

	}

	bool on_key_press(const  ew::graphics::gui::events::keyboard_event * ev)
	{
		app_log << __PRETTY_FUNCTION__ <<  "\n";
		// TODO: translate keyboard_event to eedit::core::event ?
		// send translated event to core thread

		if (this->m_have_buffer_id == false) {
			app_log << __PRETTY_FUNCTION__ <<  " no screen id defined\n";
			return true;
		}

		auto msg           = new eedit::core::keyboard_event();
		setup_event_common_part(msg);

		u32 mod_mask = 0;
		if (ev->ctrl != false) mod_mask |= input_event_s::mod_ctrl;
		if (ev->altL != false) mod_mask |= input_event_s::mod_altL;
		if (ev->altR != false) mod_mask |= input_event_s::mod_altR;

		// if (ev->ctrl != false) mod_mask |= keymap_key::mod_oskey;

		msg->ev = new eedit::input_event_s(ev->key,
						   input_event_s::no_range,
						   mod_mask,
						   ev->unicode);

		// display current key on console
		msg->ev->dump_event();
		app_log << "\n";

		eedit::core::push_event(msg);
		return false;
	}

	bool on_mouse_button_press(const button_event * ev)
	{
		app_log << __PRETTY_FUNCTION__ << "\n";
		app_log << "ev->x " << ev->x << "\n";
		app_log << "ev->y " << ev->y << "\n";
		app_log << "ev->button " << ev->button << "\n";

		auto msg           = new eedit::core::button_press_event();

		setup_event_common_part(msg);

		msg->ev = new eedit::input_event_s(input_event_s::button_press, ev->x, ev->y, ev->button);

		eedit::core::push_event(msg);

		return true;
	}

	bool on_mouse_button_release(const button_event * ev)
	{
		app_log << __PRETTY_FUNCTION__ << "\n";
		app_log << "ev->x " << ev->x << "\n";
		app_log << "ev->y " << ev->y << "\n";
		app_log << "ev->button " << ev->button << "\n";

		auto msg           = new eedit::core::button_release_event();

		setup_event_common_part(msg);

		msg->ev = new eedit::input_event_s(input_event_s::button_release, ev->x, ev->y, ev->button);

		eedit::core::push_event(msg);

		return true;
	}

	bool on_mouse_wheel_up(const button_event * ev)
	{
		app_log << __PRETTY_FUNCTION__ << "\n";
		app_log << "ev->x " << ev->x << "\n";
		app_log << "ev->y " << ev->y << "\n";
		app_log << "ev->button " << ev->button << "\n";

		auto msg           = new eedit::core::button_press_event();

		setup_event_common_part(msg);

		msg->ev = new eedit::input_event_s(input_event_s::button_press, ev->x, ev->y, ev->button);

		eedit::core::push_event(msg);

		return true;
	}

	bool on_mouse_wheel_down(const button_event * ev)
	{
		app_log << __PRETTY_FUNCTION__ << "\n";
		app_log << "ev->x " << ev->x << "\n";
		app_log << "ev->y " << ev->y << "\n";
		app_log << "ev->button " << ev->button << "\n";

		auto msg           = new eedit::core::button_press_event();

		setup_event_common_part(msg);

		msg->ev = new eedit::input_event_s(input_event_s::button_press, ev->x, ev->y, ev->button);

		eedit::core::push_event(msg);

		return true;
	}


	bool set_start_offset_by_ratio(float ratio)
	{
		if (this->m_buffer_id == 0) {
			return false;
		}

		assert(m_screen_id == (u64)this);
		m_screen_dim.w = width();
		m_screen_dim.h = height();
		m_screen_dim.c = width();
		m_screen_dim.l = height();

		//auto app = get_application();
		//auto list = app->buffer_desc_lst();

		char buffer[4][32] = {{ 0 }};
		int i  = 0;

		// func
		const char  * query_av[4] = {0};
		snprintf(buffer[i],  sizeof (buffer[i]), "%s", "set_screen_id_start_offset");
		query_av[i] = &buffer[i][0];
		++i;

		// ebid
		snprintf(buffer[i],  sizeof (buffer[i]), "%lu", this->m_ebuffer_id);
		query_av[i] = &buffer[i][0];
		++i;

		// sid
		snprintf(buffer[i],  sizeof (buffer[i]),  "%lu", this->m_screen_id);
		query_av[i] = &buffer[i][0];
		++i;

		// offset
		u64 offset = screen_get_buffer_size(screen()) * ratio;

		snprintf(buffer[i],  sizeof (buffer[i]),  "%lu", offset);
		query_av[i] = &buffer[i][0];
		++i;

		app_log << __PRETTY_FUNCTION__ << " send rpc -> to core : offset("<<offset<<") : OK\n";
		send_rpc_event(i, query_av, m_ebuffer_id, m_buffer_id, m_screen_id, m_screen_dim);

		return true;
	}

	bool process_editor_new_rpc_answer_ui_event(eedit::core::rpc_answer * msg)
	{
		app_log << __PRETTY_FUNCTION__ << " core -> ui @" << ew::core::time::get_ticks() << "\n";

		if (msg->ac == 0) {
			assert(0);
			return false;
		}

		std::string cmd(msg->av[0]);

		switch (view_state) {
		case request_buffer_id_list: {
			if (cmd ==  "get_buffer_id_list") {
				if (msg->ac < 2) {
					return false;
				}

				this->m_have_buffer_id = 1;
				this->m_ebuffer_id = atoi(msg->av[1]);
				this->m_buffer_id = 0;

				app_log << __PRETTY_FUNCTION__ << " select buffer_id " <<  m_ebuffer_id <<  "\n";
				view_state = initialized;
				send_build_layout_event(width(), height());
			}
		}
		break;

		case initialized:

			break;

		case not_initialized:
			assert(0);
			break;
		}

		return false;
	}

private:

	enum {
		not_initialized,
		request_buffer_id_list,
		initialized,
	} view_state = not_initialized;

	ew::graphics::gui::widget * m_parent      = nullptr;
	scroll_area     *     m_scrool_bar        = nullptr;

	u32                                  m_last_msg_id    = 0;
	editor_buffer_id_t                   m_ebuffer_id     = 0;
	byte_buffer_id_t                     m_buffer_id      = 0;
	bool                                 m_have_buffer_id = 0;
	editor_view_id_t                     m_screen_id      = 0;

	screen_dimension_t        m_screen_dim;
	screen_t *                m_screen = nullptr;
public:
	bool pending_redraw = false;
};

}


