#pragma once

#include <stdint.h>
#include <stddef.h>

#include "editor_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
  A mark is used to remember a given position (into a given buffer).

  A mark as 2 properties:
  *) an offset (unsigned 64 bits integer)
  *) a name    (an ascii string up to mark_name_max_size() bytes) the last byte is set to '\0'

*/

///
/// \brief enum mark_type_e pointer type
///  the OFFSCREEN_MARK/ONSCREEN_MARK masks must be always combined with either FIXED_MARK and/or MOVING_MARK
typedef enum mark_type_e {
    FIXED_MARK     = (1 << 0),
    MOVING_MARK    = (1 << 1),
    OFFSCREEN_MARK = (1 << 2),
    ONSCREEN_MARK  = (1 << 3),
} mark_type_t;

///
/// \brief mark_t pointer type
///
typedef struct mark_s * mark_t;

///
/// \brief mark_new
/// \return a new mark
EDITOR_EXPORT
mark_t mark_new(uint64_t offset, const char * name);

///
/// \brief mark_release
/// \return deletes a given mark
EDITOR_EXPORT
void mark_release(mark_t m);


///
/// \brief  mark_name_max_size
/// \return the maximum number of bytes a mark name can use
EDITOR_EXPORT
size_t  mark_name_max_size();


///
/// \brief mark_set_name sets the name of the mark (it is up to to the user to provide uniqness)
/// \param m the mark
/// \param name
///
EDITOR_EXPORT
void         mark_set_name(mark_t m,   const char * name);

///
/// \brief mark_get_name
/// \param m the mark
/// \return the pointer to the the mark's name
///
EDITOR_EXPORT
const char * mark_get_name(mark_t m);

///
/// \brief mark_set_offset
/// \param m the mark
/// \param offset
///
EDITOR_EXPORT
void         mark_set_offset(mark_t m, const uint64_t offset);

///
/// \brief mark_get_offset
/// \param m the mark
/// \return the mark's offset
///
EDITOR_EXPORT
uint64_t     mark_get_offset(mark_t m);


#ifdef __cplusplus
}
#endif
