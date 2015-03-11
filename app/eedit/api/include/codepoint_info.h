#pragma once

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif


/* codepoint_info hold information about the text to be displayed */
struct codepoint_info_s {
	// unicode
	int32_t  codepoint;
	int32_t  real_codepoint;

	// screen info
	uint32_t split_flag;
	uint32_t split_count;
	uint64_t cp_index;

	// style
	bool is_selected;
	bool used;
	bool italic;
	bool bold;
	bool underline;

	// position in buffer
	uint64_t  offset;
	uint64_t  size;

	// rect2i(vec2i, vec2i) (pos,dim) ?
	// vec2f // (x, y)
	float x;
	float y;

	// vec2i   dim; // (w, h) ?
	int32_t w;
	int32_t h;


	// color4f fg;
	float r;
	float g;
	float b;
	float a;

	// font info : the rendering must sort by texture id
};

typedef struct codepoint_info_s codepoint_info_t;

inline void codepoint_info_reset(codepoint_info_t * info)
{
	// unicode
	info->codepoint      = ' '; // the codepoint to display
	info->real_codepoint = ' '; // the codepoint before filtering  '\n' -> ' ' , '\t'  -> '  ->  ', simple enough

	info->split_flag  = 0;
	info->split_count = 0;
	info->cp_index    = uint64_t(-1);

	// style
	info->is_selected = false;
	info->used        = false;
	info->italic      = false;
	info->bold        = false;
	info->underline   = false;

	info->offset = 0; // offset in raw buffer
	info->size   = 0; // next offset, ie fold etc...

	// vec2f // (x, y)
	info->x = 0.0f;
	info->y = 0.0f;

	// vec2i   dim; // (w, h) ?
	info->w = 0;
	info->h = 0;

	// rect2i(vec2i, vec2i) (pos,dim) ?

	// color4f fg;
	info->r = 0.25f;
	info->g = 0.25f;
	info->b = 0.25f;
	info->a = 1.0f;

	// color4f bg;

	// font info : the rendering must sort by texture id
}


#ifdef __cplusplus
}
#endif
