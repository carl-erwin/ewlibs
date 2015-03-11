#pragma once


// freetype 2
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPES_H
#include FT_GLYPH_H

// Dll
#include <ew/core/dll/dll.hpp>
#include <ew/core/exception/exception.hpp>

#include <ew/core/wrapper/wrapper.hpp>

namespace ew
{
namespace graphics
{
namespace fonts
{

using namespace ew::core::exceptions;

EW_GRAPHICS_EXPORT bool load_libfreetype2_dll();
EW_GRAPHICS_EXPORT bool unload_libfreetype2_dll();

DECLARE_wrapper(FT_Error, FT_Init_FreeType, FT_Library * alibrary);

DECLARE_wrapper(void, FT_Library_Version, FT_Library library, FT_Int * amajor, FT_Int * aminor, FT_Int * apatch);

DECLARE_wrapper(FT_Error, FT_Done_FreeType, FT_Library library);

DECLARE_wrapper(FT_UInt, FT_Get_Char_Index, FT_Face face, FT_ULong charcode);

DECLARE_wrapper(FT_Error, FT_Set_Char_Size, FT_Face face, FT_F26Dot6 char_width, FT_F26Dot6 char_height, FT_UInt horz_resolution, FT_UInt vert_resolution);

DECLARE_wrapper(FT_Error, FT_Set_Pixel_Sizes, FT_Face  face, FT_UInt  pixel_width, FT_UInt  pixel_height);

DECLARE_wrapper(FT_Error, FT_New_Face, FT_Library library, const char * filepathname, FT_Long face_index, FT_Face * aface);

DECLARE_wrapper(FT_Error, FT_Load_Glyph, FT_Face face, FT_UInt   glyph_index, FT_Int32  load_flags);

DECLARE_wrapper(FT_Error, FT_Get_Glyph, FT_GlyphSlot slot, FT_Glyph * aglyph);

DECLARE_wrapper(FT_Error, FT_Glyph_To_Bitmap, FT_Glyph * the_glyph, FT_Render_Mode render_mode, FT_Vector * origin, FT_Bool destroy);

DECLARE_wrapper(void, FT_Done_Glyph, FT_Glyph glyph);

DECLARE_wrapper(FT_Error, FT_Done_Face, FT_Face face);

}
}
}
