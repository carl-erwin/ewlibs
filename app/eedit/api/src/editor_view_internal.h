#pragma once

#include <map>
#include <vector>
#include <memory>

#include "handle_table.h"
#include "editor_types.h"
#include "editor_view.h"
#include "editor_buffer.h"
#include "codepoint_info.h"

#include "codec.h"

#include "ew/graphics/font/font.hpp"

#include "../../core/input/event/input_event_map.hpp" // FIXME: typedef uint64_t editor_input_event_map_id_t;

struct editor_view {
	editor_view(editor_view_id_t view_, editor_buffer_id_t editor_buffer_id_);
	~editor_view();

	editor_buffer_id_t editor_buffer_id = 0;
	editor_view_id_t   view             = 0;
	codec_id_t codec_id                 = 0;
	// TODO: add codec_ctx


	bool region_changed   = false;
	bool ui_need_refresh  = false;
	bool ui_must_resync   = false;

	uint64_t start_offset = 0; // merge with start_cpi ?
	uint64_t end_offset   = 0;

	codepoint_info_t start_cpi;

	struct {
		std::map<std::string, eedit::editor_input_event_map *> event_map;
		eedit::editor_input_event_map *                        cur_event_map     = nullptr;
		std::vector< eedit::input_map_entry * > *              last_keymap_entry = nullptr;
	} input;

	struct {
		std::string m_font_file_name;
		ew::graphics::fonts::font * ft;
	} font;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

editor_view * editor_view_get_internal_pointer(editor_view_id_t view);
