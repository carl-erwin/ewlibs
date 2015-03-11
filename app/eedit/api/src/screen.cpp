#include <assert.h>

#include <vector>
#include <list>
#include <string>
#include "ew/graphics/font/font.hpp"

#include "core/log.hpp"

#include "../../api/include/codepoint_info.h"
// #include "../../api/include/screen_line.h"  // TODO
#include "../../api/include/screen.h"


int screen_dimension_reset(screen_dimension_t * sd)
{
	sd->l = 0;
	sd->c = 0;
	sd->w = 0;
	sd->h = 0;

	return 0;
}



struct screen_line_s {
	std::vector<codepoint_info_s> m_cpinfo_array;
	int m_used;

};

struct screen_s {
	uint32_t m_max_l  = 0; // max number of lines
	uint32_t m_max_c  = 0; // max number of columns
	uint32_t m_used_l = 0; // number of lines to display
	uint32_t m_max_width_px  = 0;
	uint32_t m_max_height_px = 0;
	uint64_t m_buffer_size = 0;
	uint64_t start_offset  = 0;
	uint64_t m_id = 0;
	int internal_index = -1;
	struct codepoint_info_s first_cpinfo;
	struct codepoint_info_s last_cpinfo;
	int _ready = 0; // only when we can render // add get/set

	std::vector<struct screen_line_s> m_line_array;
};

// TODO reset / helpers

screen_t * get_previous_screen_by_id(uint64_t screen_id);


int screen_line_copy(screen_line_t * dst, const screen_line_t * orig)
{
	if (dst != orig) {
		dst->m_cpinfo_array = orig->m_cpinfo_array;
		dst->m_used         = orig->m_used;
	} else {
		assert(0);
	}

	return 0;
}


int screen_line_alloc(screen_line_t ** l, size_t nr_cp)
{
	*l = new screen_line_t;
	screen_line_resize(*l, nr_cp);

	for (auto & e : (*l)->m_cpinfo_array)
		codepoint_info_reset(&e);

	return 0;
}

int screen_line_release(screen_line_t * l)
{
	l->m_used = 0;
	delete l;
	return 0;
}


int screen_line_resize(screen_line_t * l, size_t nr_cp)
{
	l->m_cpinfo_array.resize(nr_cp);

	for (auto & e : l->m_cpinfo_array) {
		codepoint_info_reset(&e);
	}


	l->m_used = 0;
	return 0;
}

int screen_line_get_cpinfo(const screen_line_t * l, uint32_t column, const codepoint_info_s ** cpi, screen_line_hints h /* no_fix */)
{
	auto sz = l->m_cpinfo_array.size();
	assert(sz);
	if ((column >= sz) && !(h & screen_line_hint_fix_column_overflow)) {
		assert(0);
		return 0;
	}

	column = std::min<uint32_t>(column , (sz-1));

	*cpi = &l->m_cpinfo_array[column];
	return 1;
}

int screen_line_get_first_cpinfo(const screen_line_t * l, const codepoint_info_s ** cpi, size_t * column_index)
{
	*column_index = 0;
	// todo check size
	*cpi = &l->m_cpinfo_array[*column_index];
	return 1;
}

int  screen_line_get_last_cpinfo(const screen_line_t * l, const codepoint_info_s ** cpi, size_t * column_index)
{
	*column_index = screen_line_get_number_of_used_columns(l);
	assert(column_index);
	--*column_index;

	*cpi = &l->m_cpinfo_array[*column_index];
	return 1;
}

size_t  screen_line_capacity(const screen_line_t * l)
{
	return l->m_cpinfo_array.size();
}

size_t  screen_line_get_number_of_used_columns(const screen_line_t * l)
{
	return l->m_used;
}

void  screen_line_set_number_of_used_columns(screen_line_t * l, size_t nr)
{
	assert(nr <= screen_line_capacity(l));
	l->m_used = nr;
}

