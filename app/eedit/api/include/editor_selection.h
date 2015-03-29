#pragma once

#include "editor_export.h"
#include "editor_types.h"
#include "mark.h"

// simple enough
struct editor_selection {
    mark_t * begin;
    mark_t * end;
    editor_view_id_t * owner;
};
