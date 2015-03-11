#pragma once


/*
 * the buffer "object" represent a linear array of bytes
 * it can be in memory only or backed by an on disk file.
 *
 * The editor modes use this api to read/modify the content of the file at the byte level
 *
 */


#include <stddef.h>
#include <stdint.h>

#include "editor_types.h"

#include "mark.h"

#ifdef __cplusplus
extern "C" {
#endif


/**! int     buffer_open(const char * filename, const char * buffer_name, const int mode, buffer_id * allocated_bid)
 * filename param[in] path to the file we want to load in the buffer, use "/dev/null" to create empty buffer
 * this function allocate a buffer
 * if filename is null the content will be stored in heap
 * if filename is non null the the content will be read from the file
 * if buffer_name is null , filename will be used to give a name to the buffer
 * mode = 0 : read only , mode 1 : read_write
 * the allocated_bid pointer will be filled on successfull open operation
*/
byte_buffer_id_t byte_buffer_open(const char * filename, const int mode);

// this function close a previously opened buffer see buffer_open
int     byte_buffer_close(byte_buffer_id_t bid);


// set the name of the buffer
int     byte_buffer_set_name(byte_buffer_id_t bid, const char * buffer_name);

// returns the name of the buffer
int     byte_buffer_get_name(byte_buffer_id_t bid, char * buffer_name, size_t max);

// returns the name of the filename associated to the buffer
int     byte_buffer_get_filename(byte_buffer_id_t bid, char * file_name, size_t max);

// change the on disk target file
int     byte_buffer_set_filename(byte_buffer_id_t bid, const char * file_name, size_t max); // TODO:



// this function returns the number of bytes a given buffer contains
int     byte_buffer_size(byte_buffer_id_t bid, size_t * sz);

// this function returns in flag
//     0  => the no change since last save
//     >0 => the number of changes since last save
int     byte_buffer_changed_flag(byte_buffer_id_t bid, size_t * flag);



// this function copy the content of the buffer upto 'to_read'
// nb_read is filled with the number of copied bytes
// return -1 on error
int     byte_buffer_read(const byte_buffer_id_t bid, const uint64_t offset,  uint8_t out[],  size_t to_read, size_t * nb_read);

// this function insert the 'in' array content in the buffer upto 'to_ins'
// nb_read is filled with the number of copied bytes
// return -1 on error
int     byte_buffer_insert(const byte_buffer_id_t  bid, const uint64_t offset, const uint8_t in[], const size_t to_ins, size_t * nb_ins);

//   if to_rm is provided will call buffer_read before remove the bytes
int     byte_buffer_remove(const byte_buffer_id_t  bid, const uint64_t offset, uint8_t removed[], const size_t to_rm,  size_t * nb_rm);

// can be used to know the number of blocks that compose the buffer, api to be used by indexer etc...
int     byte_buffer_nb_pages(const byte_buffer_id_t  bid, uint64_t * nb_pages);
int     byte_buffer_get_page_info(const byte_buffer_id_t  bid, const uint64_t page_index, uint64_t offset, size_t size);




//////
// TODO:  editor_buffer_event ?
// must provide a  way to notify modes/plugins of buffer changes
// this will be used by indexers,highlight,syntax chec and so on...

//typedef uint64_t  register_id;
// typedef void (on_buffer_page_event_cb)(const buffer_id bid, const uint64_t page_index, const int ev_type);
// int     buffer_register_page_event(const buffer_id bid,   on_buffer_page_event_cb cb, register_id * register_index);  // ev_type = page_modifed/page_rm/page_add
// typedef void (on_buffer_event_cb)(const buffer_id bid, int ev_type);
// int     buffer_register_buffer_event(const buffer_id bid, on_buffer_event_cb cb);


#ifdef __cplusplus
}
#endif
