#include <assert.h>

#include <memory>
#include <stdexcept>
#include <map>
#include <vector>

#include <ew/Console.hpp>

#include <ew/system/system.hpp>

#include <ew/maths/functions.hpp>

#include <ew/utils/utils.hpp>


#include <ew/graphics/font/font.hpp>

#include "../../../../common/graphics/renderer/opengl/renderer.hpp"
#include "../../../../common/graphics/renderer/opengl/libGL.hpp"
#include "../../../../common/graphics/renderer/opengl/libGLU.hpp"

// our wrappers
#include "libfreetype2.hpp"
#include "libfreetype2_wrappers.hpp"

namespace ew
{
namespace graphics
{
namespace fonts
{

using namespace ew::implementation::graphics::rendering::opengl;
using namespace ew::console;

using ew::console::cerr;
using ew::console::dbg;


// freetype variables
// TODO: use atomic test
static int freeTypeinit = 0;
static FT_Library alibrary = 0;
static FT_Int amajor;
static FT_Int aminor;
static FT_Int apatch;

EW_GRAPHICS_EXPORT bool init()
{
	if (freeTypeinit == 0) {

		load_libfreetype2_dll();

		int Ft_ret = FT_Init_FreeType(&alibrary);
		if (Ft_ret != 0) {
			cerr << "Freetype error : can't initialize lib" << "\n";
			return false;
		}

		FT_Library_Version(alibrary, &amajor, &aminor, &apatch);
		dbg << "Freetype version : " << amajor << "." << aminor << "." << apatch << "\n";
	}

	++freeTypeinit;

	return true;
}

EW_GRAPHICS_EXPORT  bool quit()
{
	if (freeTypeinit == 0) {
		return false;
	}

	--freeTypeinit;
	if (freeTypeinit == 0) {
		FT_Done_FreeType(alibrary);
		unload_libfreetype2_dll();
	}

	return true;
}


static
EW_ALWAYS_INLINE
inline int next_p2(int a)
{
	int rval = 1;
	while (rval < a)
		rval <<= 1;
	return rval;
}



class font::private_data
{
public:

	private_data()
		: last_found(0)
	{
		last_found = 0; // valgrind ?
		font_texID = 0;
		nr_subtex = 0;
		max_subtex = 0;
		subtex_W = 0;
		subtex_H = 0;
	}

	~private_data()
	{
		texture__quit();

		for (auto e : cp_font_cache) {
			delete e.ft;
			e.ft = nullptr;
		}

		//std::cerr << __PRETTY_FUNCTION__ << "this = " << this << ", aface = " << aface << "\n";
		FT_Done_Face(aface);
	}

	std::string _filename;
	FT_Face aface;
	FT_Long face_index;

	// font
	u32 _pxWidth;
	u32 _pxHeight;

	// move to texture manager
	// must recycle texture based on last time used
	// TODO: class texture
	bool   texture_init = false;
	GLuint font_texID;
	u32    font_texWidth;
	u32    font_texHeight;
	u32    nr_subtex;
	u32    max_subtex;
	u32    subtex_W;
	u32    subtex_H;

	//
	struct freetypeData {

		freetypeData()
		{
		}

		~freetypeData()
		{
			// TODO: check :  must have a valid ctx here ????


		}

		// remove

		// tex coords
		float tex_s0;
		float tex_t0;
		float tex_s1;
		float tex_t1;

		FT_Glyph ft_glyph;

		s32 _advX;
		s32 _advY;

		s32 _offsetX;
		s32 _offsetY;

		s32 _width;
		s32 _height;

		s32 _bitmap_top;
		s32 _bitmap_bottom;

		s32 _bitmap_left;
		s32 _bitmap_right;

		s32 _bitmap_width;
		s32 _bitmap_height;

		font_glyph_info  glyph_info;

	};

	// TODO: remove
	// std::map<u32,  struct freetypeData *> impl;
	//
	struct codepoint_font {
		codepoint_font()
			:
			cp((u32) -1),
			ft(nullptr)
		{
		}

		s32 cp;
		struct freetypeData * ft;
	};

	std::vector<struct codepoint_font> cp_font_cache;

	bool renderGlyphOffscreen(s32 c, font_grid * ft_grid, s32 x_pos, s32 y_pos);
	bool renderGlyph(s32 c, font_grid * ft_grid, s32 x_pos, s32 y_pos);


