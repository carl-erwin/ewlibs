#pragma once

#include <list>
#include <ew/core/types/types.hpp>
#include <ew/core/object/buffer.hpp>

/*
  derived from http://www.finseth.com/
*/

/*
  modes (interpreters)
  raw_buffer (optionnally on top of files)
*/

namespace eedit
{
typedef char * mark_name;
typedef int    status;
typedef bool   flag;
typedef ew::core::objects::buffer::iterator location;

//
class mark
{
public:
	// mark_name name; //
	location mark_location;
	bool     is_fixed;
};

//
class mode
{
public:
	char * mode_name;
	int (*add_proc)();
};

class raw_buffer
{
public:
	// + text_mode

	// optionnal file

	ew::core::objects::buffer * contents; // mapped_buffer;

	char   file_name[1024]; //
	time_t file_time;
	bool   is_modified;
};

// todo undo/redo system
// vcs buffer ?
class log_buffer : public raw_buffer
{
	// add revision / branchnig etc .... / mecanism
};

//
class text_buffer
{
public:
	text_buffer(raw_buffer * raw_buff); // ---> append in mode list

	// buffer api
	int    clear(char * buffer_name);
	int    set_name(char * buffer_name);
	char * get_name(void);


	// point api
	status   point_set(location loc);
	status   point_move(int count);
	location point_get(void);
	int      point_get_line(void);
	location buffer_start(void);
	location buffer_end(void);

	int      compare_locations(const location & loc1, const location & loc2);
	int      location_to_count(const location & loc);
	location count_to_location(const int count);

	// mark
	mark * get_previous_mark();
	mark * get_next_mark();
	mark * get_current_mark();

	status   mark_create(mark_name * name, flag is_fixed);
	void     mark_delete(mark_name name);
	status   mark_to_point(mark_name name);
	status   point_to_mark(mark_name name);
	location mark_get(mark_name name);
	status   mark_set(mark_name name, location loc);

	flag     is_point_at_mark(mark_name name);
	flag     is_point_before_mark(mark_name name);
	flag     is_point_after_mark(mark_name name);
	status   swap_point_and_mark(mark_name name);

	//  file api
	void   get_file_name(char * file_name, int size);
	status set_file_name(char * file_name);
	status buffer_write(void);
	status buffer_read(void);
	status buffer_insert(char * file_name);
	flag   is_file_changed(void);
	void   set_modified(flag is_modified);
	flag   get_modified(void);


	// text manipulation : add codec support ?
	s32  get_char(void); // start @ point
	void get_string(s32 * string, u32 count);  // start @ point, add offset ?
	int  get_num_chars(void); // optionall
	int  get_num_lines(void); // optionall

	void   insert_codepoint(s32 c);
	void   insert_string(s32 * string, size_t nr_cp);
	void   replace_char(s32 c);
	void   replace_string(s32 * string, size_t nr_cp);
	status delete_codepoints(size_t count);
	status delete_region(mark_name name);
	status copy_region(char * buffer_name, mark_name name);

	// search api
	status search_forward(s32 * string, int len);
	status search_backward(s32 * string);
	flag   is_a_match(s32 * string, int len);
	status find_first_in_forward(s32 * string, int len);
	status find_first_in_backward(s32 * string, int len);
	status find_first_not_in_forward(s32 * string, int len);
	status find_first_not_in_backward(s32  * string, int len);

	// modes like a pipe line
	status mode_append(char * mode_name, status(*add_proc)(), flag is_front);
	status mode_delete(char * mode_name);
	status mode_invoke(void);

	// mode api
	mode * get_previous_mode();
	mode * get_next_mode();
	mode * get_current_mode();

private:
	char     buffer_name[1024];
	location point;
	u64      cur_line;
	u64      num_chars;
	u64      num_lines;

	raw_buffer * ll_buffer;

	std::list < mark * > mark_list;
	std::list < mode * > mode_list;
};


// world
class editor
{
public:
	editor();
	~editor();

	int editor_init(void); // rename in open();
	int editor_quit(void); // rename in close();

	int save_state(char * file_name);
	int load_state(char * file_name);


	// buffer manipulation
	raw_buffer * get_previous_buffer();
	raw_buffer * get_next_buffer();
	raw_buffer * get_next_current();
	int          set_current_buffer_by_name(char * buffer_name);
	raw_buffer * set_next_buffer(void);

	// takes a name and creates an empty buffer with that name. (name is exclusive)
	int create_buffer(char * buffer_name);

	// buffer_clear removes all characters and marks from the specified buffer.
	int clear_buffer(char * buffer_name);

	// deletes the specified buffer. If the specified buffer is the current one,
	// the next buffer in the chain becomes the current one.
	// If no buffers are left, the initial "scratch" buffer is automatically re-created.
	int delete_buffer(char * buffer_name);
private:
	//
	class editor_private;
	editor_private * d;

	// move to private
	std::list< raw_buffer * > buffer_list;
	std::list< raw_buffer * >::iterator current_buffer_it;
};


} // ! namespace eedit

///////////////////////////////////////////////////////

#if 0

// ewlibs ?
eedit / raw_buffer / (raw bytes + files)
eedit / log_system / (undo redo system(at byte level))

// eedit
eedit / core / sub_editor / (model) + (command set)
eedit / ui / (view)  + (controler)
eedit / ui / ncurses /
eedit / ui / qt /
eedit / ui / ew /

#endif
