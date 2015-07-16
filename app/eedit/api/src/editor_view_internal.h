#pragma once

#include <map>
#include <vector>
#include <list>
#include <memory>

#include "handle_table.h"
#include "editor_types.h"
#include "editor_view.h"
#include "editor_buffer.h"
#include "codepoint_info.h"
#include "mark.h"

#include "editor_codec.h"

#include "ew/graphics/font/font.hpp"

#include "../../core/input/event/input_event_map.hpp" // FIXME: typedef uint64_t editor_input_event_map_id_t;


typedef struct screen_info {
    screen_info()
    {
        memset(&dim, 0, sizeof (dim));
        assert(last_screen == nullptr);
    }

    ~screen_info()
    {
        screen_release(last_screen);
    }

    uint64_t start_offset = 0;
    screen_dimension_t dim;
    screen_t * last_screen = nullptr;
} editor_view_screen_info_t;


struct editor_view {
    editor_view(editor_view_id_t view_, editor_buffer_id_t editor_buffer_id_);
    ~editor_view();

    editor_buffer_id_t editor_buffer_id = 0;
    editor_view_id_t   view             = 0; // TODO: rename in view_id
    codec_id_t codec_id                 = 0;
    // TODO: add codec_ctx


    bool region_changed   = false;
    bool ui_need_refresh  = false;
    bool ui_must_resync   = false;

    uint64_t start_offset = 0; // merge with start_cpi ?
    uint64_t end_offset   = 0;

    codepoint_info_t start_cpi;

    editor_view_screen_info_t  screen_info;

    // pointer to pointer
    std::list<screen_t * /* FIXME: special deleter tha call screen release lambda function ?*/> screen_pool; //

    struct {
        std::map<std::string, eedit::editor_input_event_map *> event_map;
        eedit::editor_input_event_map *                        cur_event_map     = nullptr;
        std::vector< eedit::input_map_entry * > *              last_keymap_entry = nullptr;
    } input;

    struct {
        std::string m_font_file_name;
        ew::graphics::fonts::font * ft;
    } font;


    struct {
        std::vector<mark_t> moving; // todo use shared_pointer of marks + special delete function
        std::vector<mark_t> fixed; // sorted
    } marks;

    mark_t main_mark = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

editor_view * editor_view_get_internal_pointer(editor_view_id_t view);