	struct codepoint_font * last_found;
	struct freetypeData * getFtData(s32 c);

	// helpers

	EW_ALWAYS_INLINE
	inline
	void ftdata_to_font_grid(struct freetypeData * ftdata,
				 s32 c,
				 font_grid * ft_grid,
				 s32 x_pos, s32 y_pos)
	{
		// set glyph coords
		x_pos += ftdata->glyph_info.hori_bearing_x;
		y_pos -= (ftdata->glyph_info.hori_bearing_y);

		ft_grid->v0.x = x_pos;
		ft_grid->v0.y = y_pos;
		ft_grid->v1.x = x_pos;
		ft_grid->v1.y = y_pos         + ftdata->_bitmap_height;
		ft_grid->v2.x = ft_grid->v0.x + ftdata->_bitmap_width;
		ft_grid->v2.y = ft_grid->v1.y;
		ft_grid->v3.x = ft_grid->v2.x;
		ft_grid->v3.y = y_pos;

		// tex coords
		ft_grid->v0.s = ftdata->tex_s0;
		ft_grid->v0.t = ftdata->tex_t0;
		ft_grid->v1.s = ftdata->tex_s0;
		ft_grid->v1.t = ftdata->tex_t1;
		ft_grid->v2.s = ftdata->tex_s1;
		ft_grid->v2.t = ftdata->tex_t1;
		ft_grid->v3.s = ftdata->tex_s1;
		ft_grid->v3.t = ftdata->tex_t0;

		ft_grid->v0.z = ft_grid->v1.z = ft_grid->v2.z = ft_grid->v3.z = 0.0;
	}

	bool texture__init();
	bool texture__quit();


}; // ! class font::private_data

EW_ALWAYS_INLINE
inline
struct font::private_data::freetypeData * font::private_data::getFtData(s32 c)
{

	//
	if ((last_found != 0) && last_found->cp == c) {
		return last_found->ft;
	}

