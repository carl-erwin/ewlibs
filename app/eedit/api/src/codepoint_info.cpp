
#include <assert.h>
#include "../include/codepoint_info.h"

void codepoint_info_reset(struct codepoint_info * info)
{
	info->codepoint      = ' ';
	info->real_codepoint = ' ';

	info->split_flag  = 0;
	info->split_count = 0;
	info->cp_index    = uint64_t(-1);

	info->is_selected = false;
	info->used        = false;
	info->italic      = false;
	info->bold        = false;
	info->underline   = false;

	info->offset = 0;
	info->size   = 0;

	// vec2f // (x, y)
	info->x = 0.0f;
	info->y = 0.0f;

	// vec2i   dim; // (w, h) ?
	info->w = 0;
	info->h = 0;

	// rect2i(vec2i, vec2i) (pos,dim) ?

	info->r = 0.25f;
	info->g = 0.25f;
	info->b = 0.25f;
	info->a = 1.0f;
}
