#pragma once

#include "ew/graphics/font/font.hpp"

// TODO: move as font method/helper
inline uint32_t ft_compute_ascii_text_width(ew::graphics::fonts::font & ft, const char * vec, const uint32_t size)
{
    ew::graphics::fonts::font_grid grid;
    uint32_t w = 0;
    for (uint32_t i = 0; i < size; i++) {
        int32_t c = (int32_t)vec[i];
        ft.renderGlyph(c, &grid, 0, 0);
        w += ft.character_glyph_hori_advance(c);
    }

    return w;
}

// TODO: move as font method/helper
inline uint32_t  ft_compute_text_width(ew::graphics::fonts::font & ft, const int32_t * vec, const uint32_t size)
{
    ew::graphics::fonts::font_grid grid;
    uint32_t w = 0;
    for (uint32_t i = 0; i < size; i++) {
        int32_t c = vec[i];
        ft.renderGlyph(c, &grid, 0, 0);
        w += ft.character_glyph_hori_advance(c);
    }

    return w;
}