	// fixme
	u64 sz = cp_font_cache.size();
	for (u64 i = 0; i < sz; i++) {
		if (cp_font_cache[i].cp == c) {
			last_found = &cp_font_cache[i];
			return cp_font_cache[i].ft;
		}
	}
	return 0;
}

// put in freetype namespace
static bool load_freetype_glyph_codepoint(FT_Face aface,
		const s32 cp,
		FT_Glyph & ft_glyph)
{
	if (cp == 0) {
		return false;
	}

	FT_UInt char_index = FT_Get_Char_Index(aface, (FT_ULong)cp);
	if (char_index == 0) {
		return false;
	}

	auto Ft_ret = FT_Load_Glyph(aface, char_index, FT_LOAD_DEFAULT);
	//auto Ft_ret = FT_Load_Glyph(aface, char_index, FT_LOAD_NO_BITMAP);
	if (Ft_ret != 0) {
		cerr << "Freetype error : can't load glyph" << "\n";
		return false;
	}

	if (FT_Get_Glyph(aface->glyph, &ft_glyph) != 0) {
		return false;
	}

	return true;
}


static bool unload_freetype_glyph_codepoint(FT_Glyph & ft_glyph)
{
	FT_Done_Glyph(ft_glyph);
	return true;
}


static bool freetype_get_glyph_info(FT_Face aface, font_glyph_info & glyph_info)
{
	// must load glyph before calling this

	// get "ew" glyph metrics
	FT_Glyph_Metrics & metrics = aface->glyph->metrics;

	glyph_info.width          = metrics.width >> 6;
	glyph_info.height         = metrics.height >> 6;
	glyph_info.hori_bearing_x = metrics.horiBearingX >> 6;
	glyph_info.hori_bearing_y = metrics.horiBearingY >> 6;
	glyph_info.hori_advance   = metrics.horiAdvance >> 6;
	glyph_info.vert_bearing_x = metrics.vertBearingX >> 6;
	glyph_info.vert_bearing_y = metrics.vertBearingY >> 6;
	glyph_info.vert_advance   = metrics.vertAdvance >> 6;

	return true;
}



bool font::get_codepoint_glyph_info(s32 codepoint, font_glyph_info & glyph_info)
{
	bool bret;
	FT_Glyph ft_glyph;

	bret = load_freetype_glyph_codepoint(this->data->aface, codepoint, ft_glyph);
	if (bret == false) {
		//std::cerr << " load_freetype_glyph_codepoint(cp="<<codepoint << ", codepoint, ft_glyph) error\n";
		return false;
	}

	bret = freetype_get_glyph_info(this->data->aface, glyph_info);
	if (bret == false) {
		assert(0);
		return false;
	}

	bret = unload_freetype_glyph_codepoint(ft_glyph);
	if (bret == false) {
		assert(0);
		return false;
	}

	return true;
}




EW_ALWAYS_INLINE
inline
bool font::private_data::renderGlyphOffscreen(s32 c, font_grid * ft_grid, s32 x_pos, s32 y_pos)
{
	if (c == 0)
		return false;

	texture__init();

	font::private_data * data = this;

	struct freetypeData * ftdata = getFtData(c);
	if (ftdata) {
		ftdata_to_font_grid(ftdata, c, ft_grid, x_pos, y_pos);
		return true;
	}

	u32 charToDisplay = (u32)c;
	{
		private_data::freetypeData * ftData  = new private_data::freetypeData;

		bool bret = load_freetype_glyph_codepoint(data->aface,
				charToDisplay,
				ftData->ft_glyph);
		if (bret == false) {
			//std::cerr << "get glyph for cp " << c << "\n";
			throw std::runtime_error("FT_Get_Glyph failed");
			return false;
		}

		// get "ew" glyph metrics
		bret = freetype_get_glyph_info(data->aface, ftData->glyph_info);
		if (bret == false) {
			unload_freetype_glyph_codepoint(ftData->ft_glyph);
			return false;
		}

		// render the glyph -> bitmap
		FT_Glyph_To_Bitmap(&(ftData->ft_glyph), FT_RENDER_MODE_NORMAL, 0, 1 /*destroy*/);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph) ftData->ft_glyph;
		FT_Bitmap & bitmap          = bitmap_glyph->bitmap;
		FT_GlyphSlot slot           = data->aface->glyph;

		ftData->_bitmap_left = slot->bitmap_left;
		ftData->_bitmap_top  = slot->bitmap_top;
		ftData->_advX = data->aface->glyph->advance.x >> 6;
		ftData->_advY = data->aface->glyph->advance.y >> 6;

		// "ew" glyph -> gl texture
		decltype(bitmap.width)  width;
		decltype(bitmap.rows) height;

		width  = ftData->_width   = max<decltype(width)>(2, next_p2(bitmap.width));
		height = ftData->_height = max<decltype(height)>(2, next_p2(bitmap.rows));

		ftData->_bitmap_width  = bitmap.width;
		ftData->_bitmap_height = bitmap.rows;

		dbg << CONSOLE_LEVEL0 << "texture W " << width << "\n" ;
		dbg << CONSOLE_LEVEL0 << "texture H " << height << "\n" ;

		// prepare texture data
		GLubyte * expanded_data = new GLubyte[ 2 * sizeof(GLubyte) * width * height ];
		memset(expanded_data, 0, 2 * width * height);

		// map bitmap.buffer -> texture
		for (decltype(height) j = 0; j < height; ++j) {
			dbg << "\n|";
			for (decltype(width) i = 0; i < width; ++i) {
				GLubyte val = (i >= bitmap.width || j >= bitmap.rows) ? 0 : bitmap.buffer[ i + bitmap.width * j ];
				expanded_data[ 2 * (i + j * width) + 0 ] = val;
				expanded_data[ 2 * (i + j * width) + 1 ] = val;

				dbg << (int)val << " ";
			}
		}

		dbg << "\n";

		// TODO: replace 16 by av computed var

		dbg << "allocating subtexture index " << data->nr_subtex << "\n";
		dbg << "data->font_texWidth " << data->font_texWidth << "\n";
		dbg << "data->font_texHeight " << data->font_texHeight << "\n";

		assert(data->nr_subtex < data->max_subtex);

		GLint xoffset = ((data->nr_subtex) % data->subtex_W) * data->subtex_W;
		GLint yoffset = ((data->nr_subtex) / data->subtex_H) * data->subtex_H;

		dbg << "char c = " << (char)c << " " << " font xoff " << xoffset << " font yoff " << yoffset << "\n";

		dbg << " bitmap_glyph->left = " <<  bitmap_glyph->left << "\n";
		dbg << " bitmap_glyph->top  = " <<  bitmap_glyph->top  << "\n";

		assert((float)xoffset < data->font_texWidth);
		assert((float)yoffset < data->font_texHeight);

		// fill user data
		ftdata_to_font_grid(ftData, c, ft_grid, x_pos, y_pos);

		// release freetype bitmap
		bret = unload_freetype_glyph_codepoint(ftData->ft_glyph);

		delete [] expanded_data;
		expanded_data = 0;
		delete ftData;
	}