uint64_t  screen_line_first_offset(screen_line_t * l)
{
	const codepoint_info_s * cpi = nullptr;

	size_t column_index;
	screen_line_get_first_cpinfo(l, &cpi, &column_index);
	assert(cpi->size);
	return cpi->offset;
}

uint64_t  screen_line_last_offset(const screen_line_t * l)
{
	const codepoint_info_s * cpi = nullptr;
	size_t column_index;
	screen_line_get_last_cpinfo(l, &cpi, &column_index);
	assert(cpi->size);
	return cpi->offset + cpi->size;
}


//////////////////////////////////


int screen_copy(screen_t * dst, const screen_t * orig);

uint64_t screen_get_buffer_size(const screen_t * scr)
{
	return scr->m_buffer_size;
}

// private
void screen_set_buffer_size(screen_t * scr, uint64_t sz)
{
	scr->m_buffer_size = sz;
}

void screen_dump(const screen_t * scr, const char * by)
{
	return;
	app_log << __PRETTY_FUNCTION__ << " by " << by << "\n";
	app_log << " screen m_max_l = " << scr->m_max_l << "\n";
	app_log << " screen m_max_c = " << scr->m_max_c << "\n";
	app_log << " screen m_max_width_px = " << scr->m_max_width_px << "\n";
	app_log << " screen m_max_height_px = " << scr->m_max_height_px << "\n";
	app_log << " screen m_line_array.size() = " << scr->m_line_array.size() << "\n";
	app_log << " screen number_of_used_lines() = " << screen_get_number_of_used_lines(scr) << "\n";

	assert(screen_get_number_of_used_lines(scr) <= scr->m_line_array.size());
}


int screen_alloc(screen_t ** scr, const char * called_by, uint32_t l, uint32_t c, uint32_t w, uint32_t h)
{
	auto s = new screen_t;
	assert(l);
	assert(c);
	s->m_max_width_px  = w;
	s->m_max_height_px = h;
	screen_resize(s, l, c);

	codepoint_info_reset(&s->first_cpinfo);
	codepoint_info_reset(&s->last_cpinfo);

	*scr = s;
	return 0;
}

int screen_release(screen_t * scr)
{
	if (scr) {
		delete scr;
	}

	return 0;
}

int screen_alloc_with_dimension(screen_t ** scr, const char * called_by, const screen_dimension_t * dim)
{
	return screen_alloc(scr, called_by, dim->l, dim->c, dim->w, dim->h);
}


screen_dimension_t screen_get_dimension(const screen_t * scr)
{
	screen_dimension_t d = {
		scr->m_max_l,
		scr->m_max_c,
		scr->m_max_width_px,
		scr->m_max_height_px,
	};
	return d;
}


// internal
int screen_copy(screen_t * dst, const screen_t * orig)
{
	dst->m_max_width_px  = orig->m_max_width_px;
	dst->m_max_height_px = orig->m_max_height_px;
	dst->m_max_l         = orig->m_max_l; // max number of lines
	dst->m_max_c         = orig->m_max_c; // max number of columns
	dst->m_used_l        = orig->m_used_l; // number of lines to display
	dst->m_max_width_px  = orig->m_max_width_px;
	dst->m_max_height_px = orig->m_max_height_px;
	dst->m_buffer_size   = orig->m_buffer_size;
	dst->m_line_array    = orig->m_line_array;
	dst->first_cpinfo    = orig->first_cpinfo;
	dst->last_cpinfo     = orig->last_cpinfo;

	return 0;
}

screen_t * screen_clone(screen_t * scr)
{
	auto dst = new screen_t;
	screen_copy(dst, scr);
	return dst;
}

