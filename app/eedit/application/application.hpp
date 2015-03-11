#pragma once

#include <utility>
#include <list>
#include <memory>

#include "ew/core/types/types.hpp"
using namespace ew::core::types;

#include "ew/graphics/gui/display/display.hpp"

#include "editor_buffer.h"

namespace eedit
{

///////////////////////////////////////////////////////////////////////////////////////////////////

class application;

struct user_interface {
	virtual ~user_interface() { }
	virtual bool setup(application * app) = 0;
	virtual bool main_loop() = 0;
	virtual ew::graphics::gui::display * get_display() = 0;
};

class application
{
	class application_private;
	std::unique_ptr<application_private> m_priv;
public:
	application();
	virtual ~application();

	bool parse_command_line(int ac, char ** av);

	bool set_ui_name(const char * ui_name);
	const std::string &  ui_name() const;

	void ui(std::unique_ptr<user_interface> ui);
	user_interface * ui() const;

//
	bool set_display(ew::graphics::gui::display * dpy);
	ew::graphics::gui::display * display();

//
	void set_font_file_name(char * font_file_name);
	const std::string & font_file_name() const;
	u32 font_width();
	u32 font_height();

// move to mode
	bool & build_index_flag();

	bool & offscreen_buffer_flag(); // move to gui config

//
	std::list<std::string> & files_list();

	const std::list<editor_buffer_id_t> & buffer_desc_lst();

	std::pair<bool, editor_buffer_id_t>   get_editor_buffer_by_byte_buffer_id(byte_buffer_id_t id);
	std::pair<bool, editor_buffer_id_t>   get_editor_buffer_by_editor_view_id(byte_buffer_id_t id);

	void set_default_font_size(u32 sz);
	bool run(int ac, char ** av);
	bool quit() const;

};

std::shared_ptr<application> get_application();
void          set_application(std::shared_ptr<application> app);

} // ! namespace eedit