	return true;
}





EW_ALWAYS_INLINE
inline
bool font::private_data::renderGlyph(s32 c, font_grid * ft_grid, s32 x_pos, s32 y_pos)
{
	if (c == 0)
		return false;

	texture__init();

	font::private_data * data = this;

	struct freetypeData * ftdata = getFtData(c);
	if (ftdata) {
		ftdata_to_font_grid(ftdata, c, ft_grid, x_pos, y_pos);
		return true;
	}

	u32 charToDisplay = (u32)c;
	{
		private_data::freetypeData * ftData  = new private_data::freetypeData;

		bool bret = load_freetype_glyph_codepoint(data->aface,
				charToDisplay,
				ftData->ft_glyph);
		if (bret == false) {
			//std::cerr << "get glyph for cp " << c << "\n";
			throw std::runtime_error("FT_Get_Glyph failed");
			return false;
		}

		// get "ew" glyph metrics
		bret = freetype_get_glyph_info(data->aface, ftData->glyph_info);
		if (bret == false) {
			unload_freetype_glyph_codepoint(ftData->ft_glyph);
			return false;
		}

		// render the glyph -> bitmap
		FT_Glyph_To_Bitmap(&(ftData->ft_glyph), FT_RENDER_MODE_NORMAL, 0, 1 /*destroy*/);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph) ftData->ft_glyph;
		FT_Bitmap & bitmap          = bitmap_glyph->bitmap;
		FT_GlyphSlot slot           = data->aface->glyph;

		ftData->_bitmap_left = slot->bitmap_left;
		ftData->_bitmap_top  = slot->bitmap_top;
		ftData->_advX = data->aface->glyph->advance.x >> 6;
		ftData->_advY = data->aface->glyph->advance.y >> 6;

		// "ew" glyph -> gl texture
		decltype(bitmap.width) width;
		decltype(bitmap.rows)  height;

		width = ftData->_width   = max<s32>(2, next_p2(bitmap.width));
		height = ftData->_height = max<s32>(2, next_p2(bitmap.rows));

		ftData->_bitmap_width = bitmap.width;
		ftData->_bitmap_height = bitmap.rows;

		dbg << CONSOLE_LEVEL0 << "texture W " << width << "\n" ;
		dbg << CONSOLE_LEVEL0 << "texture H " << height << "\n" ;

		// prepare texture data
		GLubyte * expanded_data = new GLubyte[ 2 * sizeof(GLubyte) * width * height ];
		memset(expanded_data, 0, 2 * width * height);

		// map bitmap.buffer -> texture
		for (decltype(height) j = 0; j < height; ++j) {
			dbg << "\n|";
			for (decltype(width) i = 0; i < width; ++i) {
				GLubyte val = (i >= bitmap.width || j >= bitmap.rows) ? 0 : bitmap.buffer[ i + bitmap.width * j ];
				expanded_data[ 2 * (i + j * width) + 0 ] = val;
				expanded_data[ 2 * (i + j * width) + 1 ] = val;

				dbg << (int)val << " ";
			}
		}

		dbg << "\n";

		// TODO: replace 16 by av computed var

		dbg << "allocating subtexture index " << data->nr_subtex << "\n";
		dbg << "data->font_texWidth " << data->font_texWidth << "\n";
		dbg << "data->font_texHeight " << data->font_texHeight << "\n";

		assert(data->nr_subtex < data->max_subtex);

		GLint xoffset = ((data->nr_subtex) % data->subtex_W) * data->subtex_W;
		GLint yoffset = ((data->nr_subtex) / data->subtex_H) * data->subtex_H;

		dbg << "char c = " << (char)c << " " << " font xoff " << xoffset << " font yoff " << yoffset << "\n";

		dbg << " bitmap_glyph->left = " <<  bitmap_glyph->left << "\n";
		dbg << " bitmap_glyph->top  = " <<  bitmap_glyph->top  << "\n";

		assert((float)xoffset < data->font_texWidth);
		assert((float)yoffset < data->font_texHeight);

		ftData->tex_s0 = (float)xoffset  / data->font_texWidth;
		ftData->tex_t0 = (float)yoffset  / data->font_texHeight;

		float s1 = ((float)bitmap.width) / data->font_texWidth;
		float t1 = ((float)bitmap.rows)  / data->font_texHeight;

		ftData->tex_s1 = ftData->tex_s0 + s1;
		ftData->tex_t1 = ftData->tex_t0 + t1;

		// the texture corners
		dbg << "tex_s0  = " << ftData->tex_s0 << "\n";
		dbg << "tex_t0  = " << ftData->tex_t0 << "\n";
		dbg << "tex_s1  = " << ftData->tex_s1 << "\n";
		dbg << "tex_t1  = " << ftData->tex_t1 << "\n";

		assert(ftData->tex_s0 <= 1.0f);
		assert(ftData->tex_t0 <= 1.0f);
		assert(ftData->tex_s1 <= 1.0f);
		assert(ftData->tex_t1 <= 1.0f);

		glBindTexture(GL_TEXTURE_2D, data->font_texID);
		glTexSubImage2D(GL_TEXTURE_2D, 0,
				xoffset, yoffset,
				width, height,
				GL_LUMINANCE_ALPHA,
				GL_UNSIGNED_BYTE,
				expanded_data);

		data->nr_subtex++;


		// fill cache
		struct codepoint_font cpf;
		cpf.cp = c;
		cpf.ft = ftData;

		// save struct
		u32 pos = data->cp_font_cache.size();
		data->cp_font_cache.push_back(cpf);
		// invalidate cache //  data->last_found = 0;
		data->last_found = &data->cp_font_cache[pos];

		// fill user data
		ftdata_to_font_grid(ftData, c, ft_grid, x_pos, y_pos);

		// release freetype bitmap
		bret = unload_freetype_glyph_codepoint(ftData->ft_glyph);

		delete [] expanded_data;
		expanded_data = 0;
	}

