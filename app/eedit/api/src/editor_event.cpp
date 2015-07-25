#include <cstring>
#include <memory>
#include <functional>
#include <ew/Utils.hpp>
#include "../../core/input/event/input_event_map.hpp"

#include "editor_message.h"

extern "C"
struct editor_message_s * editor_event_alloc()
{
    struct editor_message_s * ev = static_cast<struct editor_message_s *>(::operator new (sizeof (struct editor_message_s)));
    memset(ev, 0, sizeof (struct editor_message_s));
    return ev;
}


// move  to .cpp
static inline void duplicate_args(const int in_ac, const char ** in_av,
                                  int & out_ac,  char ** & out_av)
{
    out_ac = in_ac;
    out_av = new char * [out_ac];
    for (int i = 0; i < out_ac; ++i)
        out_av[i] = ew::utils::c_string_dup(in_av[i]);
}

static inline void release_args(const int ac, char ** & av)
{
    for (int i = 0; i < ac; ++i)
        delete [] av[i];
    delete [] av;
    av = 0;
}

inline void print_args(const int ac, char ** & av)
{
    for (int i = 0; i < ac; ++i) {
        app_logln(-1, "av[%d] = '%s'", av[i]);
    }

}



struct editor_message_s * editor_layout_event_new(editor_message_type_e type)
{
    auto ev = editor_event_alloc();
    ev->type = type;
    return ev;
}

struct editor_message_s * editor_rpc_call_new(int call_ac, const char ** call_av)
{
    auto ev = editor_event_alloc();
    ev->type = EDITOR_RPC_CALL_EVENT;
    duplicate_args(call_ac, call_av, ev->rpc.ac, ev->rpc.av);
    return ev;
}

struct editor_message_s * editor_rpc_answer_new(struct editor_message_s * request, int answer_ac, const char ** anwser_av)
{
    auto ev = editor_event_alloc();
    ev->type = EDITOR_RPC_ANSWER_EVENT;
    duplicate_args(answer_ac, anwser_av, ev->rpc.ac, ev->rpc.av);
    return ev;
}


extern "C"
void editor_event_free(struct editor_message_s * ev)
{
    if (!ev)
        return;

    switch (ev->type) {
    case EDITOR_RPC_CALL_EVENT:
    case EDITOR_RPC_ANSWER_EVENT:
        release_args(ev->rpc.ac, ev->rpc.av);
        break;
    default:
        break;
    }


    if (ev) ::operator delete ((void *)ev);
}
