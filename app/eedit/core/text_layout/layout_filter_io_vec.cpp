#include <deque>

#include "layout_filter_io.h"
#include "layout_filter_io_vec.h"


struct layout_io_vec_s {
    std::deque<layout_io_t> q;
};



extern "C" {

    EDITOR_EXPORT
    layout_io_vec_t layout_io_vec_new()
    {
        auto vec = new struct layout_io_vec_s;
        return vec;
    }


    EDITOR_EXPORT
    int layout_io_vec_release(layout_io_vec_t vec)
    {
        delete vec;
        return 0;
    }


    EDITOR_EXPORT
    size_t layout_io_vec_size(layout_io_vec_t vec)
    {
        return vec->q.size();
    }


    EDITOR_EXPORT
    int layout_io_vec_clear(layout_io_vec_t vec)
    {
        vec->q.clear();
        return 0;
    }

    EDITOR_EXPORT
    layout_io_t * layout_io_vec_first(layout_io_vec_t vec)
    {
        if (!vec || !vec->q.size())
            return nullptr;

        return &vec->q.front();
    }



    EDITOR_EXPORT
    layout_io_t * layout_io_vec_last(layout_io_vec_t vec)
    {
        if (!vec || !vec->q.size())
            return nullptr;

        return &vec->q.back();
    }

    // FOR LAYOUT MODULES MODULE
    EDITOR_EXPORT
    int layout_io_vec_push(layout_io_vec_t vec, layout_io_t * io)
    {
        vec->q.push_back(*io);
        return 0;
    }

    EDITOR_EXPORT
    int layout_io_vec_pop(layout_io_vec_t vec, layout_io_t * io)
    {
        if (!vec->q.size())
            return -1;

        *io = vec->q.back();
        vec->q.pop_back();
        return 0;
    }


    EDITOR_EXPORT
    int layout_io_vec_get(layout_io_vec_t vec, layout_io_t * io)
    {
        if (!vec->q.size())
            return 0;


        *io = vec->q.front();
        vec->q.pop_front();
        return 1;
    }


    EDITOR_EXPORT
    int layout_io_vec_unget(layout_io_vec_t vec, layout_io_t * io)
    {
        vec->q.push_front(*io);
        return 0;
    }


    // FUTURE VERSION
    EDITOR_EXPORT
    int layout_io_vec_notify(layout_io_vec_t vec)
    {

        return 0;
    }


    EDITOR_EXPORT
    int layout_io_vec_wait(layout_io_vec_t vec)
    {

        return 0;
    }

}
