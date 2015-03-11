#pragma once

#include "../eedit/api/include/editor_types.h"
#include "../eedit/api/include/mark.h"

// TODO: region.h
// simple enough
struct editor_region {
	mark_t * begin;
	mark_t * end;
	editor_view_id_t * owner;
};