int screen_resize(screen_t * scr, uint32_t l, uint32_t c)
{
	scr->m_line_array.resize(l);

	assert(scr->m_line_array.size() >= l);

	// reserve max columns for each reserved line
	for (auto & ln : scr->m_line_array) {
		screen_line_resize(&ln, c);
		screen_line_set_number_of_used_columns(&ln, 0);
	}

	codepoint_info_reset(&scr->first_cpinfo);
	codepoint_info_reset(&scr->last_cpinfo);

	// set limits
	screen_set_max_number_of_lines(scr, l); // return m_line_array.capacity() ?
	screen_set_max_number_of_columns(scr, c);
	screen_set_number_of_used_lines(scr, 0);
	// dump();

	return 0;
}

void screen_reset_lines(screen_t * scr)
{
	for (auto & l : scr->m_line_array) {
		screen_line_set_number_of_used_columns(&l, 0);
	}

	screen_set_number_of_used_lines(scr, 0);
}

void screen_reset(screen_t * scr)
{
	screen_reset_lines(scr);
	codepoint_info_reset(&scr->first_cpinfo);
	codepoint_info_reset(&scr->last_cpinfo);
}

void screen_set_max_number_of_lines(screen_t * scr, uint32_t max)
{
	scr->m_max_l = max;
}

uint32_t  screen_get_max_number_of_lines(screen_t * scr)
{
	return scr->m_max_l;
}

//
uint32_t screen_get_max_number_of_columns(screen_t * scr)
{
	return scr->m_max_c;
}

void screen_set_max_number_of_columns(screen_t * scr, uint32_t max)
{
	scr->m_max_c = max;
}

void screen_get_max_line_and_column(screen_t * scr, uint32_t * max_l, uint32_t * max_c)
{
	*max_l = scr->m_max_l;
	*max_c = scr->m_max_c;
}


uint32_t screen_get_max_width_px(screen_t * scr)
{
	return scr->m_max_width_px;
}

void screen_set_max_width_px(screen_t * scr, uint32_t max)
{
	scr->m_max_width_px = max;
}

uint32_t screen_get_max_height_px(screen_t * scr)
{
	return scr->m_max_height_px;
}

void screen_set_max_height_px(screen_t * scr, uint32_t max)
{
	scr->m_max_height_px = max;
}

void screen_get_max_width_and_height(screen_t * scr, uint32_t * max_w, uint32_t * max_h)
{
	*max_w = scr->m_max_width_px;
	*max_h = scr->m_max_height_px;
}


//
void  screen_set_number_of_used_lines(screen_t * scr, uint32_t max)
{
	scr->m_used_l = max;
}

uint32_t  screen_get_number_of_used_lines(const screen_t * scr)
{
	return scr->m_used_l;
}
//

int screen_get_line(const screen_t * scr, uint32_t line_index, const screen_line_t ** l)
{
	auto sz = scr->m_line_array.size();
	if (line_index >= sz) {
		assert(0);
		return 0;
	}

	*l = &scr->m_line_array[line_index];
	return 1;
}

int screen_get_last_line(const screen_t * scr, const screen_line_t ** l, size_t * index)
{
	*index = screen_get_number_of_used_lines(scr);
	if (*index)
		--(*index);

	*l = &scr->m_line_array[*index];
	return 1;
}


int screen_get_first_cpinfo(screen_t * scr, const codepoint_info_s ** cpi)
{
	*cpi = &scr->first_cpinfo;
	return 0;
}

int screen_get_last_cpinfo(screen_t * scr, const codepoint_info_s ** cpi)
{
	*cpi = &scr->last_cpinfo;
	return 0;
}

int screen_get_first_and_last_cpinfo(screen_t * scr,
				     const codepoint_info_s ** fcpi,
				     const codepoint_info_s ** lcpi)
{
	*fcpi = &scr->first_cpinfo;
	*lcpi = &scr->last_cpinfo;
	return 0;
}


int screen_set_start_offset(screen_t * scr, uint64_t start_offset)
{
	scr->start_offset = start_offset;
	return 0;
}


