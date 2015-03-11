#pragma once

#include <memory>

#include <ew/core/types/types.hpp>
#include <ew/graphics/color.hpp>

/*
 From freetype tut2

- retrieve glyph metrics
- manage glyph images
- retrieve global metrics (including kerning)
- render a simple string of text, with kerning
- render a centered string of text (with kerning)
- render a transformed string of text (with centering)
- access metrics in design font units when needed, and how to scale them to device space

*/

namespace ew
{
namespace graphics
{
namespace fonts   // ?
{

using namespace ew::core::types;

bool init();
bool quit();

class Glyph; // todo


struct font_glyph_info {
	s32  width;
	s32  height;
	s32  hori_bearing_x;
	s32  hori_bearing_y;
	s32  hori_advance;
	s32  vert_bearing_x;
	s32  vert_bearing_y;
	s32  vert_advance;
};


/* todo rename this in font_char desc ??? */
struct font_grid;

// TODO: rename to match opengl format
// vertex::format::TEX2D_RGBA_XYZ ?
// font_vertex_info is a better name

struct font_vertex_info { // GL_T2F_C4F_N3F_V3F,
	float  s = 0.0f;
	float  t = 0.0f;
	float  r = 0.0f;
	float  g = 0.0f;
	float  b = 0.0f;
	float  a = 1.0f;

	float  nx =  0.0f;
	float  ny =  0.0f;
	float  nz = -1.0f;

	float  x = 0.0f;
	float  y = 0.0f;
	float  z = 0.0f;
};

struct font_grid {
public:
	font_vertex_info v0;
	font_vertex_info v1;
	font_vertex_info v2;
	font_vertex_info v3;
};

class EW_GRAPHICS_EXPORT font
{
public:
	font(const char * filename , u32 pxWidth = 12, u32 pxHeight = 12);
	virtual ~font();

	virtual bool open();
	// TODO: add close ?

	virtual u32 get_character_texture_id(s32 c);

	//
	virtual u32 pixel_width(); // TODO : return ctor pxWidth
	virtual u32 pixel_height(); // TODO : return ctor pxHeight

	virtual bool renderGlyph(s32 c, font_grid * ft_grid, s32 x_pos, s32 y_pos);

	virtual void showCharacterInfo(s32 c);
	virtual void renderCharacter(s32 c, s32 x, s32 y);

	virtual void renderGrid(font_grid * grid, u32 nr);

	// Rect getCharDimension();
	virtual s32 character_width(s32 c);
	virtual s32 character_height(s32 c);
	virtual s32 character_bitmap_width(s32 c);
	virtual s32 character_bitmap_height(s32 c);
	virtual s32 character_bitmap_left(s32 c);
	virtual s32 character_bitmap_top(s32 c);
	virtual s32 character_advance_x(s32 c);
	virtual s32 character_advance_y(s32 c);

	// glyph info
	virtual s32 character_glyph_width(s32 c);
	virtual s32 character_glyph_height(s32 c);
	virtual s32 character_glyph_hori_bearing_x(s32 c);
	virtual s32 character_glyph_hori_bearing_y(s32 c);
	virtual s32 character_glyph_hori_advance(s32 c);
	virtual s32 character_glyph_vert_bearing_x(s32 c);
	virtual s32 character_glyph_vert_bearing_y(s32 c);
	virtual s32 character_glyph_vert_advance(s32 c);

	virtual bool get_codepoint_glyph_info(s32 codepoint, font_glyph_info & glyph_info);

private:
	class private_data;
	private_data * data;
};


// NEW
// TODO: replace x,y,width,height by rectangle_area
EW_GRAPHICS_EXPORT bool font_print(ew::graphics::fonts::font & ft,
				   s32 x, s32 y, u32 width, u32 height,
				   ew::graphics::colors::color3ub color,
				   s32 * cp_array, size_t nr_cp);

EW_GRAPHICS_EXPORT bool font_print(ew::graphics::fonts::font & ft,
				   s32 x, s32 y, u32 width, u32 height,
				   ew::graphics::colors::color3ub col,
				   char * str);


}
}
}
