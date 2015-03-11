#include <memory>
#include <vector>
#include <list>
#include <set>
#include <map>


#include "ew/graphics/font/font.hpp"

#include "../../application/application.hpp"


#include "../../core/input/event/input_event_map.hpp"

#include "codepoint_info.h"
#include "mark.h"
#include "codec.h"

#include "byte_buffer.h"
#include "editor_buffer.h"
#include "editor_view.h"
#include "editor_region.h"
#include "editor_selection.h"

#include "handle_table.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
struct editor_buffer_s;

/// local table
static handle_index_allocator<editor_buffer_s> table;

///////////////////////////////////////////////////////////////////////////////////////////////////


/* fixme remove the rdr begin offet from editor buffer
   move the rdr begin offet from editor screen
*/
// TODO: buffer_info.h
// is buffer_info == buffer.h ?
struct editor_buffer_s {
	editor_buffer_s(byte_buffer_id_t bid, const std::string & file_name, const std::string & buffer_name, const std::string & font_file_name);

	~editor_buffer_s();

	std::set<editor_view_id_t> view;


	// TODO: build editor_view and return view_id
	bool add_buffer_view(editor_view_id_t view_id)
	{
		auto ret = view.insert(view_id);
		return ret.second;
	}

	std::shared_ptr<ew::graphics::fonts::font> font()
	{
		return m_font;
	}


	editor_buffer_id_t editor_buffer_id = 0;
	byte_buffer_id_t   bid  = 0;

	std::string file_name;   /* file path */
	std::string buffer_name; /* the bufer name : '*scratch*' '*Message*' etc.. ,  */
	std::string font_file_name;

	// flags
	bool buffer_changed  = false;
	// date last changed
	// date: last save

private:
	//{- TODO: default font : -> struct font_config.default.{normal,bold,italic,bold-italic} ?
	std::shared_ptr<ew::graphics::fonts::font> m_font;
	//-}

	//{-           marks list   // sorted by offset, use copy_if -> to screen (if visible on screen)
	// this list represent all the cursor found in all view to this buffer
	std::vector<mark_t *> marks; // todo use shared_pointer of marks + special delete function
	//-}

	//{-           region list // sorted
	std::list<editor_region> regions;
	//-}
};

///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////

editor_buffer_s::editor_buffer_s(byte_buffer_id_t bid_,
				 const std::string & file_name_,
				 const std::string & buffer_name_,
				 const std::string & font_file_name_)
	:
	bid(bid_),
	file_name(file_name_),
	buffer_name(buffer_name_),
	font_file_name(font_file_name_)

{
	app_log << __PRETTY_FUNCTION__ << " " << bid << " " << file_name <<  " " << buffer_name << " " << font_file_name << "\n";

	m_font  = std::make_shared<ew::graphics::fonts::font>(eedit::get_application()->font_file_name().c_str(),
			eedit::get_application()->font_width(),
			eedit::get_application()->font_height());
	if (m_font->open() == false) {
		assert(0);
	}
}


editor_buffer_s::~editor_buffer_s()
{
	app_log << __PRETTY_FUNCTION__ << " ENTER\n";
	app_log << __PRETTY_FUNCTION__ << " bid(" << bid << ") filename(" << file_name <<  ") buffer_name(" << buffer_name << ") font_file_name(" << font_file_name << ")\n";

	app_log << __PRETTY_FUNCTION__ << " number of view to delete " << view.size() << "\n";
// replace by unique_ptr ?
	int count = 0;
	for (auto it = view.begin(); it != view.end(); ) {
		app_log << __PRETTY_FUNCTION__ << " : count = " << count << " : deleting  view("<< *it <<")\n";
		editor_view_close(*it);
		// delete it->second;
		++it;
	}

	byte_buffer_close(bid);
	app_log << __PRETTY_FUNCTION__ << " LEAVE\n";
}


///////////////////////////////////////////////////////////////////////////////////////////////////


extern "C" {

	SHOW_SYMBOL
	byte_buffer_id_t editor_buffer_get_byte_buffer_id(editor_buffer_id_t editor_buffer_id)
	{
		auto t = table.get(editor_buffer_id);
		if (t == nullptr)
			return 0;

		return t->bid;

	}

	SHOW_SYMBOL
	editor_buffer_id_t editor_buffer_check_id(editor_buffer_id_t editor_buffer_id)
	{
		auto t = table.get(editor_buffer_id);
		if (t == nullptr)
			return INVALID_EDITOR_BUFFER_ID;

		return editor_buffer_id;
	}

	SHOW_SYMBOL
	editor_buffer_id_t editor_buffer_open(const char * filename, const char * buffer_name)
	{
		byte_buffer_id_t bid = byte_buffer_open(filename, 1 /* dummy open mode for now */);
		if (bid == 0) {
			return editor_buffer_id_t(INVALID_EDITOR_BUFFER_ID);
		}

		editor_buffer_s * edbuf;
		editor_buffer_id_t editor_buffer_id;
		std::tie(edbuf, editor_buffer_id) = table.construct(bid, filename, buffer_name, ""); // TODO font handling

		edbuf->editor_buffer_id = editor_buffer_id;

		app_log << __PRETTY_FUNCTION__ << " allocated bid    = " << bid << "\n";
		app_log << __PRETTY_FUNCTION__ << " allocated editor_buffer_id   = " << editor_buffer_id << "\n";
		return editor_buffer_id;
	}


	SHOW_SYMBOL
	editor_view_id_t editor_buffer_check_view_id(editor_buffer_id_t editor_buffer_id, editor_view_id_t view)
	{
		auto edbuf = table.get(editor_buffer_id);
		if (edbuf == nullptr)
			return 0;

		auto ret = edbuf->view.find(view);
		if (ret == edbuf->view.end()) {
			return 0;
		}

		return view;
	}

	SHOW_SYMBOL
	int	editor_buffer_add_view(editor_buffer_id_t editor_buffer_id, editor_view_id_t view, screen_dimension_t * dim)
	{
		auto edbuf = table.get(editor_buffer_id);
		if (edbuf == nullptr)
			return -1;

		auto ret = edbuf->view.find(view);
		if (ret != edbuf->view.end()) {
			app_log << " found already binded editor_view[edbuf("<<editor_buffer_id<<")] = " << view << "\n";
			return 0;
		}

		edbuf->view.insert(view);
		app_log << " bind editor_view[edbuf("<<editor_buffer_id<<")] = " << view << "\n";

		editor_view_bind(view, editor_buffer_id);
		return 0;
	}

	SHOW_SYMBOL
	void editor_buffer_set_changed_flag(editor_buffer_id_t editor_buffer_id, bool flag)
	{
		auto edbuf = table.get(editor_buffer_id);
		if (edbuf == nullptr)
			return;

		edbuf->buffer_changed = flag;
	}

	SHOW_SYMBOL
	bool editor_buffer_get_changed_flag(editor_buffer_id_t editor_buffer_id)
	{
		auto edbuf = table.get(editor_buffer_id);
		if (edbuf == nullptr)
			return false;

		return edbuf->buffer_changed;
	}

}

