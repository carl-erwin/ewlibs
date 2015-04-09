#pragma once

#include <mutex>
#include <memory>

#include "../application/application.hpp"
#include "editor_event.h"

#include "../core/process_event_ctx.h"


////////////////////////////////////////////////////////////////////////////////

namespace eedit
{

class application;

namespace core
{

enum build_screen_list_hints {
    build_screen_no_hints = 0,
    rewind_screen         = 1,
    resync_screen         = 2,
};


struct selection_record_s {
    u64 bid;

    int b_x;
    int b_y;
    u64 b_offset;
    bool use = false;

    int e_x;
    int e_y;
    u64 e_offset;

    codepoint_info_s start_cpi;
};


struct core_context_t {
    std::mutex m_mtx;
    bool core_started = false;
    bool core_running = true;
    editor_event_queue_s * m_msg_queue;
};

////////////////////////////////////////////////////////////////////////////////

// FIXME: move to view
struct screen_cache {
    screen_cache()
    {
        assert(last_screen == nullptr);
    }

    ~screen_cache()
    {
        screen_release(last_screen);
    }

    u64 start_offset = 0;
    screen_dimension_t dim;
    screen_t * last_screen = nullptr;
};


// FIXME: cleanup/remove all this ????
void main(std::shared_ptr<application> app);

bool push_event(struct editor_event_s * msg);

bool process_rpc_call_event(struct editor_event_s * msg);

screen_cache * get_screen_cache(u64 id);

void set_last_screen(u64 id, screen_t * scr);

screen_t * get_last_screen(u64 id);

bool notify_buffer_changes(struct editor_event_s * msg, codepoint_info_s * start_cpi, bool send_screen = false);

screen_t * get_previous_screen_by_id(u64 id);

screen_t * get_new_screen_by_id(u64 screen_id);



bool setup_screen_by_id(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, editor_view_id_t sid, screen_dimension_t & dim);

void send_event_to_ui(const struct editor_event_s * ev_in, struct editor_event_s * ev_out);

void send_new_layout_event_to_ui(const struct editor_event_s * ev_in, screen_t * screen);

bool build_screen_layout_from_event(struct editor_event_s * msg, const codepoint_info_s * start_cpi, screen_t * scr);


bool save_buffer(struct editor_event_s * msg);

int dump_buffer_log(struct editor_event_s * msg);

int quit_editor(struct editor_event_s * msg);


bool eval_input_event(struct editor_event_s * base_msg);
bool process_input_event(struct editor_event_s * msg);
bool release_event(struct editor_event_s * msg);

bool notify_buffer_changes(struct editor_event_s * msg, codepoint_info_s * start_cpi, bool send_screen);

bool process_build_layout_event(struct editor_event_s * msg);
bool check_input_msg(struct editor_event_s * msg);
bool process_application_event(core_context_t * core_ctx, struct editor_event_s * msg);
bool process_event(core_context_t * core_ctx, struct editor_event_s * msg);



} // ! namespace core

} // ! namespace eedit


