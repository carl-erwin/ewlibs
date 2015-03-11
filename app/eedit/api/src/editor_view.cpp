#include <map>

#include "handle_table.h"
#include "editor_types.h"
#include "editor_view.h"
#include "editor_buffer.h"
#include "codepoint_info.h"

#include "codec.h"

#include "editor_view_internal.h"

#include "application/application.hpp"

// INTERNAL
editor_view::editor_view(editor_view_id_t view_id_, editor_buffer_id_t editor_buffer_id_)
	: editor_buffer_id(editor_buffer_id_), view(view_id_)
{
	app_log << "loading input map configuration\n";
	auto bret = eedit::setup_default_input_map(input.event_map);
	assert(bret == true);

	// must remove this ? put split cotext in text-mode

	codepoint_info_reset(&start_cpi);
	start_cpi.used      = 1;

	app_log << "set default text codec\n";
	//
	codec_id = codec_get_by_name("text/ascii");

	//    setup_default_input_map(input_event_table);

	auto app = eedit::get_application();

	ew::graphics::fonts::init();

	app_log << " font filename " <<  app->font_file_name().c_str() << "\n";

	font.ft = new ew::graphics::fonts::font(app->font_file_name().c_str(), app->font_width(), app->font_height());
	font.ft->open();

	app_log << " font ptr " <<  font.ft << "\n";

}

editor_view::~editor_view()
{
	eedit::release_input_map(input.event_map);
	ew::graphics::fonts::quit();
}



static std::map<editor_view_id_t, editor_view *> table;

extern "C" {

	SHOW_SYMBOL
	int editor_view_bind(editor_view_id_t view_id, editor_buffer_id_t editor_buffer_id)
	{
		editor_view * vptr = nullptr;
		auto it = table.find(view_id);
		if (it == table.end()) {
			vptr =  new editor_view(view_id, editor_buffer_id);
			table[view_id] = vptr;
			app_log << __PRETTY_FUNCTION__ << " allocated view   = " << vptr << "\n";

		} else 	if (it->first) {
			vptr = it->second;
			app_log << __PRETTY_FUNCTION__ << " reuse view   = " << vptr << "\n";

		}

		vptr->editor_buffer_id = editor_buffer_id; // bind
		return 0;
	}

	SHOW_SYMBOL
	int editor_view_close(editor_view_id_t view_id)
	{
		assert(0);
		return 0;
	}

	SHOW_SYMBOL
	uint64_t editor_view_get_start_offset(editor_view_id_t view)
	{
		auto e = table.find(view);
		if (e == table.end())
			return 0;

		auto v = e->second;
		return v->start_offset;
	}

	SHOW_SYMBOL
	void editor_view_set_start_offset(editor_view_id_t view, uint64_t offset)
	{
		auto e = table.find(view);
		if (e == table.end())
			return;

		auto v = e->second;
		v->start_offset = offset;
	}


	SHOW_SYMBOL
	uint64_t editor_view_get_end_offset(editor_view_id_t view)
	{
		auto e = table.find(view);
		if (e == table.end())
			return 0;

		auto v = e->second;
		return v->end_offset;
	}


	SHOW_SYMBOL
	void editor_view_set_end_offset(editor_view_id_t view, uint64_t offset)
	{
		auto e = table.find(view);
		if (e == table.end())
			return;

		auto v = e->second;
		v->end_offset = offset;
	}



// view -> codec
	SHOW_SYMBOL
	codec_id_t         editor_view_get_codec_id(editor_view_id_t  view)
	{
		auto v = table.find(view);
		if (v == table.end())
			return 0;

		return (editor_font_t)v->second->codec_id;
	}

	SHOW_SYMBOL
	codec_context_id_t editor_view_get_codec_ctx(editor_view_id_t view)
	{
		// FIXME:
		return 0;
	}


	SHOW_SYMBOL
	editor_font_t editor_view_get_font(editor_view_id_t view)
	{
		auto v = table.find(view);
		if (v == table.end())
			return 0;

		// FIXME:
		return (editor_font_t)v->second->font.ft;
	}


	SHOW_SYMBOL
	void editor_view_set_region_changed_flag(editor_view_id_t view, bool flag)
	{
		auto v = table.find(view);
		if (v == table.end())
			return;

		v->second->region_changed = flag;
	}

	SHOW_SYMBOL
	bool editor_view_get_region_changed_flag(editor_view_id_t view)
	{
		auto v = table.find(view);
		if (v == table.end())
			return false;

		return v->second->region_changed;
	}

	SHOW_SYMBOL
	void editor_view_set_ui_need_refresh(editor_view_id_t view, bool flag)
	{
		auto v = table.find(view);
		if (v == table.end())
			return;

		v->second->ui_need_refresh = flag;
	}

	SHOW_SYMBOL
	bool editor_view_get_ui_need_refresh(editor_view_id_t view)
	{
		auto v = table.find(view);
		if (v == table.end())
			return false;

		return v->second->ui_need_refresh;
	}



	SHOW_SYMBOL
	void editor_view_set_ui_must_resync(editor_view_id_t view, bool flag)
	{
		auto v = table.find(view);
		if (v == table.end())
			return;

		v->second->ui_must_resync = flag;
	}

	SHOW_SYMBOL
	bool editor_view_get_ui_must_resync(editor_view_id_t view)
	{
		auto v = table.find(view);
		if (v == table.end())
			return false;

		return v->second->ui_must_resync;
	}

	SHOW_SYMBOL
	void editor_view_set_start_cpi(editor_view_id_t view, const codepoint_info_t * cpi)
	{
		auto v = table.find(view);
		if (v == table.end())
			return;

		memcpy(&v->second->start_cpi, cpi, sizeof (codepoint_info_t));
	}

	SHOW_SYMBOL
	void editor_view_get_start_cpi(editor_view_id_t view, codepoint_info_t * cpi)
	{
		auto v = table.find(view);
		if (v == table.end())
			return;

		memcpy(cpi, &v->second->start_cpi, sizeof (codepoint_info_t));
	}



} // ! extern "C"



editor_view * editor_view_get_internal_pointer(editor_view_id_t view)
{
	auto v = table.find(view);
	if (v == table.end())
		return nullptr;
	return v->second;
}