int screen_get_codepoint_by_coords(screen_t * scr, int32_t x, int32_t y, const struct codepoint_info_s ** out_cpi)
{
	app_log << __PRETTY_FUNCTION__ << " : x = " << x << " y " << y << "\n";

	*out_cpi = nullptr;

	for (uint32_t scr_line_index = 0; scr_line_index < screen_get_number_of_used_lines(scr); ++scr_line_index) {

		auto l = &scr->m_line_array[scr_line_index];
		const codepoint_info_s * cpi = nullptr;

		size_t first_col_index;
		screen_line_get_first_cpinfo(l, &cpi, &first_col_index);
		if (y > cpi->y) {
			continue;
		}

		size_t last_col_index;
		const codepoint_info_s * last_cpi = nullptr;
		screen_line_get_last_cpinfo(l, &last_cpi, &last_col_index);

		for (uint32_t scr_col_index = last_col_index + 1; scr_col_index > 0; --scr_col_index) {
			screen_line_get_cpinfo(l, scr_col_index - 1, &cpi, screen_line_hint_no_column_fix);
			if (x >= cpi->x) {
				*out_cpi = cpi;
				return 1;
			}
		}
		return 0;
	}

	return 0;
}


int     screen_contains_offset(const screen_t * scr, const uint64_t offset)
{
	static int debug = 0;

	if (scr->first_cpinfo.used == 0) {
		if (debug)
			app_log << __PRETTY_FUNCTION__ << " first_cpinfo == nullptr\n";
		assert(0);
		return 0;
	}

	if (scr->last_cpinfo.used == 0) {
		if (debug)
			app_log << __PRETTY_FUNCTION__ << " last_cpinfo == nullptr\n";
		assert(0);
		return 0;
	}

	if (debug) {
		app_log << __PRETTY_FUNCTION__
			<< " : offset("<<offset<<") , "
			<< " first_cpinfo->offset("<< scr->first_cpinfo.offset <<"), "
			<< " last_cpinfo->offset ("<< scr->last_cpinfo.offset <<")\n";
	}

	if (offset > scr->last_cpinfo.offset) {
		if (debug)
			app_log << __PRETTY_FUNCTION__ << " : offset("<<offset<<") > "<< "last_cpinfo.offset ("<<scr->last_cpinfo.offset<<")\n";
		return 0;
	}

	if (offset < scr->first_cpinfo.offset) {
		if (debug)
			app_log << __PRETTY_FUNCTION__ << " : offset("<<offset<<") < "<< "first_cpinfo.offset ("<<scr->first_cpinfo.offset<<") \n";
		return 0;
	}

	app_log << __PRETTY_FUNCTION__ << " found : offset("<<offset<<") on screen\n";

	return 1;
}



