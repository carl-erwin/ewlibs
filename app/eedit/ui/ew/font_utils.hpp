#pragma once

#include "ew/graphics/font/font.hpp"

// TODO: move as font method/helper
inline u32 ft_compute_ascii_text_width(ew::graphics::fonts::font & ft, const char * vec, const u32 size)
{
	ew::graphics::fonts::font_grid grid;
	u32 w = 0;
	for (u32 i = 0; i < size; i++) {
		s32 c = (s32)vec[i];
		ft.renderGlyph(c, &grid, 0, 0);
		w += ft.character_glyph_hori_advance(c);
	}

	return w;
}

// TODO: move as font method/helper
inline u32  ft_compute_text_width(ew::graphics::fonts::font & ft, const s32 * vec, const u32 size)
{
	ew::graphics::fonts::font_grid grid;
	u32 w = 0;
	for (u32 i = 0; i < size; i++) {
		s32 c = vec[i];
		ft.renderGlyph(c, &grid, 0, 0);
		w += ft.character_glyph_hori_advance(c);
	}

	return w;
}
