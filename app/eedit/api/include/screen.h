#pragma once

/*
    the screen is a 2D array of screen_lines
 */


#include <assert.h>

#include "editor_export.h"
#include "codepoint_info.h"

#ifdef __cplusplus
extern "C" {
#endif

enum screen_line_hints {
    screen_line_hint_no_column_fix,
    screen_line_hint_fix_column_overflow = 1
};


struct screen_dimension_t {
    uint32_t l;
    uint32_t c;
    uint32_t w;
    uint32_t h;
};

typedef struct screen_line_s screen_line_t;

typedef struct screen_s      screen_t;


EDITOR_EXPORT
int screen_dimension_reset(screen_dimension_t * sd); // inline memset()


/* screen line */
EDITOR_EXPORT
int                screen_line_alloc(screen_line_t ** l, size_t nr_cp);

EDITOR_EXPORT
int                screen_line_release(screen_line_t * l);

EDITOR_EXPORT
int                screen_line_copy(screen_line_t * dst, const screen_line_t * orig);

EDITOR_EXPORT
int                screen_line_resize(screen_line_t * l, size_t nr_cp);

EDITOR_EXPORT
int                screen_line_get_cpinfo(const screen_line_t * l, uint32_t column, const codepoint_info_s ** cpi, screen_line_hints h /* no_fix */);

EDITOR_EXPORT
int                screen_line_get_first_cpinfo(const screen_line_t * l, const codepoint_info_s ** cpi, size_t * column_index);

EDITOR_EXPORT
int                screen_line_get_last_cpinfo(const screen_line_t * l, const codepoint_info_s ** cpi, size_t * column_index);

EDITOR_EXPORT
size_t             screen_line_capacity(const screen_t * scr);

EDITOR_EXPORT
size_t             screen_line_get_number_of_used_columns(const screen_line_t * l);

EDITOR_EXPORT
void               screen_line_set_number_of_used_columns(screen_line_t * l, size_t nr);

EDITOR_EXPORT
uint64_t           screen_line_first_offset(const screen_line_t * l);

EDITOR_EXPORT
uint64_t           screen_line_last_offset(const screen_line_t * l);

EDITOR_EXPORT
uint64_t           screen_get_buffer_size(const screen_t * scr);

/* */
EDITOR_EXPORT
int                screen_contains_offset(const screen_t * scr, const uint64_t offset);

EDITOR_EXPORT
screen_t *         get_previous_screen_by_id(uint64_t screen_id); // HERE ?

EDITOR_EXPORT
int                screen_set_start_offset(screen_t * scr, uint64_t start_offset);

EDITOR_EXPORT
int                screen_get_first_cpinfo(screen_t * scr, const codepoint_info_s ** cpi);

EDITOR_EXPORT
int                screen_get_last_cpinfo(screen_t * scr, const codepoint_info_s ** cpi);

EDITOR_EXPORT
int                screen_get_first_and_last_cpinfo(screen_t * scr, const codepoint_info_s ** first_cpi, const codepoint_info_s ** last_cpi);

/* allocator */
EDITOR_EXPORT
void               screen_dump(const screen_t * scr, const char * by); // FIXME: allow external dump with iterator cb ?

EDITOR_EXPORT
int                screen_alloc(screen_t ** scr, const char * called_by, uint32_t l, uint32_t c, uint32_t w, uint32_t h);

EDITOR_EXPORT
int                screen_release(screen_t * scr);

EDITOR_EXPORT
int                screen_copy(screen_t * dst, const screen_t * orig);

EDITOR_EXPORT
screen_t *         screen_clone(screen_t * scr);

EDITOR_EXPORT
uint64_t           screen_get_start_offset(screen_t * scr);

EDITOR_EXPORT
int                screen_alloc_with_dimension(screen_t ** scr, const char * called_by, const screen_dimension_t * dim);

EDITOR_EXPORT
screen_dimension_t screen_get_dimension(const screen_t * scr);

// internal


EDITOR_EXPORT
void               screen_set_buffer_size(screen_t * scr, uint64_t sz);

EDITOR_EXPORT
int                screen_resize(screen_t * scr, uint32_t l, uint32_t c);

EDITOR_EXPORT
void               screen_reset_lines(screen_t * scr);

EDITOR_EXPORT
void               screen_reset(screen_t * scr);

EDITOR_EXPORT
void               screen_set_max_number_of_lines(screen_t * scr, uint32_t max);

EDITOR_EXPORT
uint32_t           screen_get_max_number_of_lines(screen_t * scr);

EDITOR_EXPORT
void               screen_set_max_number_of_columns(screen_t * scr, uint32_t max);

EDITOR_EXPORT
uint32_t           screen_get_max_number_of_columns(screen_t * scr);

EDITOR_EXPORT
void               screen_get_max_line_and_column(screen_t * scr, uint32_t * max_l, uint32_t * max_c);

EDITOR_EXPORT
uint32_t           screen_get_max_width_px(screen_t * scr);

EDITOR_EXPORT
void               screen_set_max_width_px(screen_t * scr, uint32_t max);

EDITOR_EXPORT
uint32_t           screen_get_max_height_px(screen_t * scr);

EDITOR_EXPORT
void               screen_set_number_of_used_lines(screen_t * scr, uint32_t max);

EDITOR_EXPORT
uint32_t           screen_get_number_of_used_lines(const screen_t * scr);

EDITOR_EXPORT
int                screen_get_line(const screen_t * scr, uint32_t line_index, const screen_line_t ** l);

EDITOR_EXPORT
int                screen_get_last_line(const screen_t * scr, const screen_line_t ** l, size_t * index);

EDITOR_EXPORT
int                screen_get_codepoint_by_coords(int32_t x, int32_t y, struct codepoint_info_s ** out_cpi);

EDITOR_EXPORT
int                screen_get_line_by_offset(const screen_t * scr, const uint64_t offset, const screen_line_t ** l, size_t * scr_line_index, size_t * scr_col_index);

EDITOR_EXPORT
int                screen_get_codepoint_info(const screen_t * scr, uint32_t line, uint32_t column, const screen_line_t ** l_, const codepoint_info_s ** cpi, screen_line_hints h);

EDITOR_EXPORT
int                screen_get_line_by_coords(screen_t * scr, uint32_t line, uint32_t column, const screen_line_t ** l_, const codepoint_info_s ** cpi);

EDITOR_EXPORT
int                screen_put_cp(screen_t * scr, const int32_t real_cp, const int32_t vcp, const uint32_t line, const uint32_t column, const screen_line_t ** l, const codepoint_info_s ** cpi);

EDITOR_EXPORT
void               screen_set_ready_flag(screen_t * scr, int flag);

EDITOR_EXPORT
int                screen_get_ready_flag(const screen_t * scr);

#ifdef __cplusplus
}
#endif