	return true;
}


//
u32 font::pixel_width()
{
	return data->_pxWidth;
}

u32 font::pixel_height()
{
	return data->_pxHeight;
}

bool font::renderGlyph(s32 c, font_grid * ft_grid, s32 x_pos, s32 y_pos)
{
	return data->renderGlyph(c, ft_grid, x_pos, y_pos);
}

void font::showCharacterInfo(s32 c)
{
	cerr << "showChararcterInfo(" << c << ")\n";
	cerr << " getCharacterAdvanceX(" << character_advance_x(c) << ");" << "\n";
	cerr << " getCharacterAdvanceY(" << character_advance_y(c) << ");" << "\n";
	cerr << " getCharacterWidth(" << character_width(c) << ");" << "\n";
	cerr << " getCharacterHeight(" << character_height(c) << ");" << "\n";
	cerr << " getCharacterBitmapWidth(" << character_bitmap_width(c) << ");" << "\n";
	cerr << " getCharacterBitmapHeight(" << character_bitmap_height(c) << ");" << "\n";
	cerr << " getCharacterBitmapTop(" << character_bitmap_top(c) << ");" << "\n";
	cerr << " getCharacterBitmapLeft(" << character_bitmap_left(c) << ");" << "\n";

	cerr << " getCharacterGlyphWidth(" << character_glyph_width(c) << ");" << "\n";
	cerr << " getCharacterGlyphHeight(" << character_glyph_height(c) << ");" << "\n";
	cerr << " getCharacterGlyphHoriBearingX(" << character_glyph_hori_bearing_x(c) << ");" << "\n";
	cerr << " getCharacterGlyphHoriBearingY(" << character_glyph_hori_bearing_y(c) << ");" << "\n";
	cerr << " getCharacterGlyphHoriAdvance(" << character_glyph_hori_advance(c) << ");" << "\n";
	cerr << " getCharacterGlyphVertBearingX(" << character_glyph_vert_bearing_x(c) << ");" << "\n";
	cerr << " getCharacterGlyphVertBearingY(" << character_glyph_vert_bearing_y(c) << ");" << "\n";
	cerr << " getCharacterGlyphVertAdvance(" << character_glyph_vert_advance(c) << ");" << "\n";
};




#define GET_UNIC_FT_DATA_MEMBER(C, D, M)                        \
    if((C) <= 0)                                            \
        return 0;                                           \
    \
    private_data::freetypeData * ftd = (D)->getFtData((C)); \
    if (!ftd)                                               \
        return 0;                                           \
    return ftd->M;


// rename into renderGlyphs
void font::renderGrid(font_grid * ft_grid, u32 nr)
{
	static bool use_vertex_array = true;

	assert(data->font_texID);
	glBindTexture(GL_TEXTURE_2D, data->font_texID);

	// direct call
	if (!use_vertex_array) {
		glBegin(GL_QUADS);
		for (u32 i = 0; i  < nr; i++) {
			glTexCoord2fv(&ft_grid[i].v0.s);
			glColor4fv(&ft_grid[i].v0.r);
			glVertex2fv(&ft_grid[i].v0.x);

			glTexCoord2fv(&ft_grid[i].v1.s);
			glColor4fv(&ft_grid[i].v1.r);
			glVertex2fv(&ft_grid[i].v1.x);


			glTexCoord2fv(&ft_grid[i].v2.s);
			glColor4fv(&ft_grid[i].v2.r);
			glVertex2fv(&ft_grid[i].v2.x);

			glTexCoord2fv(&ft_grid[i].v3.s);
			glColor4fv(&ft_grid[i].v3.r);
			glVertex2fv(&ft_grid[i].v3.x);
		}
		glEnd();
	}

	/*
	  Specifies the type of array to enable.
	  Symbolic constants
	  GL_V2F,
	  GL_V3F,
	  GL_C4UB_V2F,
	  GL_C4UB_V3F,
	  GL_C3F_V3F,
	  GL_N3F_V3F,
	  GL_C4F_N3F_V3F,
	  GL_T2F_V3F,
	  GL_T4F_V4F,
	  GL_T2F_C4UB_V3F,
	  GL_T2F_C3F_V3F,
	  GL_T2F_N3F_V3F,
	  GL_T2F_C4F_N3F_V3F,
	  GL_T4F_C4F_N3F_V4F
	*/

	// vertex array
	if (use_vertex_array) {
		glInterleavedArrays(GL_T2F_C4F_N3F_V3F, 0, ft_grid);
		glDrawArrays(GL_QUADS, 0, nr * 4);
	}
}

void font::renderCharacter(s32 c, s32 x_pos, s32 y_pos)
{

}


// TODO: return s32 (-1) on error ?
u32 font::get_character_texture_id(s32 c)
{
	return data->font_texID;
}

s32 font::character_bitmap_width(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, _bitmap_width);
}

