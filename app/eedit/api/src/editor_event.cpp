#include <memory>
#include <functional>
#include <ew/Utils.hpp>
#include "../../core/input/event/input_event_map.hpp"

#include "editor_event.h"


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

static inline void print_args(const int ac, char ** & av)
{
    for (int i = 0; i < ac; ++i) {
        app_log <<  "av[" << i << "] = " << av[i] <<  "\n";
    }

}


struct rpc_call : public event {

    rpc_call(const int ac_,  const char ** av_) : event(EDITOR_RPC_CALL_EVENT)
    {
        duplicate_args(ac_, av_, ac,  av);
        //print_args(ac, av);
    }

    virtual ~rpc_call()
    {
        release_args(ac, av);
    }

    int  ac    = 0;
    char ** av = nullptr;
};

struct rpc_answer : public event {
    rpc_answer(const rpc_call * request,  const int ac_,  const char ** av_) : event(EDITOR_RPC_ANSWER_EVENT)
    {
        if (request)
            this->id = request->id;

        duplicate_args(ac_, av_, ac,  av);
        //print_args(ac, av);
    }

    virtual ~rpc_answer()
    {
        release_args(ac, av);
    }

    int  ac    = 0;
    char ** av = nullptr;
};


struct input_event : public event {
    input_event(editor_event_type_e type)
        : event(type)
    {

    }

    ::eedit::input_event_s * ev      = nullptr; // move to ? src_ctx ?

    virtual ~input_event()
    {
        delete ev; // per event type
    }

};

