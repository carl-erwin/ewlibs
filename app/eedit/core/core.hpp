#pragma once

#include <mutex>
#include <memory>

#include "../application/application.hpp"
#include "editor_message.h"

#include "../core/process_event_ctx.h"


////////////////////////////////////////////////////////////////////////////////

namespace eedit
{

class application;

namespace core
{



struct selection_record_s {
    uint64_t bid;

    int b_x;
    int b_y;
    uint64_t b_offset;
    bool use = false;

    int e_x;
    int e_y;
    uint64_t e_offset;

    codepoint_info_s start_cpi;
};


struct core_context_t {
    std::mutex m_mtx;
    bool core_started = false;
    bool core_running = true;
    editor_event_queue_s * m_msg_queue;
};

////////////////////////////////////////////////////////////////////////////////


// FIXME: cleanup/remove all this ????
void main(std::shared_ptr<application> app);

bool push_event(struct editor_message_s * msg);

bool process_rpc_call_event(struct editor_message_s * msg);




bool notify_buffer_changes(struct editor_message_s * msg, codepoint_info_s * start_cpi, bool send_screen = false);



bool setup_screen_by_id(editor_buffer_id_t editor_buffer_id, byte_buffer_id_t bid, editor_view_id_t sid, screen_dimension_t & dim);

void send_event_to_ui(const struct editor_message_s * ev_in, struct editor_message_s * ev_out);

void send_new_layout_event_to_ui(const struct editor_message_s * ev_in, screen_t * screen);

bool build_screen_layout_from_event(struct editor_message_s * msg, const codepoint_info_s * start_cpi, screen_t * scr);


bool save_buffer(struct editor_message_s * msg);

int dump_buffer_log(struct editor_message_s * msg);

int quit_editor(struct editor_message_s * msg);


bool eval_input_event(struct editor_message_s * base_msg);
bool process_input_event(struct editor_message_s * msg);
bool release_event(struct editor_message_s * msg);

bool notify_buffer_changes(struct editor_message_s * msg, codepoint_info_s * start_cpi, bool send_screen);

bool trigger_new_layout(struct editor_message_s * msg);
bool check_input_message(struct editor_message_s * msg);
bool process_application_message(core_context_t * core_ctx, struct editor_message_s * msg);
bool process_editor_message(core_context_t * core_ctx, struct editor_message_s * msg);



} // ! namespace core

} // ! namespace eedit