s32 font::character_bitmap_height(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, _bitmap_height);
}

s32 font::character_bitmap_left(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, _bitmap_left);
}

s32 font::character_bitmap_top(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, _bitmap_top);
}

s32 font::character_width(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, _width);
}

s32 font::character_height(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, _height);
}

s32 font::character_advance_x(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, _advX);
}

s32 font::character_advance_y(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, _advY);
}

s32 font::character_glyph_width(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, glyph_info.width);
}

s32 font::character_glyph_height(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, glyph_info.height);
}

s32 font::character_glyph_hori_bearing_x(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, glyph_info.hori_bearing_x);
}

s32 font::character_glyph_hori_bearing_y(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, glyph_info.hori_bearing_y);
}

s32 font::character_glyph_hori_advance(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, glyph_info.hori_advance);
}

s32 font::character_glyph_vert_bearing_x(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, glyph_info.vert_bearing_x);
}

s32 font::character_glyph_vert_bearing_y(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, glyph_info.vert_bearing_y);
}

s32 font::character_glyph_vert_advance(s32 c)
{
	GET_UNIC_FT_DATA_MEMBER(c, data, glyph_info.vert_advance);
}

font::font(const char * filename,  u32 pxWidth, u32 pxHeight)
	:
	data(new private_data)
{
	//std::cerr << __PRETTY_FUNCTION__ << " this = " << this << " " << filename <<  " " << pxWidth << " " << pxHeight << "\n";

	data->_filename = filename;
	data->_pxWidth  = pxWidth;
	data->_pxHeight = pxHeight;

	if (data->_pxWidth == 0)
		data->_pxWidth = data->_pxHeight;

	if (data->_pxHeight == 0)
		data->_pxHeight = data->_pxWidth;

	// throw badRange ?
	if (data->_pxWidth == 0) {
		data->_pxWidth = 10;
		data->_pxHeight = 10;
	}

	data->aface = 0;
	data->face_index = 0;
}

