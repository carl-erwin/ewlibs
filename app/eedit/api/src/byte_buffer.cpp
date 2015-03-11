#include <stdio.h>
#include <string>
#include <array>

#include <ew/filesystem/file.hpp>
#include <ew/filesystem/mapped_file.hpp>
//using namespace ew::filesystem;

// using namespace ew::core::objects; // ::stream;

#include "handle_table.h"

#include "../include/byte_buffer.h"

#include "core/log.hpp"

struct byte_buffer_s {
	typedef ew::filesystem::mapped_file file_t;

	byte_buffer_s(const char * filename_, file_t * fd_)
		:
		fd(fd_),
		file_name_path(filename_)
	{
	}

// handler
	file_t * fd = nullptr;

// properties
	std::string file_name_path; // abs path to filename


	size_t start_offset = 0; ///
	size_t start_line   = 0; ///
	size_t change_count = 0; ///
};


/// local table
static handle_index_allocator<byte_buffer_s> table;

///////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {

	SHOW_SYMBOL
	byte_buffer_id_t byte_buffer_open(const char * filename, const int mode)
	{

		size_t nr_pages = 256; // * 4k
		auto fd = new ew::filesystem::mapped_file(filename, nr_pages);
		if (! fd->open(ew::core::objects::stream::mode::read_only)) {
			delete fd;
			return 0;
		}

		byte_buffer_s * file;
		byte_buffer_id_t bid = 0;
		std::tie(file , bid) = table.construct(filename, fd);
		if (bid == 0) {
			return 0;
		}

		app_log << __PRETTY_FUNCTION__ << " allocated fd   = " << fd << "\n";
		app_log << __PRETTY_FUNCTION__ << " allocated fd   = " << bid << "\n";
		app_log << __PRETTY_FUNCTION__ << " allocated file = " << file << "\n";
		return bid;
	}

	SHOW_SYMBOL
	int     byte_buffer_close(byte_buffer_id_t bid)
	{
		app_log << __PRETTY_FUNCTION__ << " release fd = " << bid << "\n";
		auto ret = table.destroy(bid);
		return ret == true;
	}

	SHOW_SYMBOL
	int     byte_buffer_size(const byte_buffer_id_t bid, size_t * sz)
	{
		auto file = table.get(bid);
		if (file == nullptr)
			return 0;

		if (sz) {
			assert(file->fd);
			*sz = file->fd->size();
		}
		return 0;
	}


	SHOW_SYMBOL
	int     byte_buffer_read(const byte_buffer_id_t bid, const uint64_t offset,  uint8_t out[],  size_t to_read, size_t * nb_read)
	{
		auto file = table.get(bid);
		if (file == nullptr)
			return -1;

		if (offset > file->fd->size()) {
			return 0;
		}

		// TODO: cache last offset + read size, for this bid

		u64 nb_read_ = 0;
		file->fd->read((u64)offset, (u8 *)out, to_read, &nb_read_);
		*nb_read = nb_read_;

		return 0;
	}


	SHOW_SYMBOL
	int     byte_buffer_changed_flag(const byte_buffer_id_t bid, size_t * count)
	{
		auto file = table.get(bid);
		if (file == nullptr)
			return -1;

		*count = file->change_count;
		return 0;
	}

}