int screen_get_line_by_offset(const screen_t * scr, const uint64_t offset, const screen_line_t ** l, size_t * scr_line_index, size_t * scr_col_index)
{
	static int debug = 0;

	*l = nullptr;

	if (!screen_contains_offset(scr, offset)) {
		return 0;
	}

	if (debug) {
		// for each ? dichotomy
		app_log << __PRETTY_FUNCTION__
			<< " : offset("<<offset<<") , "
			<< " number_of_used_lines() " << screen_get_number_of_used_lines(scr) << "\n";
	}

	for (*scr_line_index = 0; *scr_line_index < (size_t)screen_get_number_of_used_lines(scr); ++(*scr_line_index)) {

		if (debug) {
			app_log << __PRETTY_FUNCTION__ << " : CHECKING line_index("<<*scr_line_index<<") / " << (size_t)screen_get_number_of_used_lines(scr) << "\n";
		}

		// screen_get_line(scr, *scr_line_index, &l, no_bundary_check);
		*l = &scr->m_line_array[*scr_line_index];
		const codepoint_info_s * cpi = nullptr;

		size_t last_col_index;
		screen_line_get_last_cpinfo((*l), &cpi, &last_col_index);

		if (debug) {
			app_log << __PRETTY_FUNCTION__ << " : scr_line_index("<<*scr_line_index<<") , " << " cpi->offset() " <<  cpi->offset << "\n";
		}

		if (offset > cpi->offset) { // this line ?
			if (debug) {
				app_log << __PRETTY_FUNCTION__ << " offset("<<offset<<") > line_index("<<*scr_line_index<<").last_cpi->offset("<< cpi->offset << ")\n";
			}
			continue;
		}

		if (debug) {
			app_log << __PRETTY_FUNCTION__ << " offset("<<offset<<") <= line_index("<<*scr_line_index<<").last_cpi->offset("<< cpi->offset << ")\n";
		}

		if (debug) {
			app_log << __PRETTY_FUNCTION__
				<< " : offset("<<offset<<") , "
				<< " last_col_index " << last_col_index << "\n";
		}

		for (*scr_col_index = 0; *scr_col_index < (last_col_index + 1); ++(*scr_col_index)) {
			screen_line_get_cpinfo(*l, *scr_col_index, &cpi, screen_line_hint_no_column_fix);

			if (debug) {
				app_log << __PRETTY_FUNCTION__
					<< " : scr_col_index("<<scr_col_index<<") , "
					<< " cpi->offset() " <<  cpi->offset << "\n";
			}

			if (cpi->offset == offset) {

				if (debug) {
					app_log << __PRETTY_FUNCTION__ << " : found offset at column_index " << *scr_col_index << "\n";
				}
				return 1;
			}
		}

	}

	*l = nullptr;

	if (debug) {
		app_log << __PRETTY_FUNCTION__
			<< " : offset("<<offset<<") , "
			<< " not found after lookup\n";
	}

	return 0;
}


int screen_get_codepoint_info(const screen_t * scr,
			      uint32_t line, uint32_t column,
			      const screen_line_t ** l_,
			      const codepoint_info_s ** cpi,
			      screen_line_hints h)
{
	*cpi            = nullptr;
	*l_             = nullptr;
	const screen_line_t * l = nullptr;

	int bret = screen_get_line(scr, line, &l);
	if (bret == 0) {
		assert(0);
		return 0;
	}

	*l_             = l;
	bret = screen_line_get_cpinfo(l, column, cpi, h);
	return bret;
}

int screen_get_line_by_coords(const screen_t * scr, uint32_t line, uint32_t column, const screen_line_t ** l_, const codepoint_info_s ** cpi)
{
	return screen_get_codepoint_info(scr, line, column, l_, cpi, screen_line_hint_no_column_fix);
}


int screen_put_cp(screen_t * scr, const int32_t real_cp, const int32_t vcp, const uint32_t line, const uint32_t column, const screen_line_t ** l, const codepoint_info_s ** cpi)
{
	int bret = screen_get_codepoint_info(scr, line, column, l, cpi, screen_line_hint_no_column_fix);
	if (bret == 0) {
		assert(0);
		return 0;
	}

	uint32_t old_used = screen_line_get_number_of_used_columns(*l);

	screen_line_t * rw_l = const_cast<screen_line_t *>(*l);
	screen_line_set_number_of_used_columns(rw_l, std::max<uint32_t>(column + 1, old_used));

	codepoint_info_s * rw_cpi = const_cast<codepoint_info_s*>(*cpi);
	rw_cpi->codepoint = vcp;
	rw_cpi->real_codepoint = real_cp;
	rw_cpi->used = 1;

	old_used = screen_get_number_of_used_lines(scr);
	screen_set_number_of_used_lines(scr, std::max<uint32_t>(line + 1, old_used));

	return 1;
}

void screen_set_ready_flag(screen_t * scr, int flag)
{
	scr->_ready = flag;
}

int  screen_get_ready_flag(const screen_t * scr)
{

	if (!scr)
		return 0;

	return scr->_ready;
}



uint64_t screen_get_start_offset(screen_t * scr)
{
	return scr->first_cpinfo.offset;
}