bool font::font::private_data::texture__init()
{
	auto data = this;

	if (data->texture_init == false) {
		// alloc a single texture for ~ 256 glyph
		glGenTextures(1, &(data->font_texID));
		// if (data->font_texID == 0) {
		//   return false;
		// }

		glBindTexture(GL_TEXTURE_2D, data->font_texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// reserve pixel

		static int NR_SUBTEX_PER_LINE = 16;

		// data->font_texHeight = next_p2(data->_pxHeight) * NR_SUBTEX_PER_LINE;
		// data->font_texWidth  = data->font_texHeight;

		data->font_texHeight = 1024; // / Font_Texheight;
		data->font_texWidth  = 1024; // / Font_Texheight;

		data->subtex_W = data->subtex_H = next_p2(data->_pxHeight);

		NR_SUBTEX_PER_LINE = data->font_texHeight / data->subtex_H;

		//std::cerr << "NR_SUBTEX_PER_LINE  " << NR_SUBTEX_PER_LINE << "\n";
		//std::cerr << "font_texID = " << data->font_texID << "\n";
		//std::cerr << "font tex width  " << data->font_texWidth  << "\n";
		//std::cerr << "font tex height " << data->font_texHeight << "\n";
		//std::cerr << "font subtex width  " << data->subtex_W << "\n";
		//std::cerr << "font subtex height " << data->subtex_H << "\n";


		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data->font_texWidth, data->font_texHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 0);

		data->max_subtex = NR_SUBTEX_PER_LINE * NR_SUBTEX_PER_LINE;
		data->nr_subtex = 0;

		data->texture_init = true;
	}
	return true;
}

bool font::font::private_data::texture__quit()
{
	auto d = this;

	if (d->texture_init == false) {
		return false;
	}

	glDeleteTextures(1, &(d->font_texID));

	return true;
}

bool font::open()
{
	int Ft_ret;

	Ft_ret = FT_New_Face(alibrary, (const char *) data->_filename.c_str(),
			     data->face_index,
			     &(data->aface));
	if (Ft_ret != 0) {
		// cerr << "Freetype error : can't load Font '" << filename << "'\n";
		return false;
	}

	// cerr << "Freetype : " << "'" << data->aface->family_name << "' font loaded" << "\n";

	if (data->aface->num_fixed_sizes == 0) {
		// cerr << "Freetype : this font is scalable\n";

	} else {

		// cerr << "Freetype : this font is not scalable\n";

	}

	// int font_dpi = 96;     /*  */
	// Ft_ret = FT_Set_Char_Size(data->aface, data->_pxHeight << 6, data->_pxHeight << 6, font_dpi, font_dpi);
	// if(Ft_ret != 0) {
	//   cerr << "Freetype error : can't set char size" << "\n";
	//   return;
	// }

	Ft_ret  = FT_Set_Pixel_Sizes(
			  data->aface,        /* handle to face object */
			  data->_pxWidth,     /* pixel_width           */
			  data->_pxHeight);   /* pixel_height          */
	if (Ft_ret != 0) {
		cerr << "Freetype error : can't set char size" << "\n";
		return false;
	}

	if (Ft_ret != 0) {
		cerr << "Freetype error : can't set char size" << "\n";
		return false;
	}

	return true;
}


//


/*
  this function prints an array of unicode codepoints
*/


static inline bool layout_codepoint(s32 cp,
				    float x_pos, float y_pos,
				    float r, float g, float b,
				    u32 xadv,
				    u32 width,
				    float * x_next,
				    ew::graphics::fonts::font & ft,
				    fonts::font_grid * pftg)
{
	s32 vc = cp;

	if (vc) {

		if ((x_pos + xadv) > width) {
			//cerr << "x_pos = " << x_pos << ", xadv = " << xadv << "\n";
			return false;
		}

		pftg->v0.r = r;
		pftg->v1.r = r;
		pftg->v2.r = r;
		pftg->v3.r = r;

		pftg->v0.g = g;
		pftg->v1.g = g;
		pftg->v2.g = g;
		pftg->v3.g = g;

		pftg->v0.b = b;
		pftg->v1.b = b;
		pftg->v2.b = b;
		pftg->v3.b = b;

		ft.renderGlyph(vc, pftg, x_pos, y_pos); // get glyph metrics

		if (pftg->v0.x == pftg->v2.x) {

			pftg->v0.x = x_pos; //  - ft.getCharacterGlyphHoriBearingX(vc);
			pftg->v2.x = pftg->v0.x + ft.character_glyph_hori_advance(' ');
		}

		// to check rendering error : return false;
		*x_next = x_pos + xadv;

	} else {

		*x_next = x_pos + ft.character_glyph_hori_advance(' ');
	}

	return true;
}


