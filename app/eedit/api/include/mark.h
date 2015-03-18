#pragma once

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
///
typedef enum mark_type_e {
	FIXED_MARK,
	MOVING_MARK,
} mark_type_t;


///
/// \brief mark_t pointer type
///
typedef struct mark_s * mark_t;

///
/// \brief mark_new
/// \return a new mark
mark_t mark_new(uint64_t offset, const char * name);

///
/// \brief mark_release
/// \return deletes a given mark
void mark_release(mark_t m);


///
/// \brief  mark_name_max_size
/// \return the maximum number of bytes a mark name can use
size_t  mark_name_max_size();


///
/// \brief mark_set_name sets the name of the mark (it is up to to the user to provide uniqness)
/// \param m the mark
/// \param name
///
void         mark_set_name(mark_t m,   const char * name);

///
/// \brief mark_get_name
/// \param m the mark
/// \return the pointer to the the mark's name
///
const char * mark_get_name(mark_t m);

///
/// \brief mark_set_offset
/// \param m the mark
/// \param offset
///
void         mark_set_offset(mark_t m, const uint64_t offset);

///
/// \brief mark_get_offset
/// \param m the mark
/// \return the mark's offset
///
uint64_t     mark_get_offset(mark_t m);


#ifdef __cplusplus
}
#endif
