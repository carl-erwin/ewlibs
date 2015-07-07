#pragma once

#include "editor_export.h"
#include "layout_filter_io.h"

extern "C" {

    typedef struct layout_io_vec_s * layout_io_vec_t;


    EDITOR_EXPORT
    layout_io_vec_t layout_io_vec_new();

    EDITOR_EXPORT
    int layout_io_vec_release(layout_io_vec_t vec);

    EDITOR_EXPORT
    size_t layout_io_vec_size(layout_io_vec_t vec);

    EDITOR_EXPORT
    int layout_io_vec_clear(layout_io_vec_t vec);

    EDITOR_EXPORT
    layout_io_t * layout_io_vec_first(layout_io_vec_t vec);

    EDITOR_EXPORT
    layout_io_t * layout_io_vec_last(layout_io_vec_t vec);

    EDITOR_EXPORT
    int layout_io_vec_push(layout_io_vec_t vec, layout_io_t * io);  // write

    EDITOR_EXPORT
    int layout_io_vec_pop(layout_io_vec_t vec, layout_io_t * io);   // cancel-write

    EDITOR_EXPORT
    int layout_io_vec_get(layout_io_vec_t vec, layout_io_t * io);   // read

    EDITOR_EXPORT
    int layout_io_vec_unget(layout_io_vec_t vec, layout_io_t * io); // un-read

    // FUTURE VERSION
    EDITOR_EXPORT
    int layout_io_vec_notify(layout_io_vec_t vec); // later for multi thread

    EDITOR_EXPORT
    int layout_io_vec_wait(layout_io_vec_t vec);   // later for mutil thread
}