bool font_print(ew::graphics::fonts::font & ft,
		s32 x, s32 y, u32 width, u32 height,
		ew::graphics::colors::color3ub color,
		s32 * cp_array, size_t nr_cp)
{
	fonts::font_grid * pftg = nullptr;
	fonts::font_grid pftg_temp;
	// todo: check

	float r = color.red()   / 255.0f;
	float g = color.green() / 255.0f;
	float b = color.blue()  / 255.0f;

	y += ft.pixel_height();

	ew_debug_glGetError("", __FUNCTION__, __LINE__);


	pftg = new fonts::font_grid[nr_cp];
	size_t idx = 0;
	for (idx = 0; idx < nr_cp; ++idx) {

		float x_next;

		s32 tmp_x = 0;
		s32 tmp_y = 0;

		s32 vc = cp_array[idx];

		ft.renderGlyph(vc, &pftg_temp, tmp_x, tmp_y); // get glyph metrics

		u32 xadv = ft.character_glyph_hori_advance(vc);

		bool res = layout_codepoint(vc,
					    x, y,
					    r, g, b,
					    xadv,
					    width,
					    &x_next,
					    ft,
					    &pftg[idx]);

		if (res == false) {
			break;
		}

		x = x_next;
	}



	/*
	  GL_BLEND                0x0BE2
	  GL_BLEND_SRC                0x0BE1
	  GL_BLEND_DST                0x0BE0
	  GL_ZERO                 0x0
	  GL_ONE                  0x1
	  GL_SRC_COLOR                0x0300
	  GL_ONE_MINUS_SRC_COLOR          0x0301
	  GL_SRC_ALPHA                0x0302
	  GL_ONE_MINUS_SRC_ALPHA          0x0303
	  GL_DST_ALPHA                0x0304
	  GL_ONE_MINUS_DST_ALPHA          0x0305
	  GL_DST_COLOR                0x0306
	  GL_ONE_MINUS_DST_COLOR          0x0307
	  GL_SRC_ALPHA_SATURATE           0x0308
	*/
	glDisable(GL_DEPTH_TEST);
	ew_debug_glGetError("", __FUNCTION__, __LINE__);

	glDisable(GL_LIGHTING);
	ew_debug_glGetError("", __FUNCTION__, __LINE__);

	glEnable(GL_TEXTURE_2D);
	ew_debug_glGetError("", __FUNCTION__, __LINE__);

	glEnable(GL_BLEND);
	ew_debug_glGetError("", __FUNCTION__, __LINE__);

#if 1
	glDisable(GL_DEPTH_TEST);
	ew_debug_glGetError("", __FUNCTION__, __LINE__);


	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // ok
	// glBlendFunc(GL_SRC_COLOR,  GL_DST_COLOR); //

	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//ew_debug_glGetError("", __FUNCTION__, __LINE__);
#endif

	ft.renderGrid(pftg, idx);
	ew_debug_glGetError("", __FUNCTION__, __LINE__);


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	delete [] pftg;

	return true;
}



bool font_print(ew::graphics::fonts::font & ft,
		s32 x, s32 y, u32 width, u32 height,
		ew::graphics::colors::color3ub col,
		char * str)
{
	u32 len = strlen(str);

	s32 * cp = new s32[len + 1];

	for (u32 i = 0; i < len; ++i)
		cp[i] = str[i];

	bool ret = font_print(ft,
			      x, y, width, height, col,
			      cp, len);

	delete [] cp;

	return ret;
}



// -----------------------------------

font::~font()
{
	//std::cerr << __PRETTY_FUNCTION__ << " this = " << this << "\n";
	// unload all textures
	//delete data;
	//data = 0;
}

// -----------------------------------

}
}
}

