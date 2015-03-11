#pragma once

#include <ew/core/types/types.hpp>
#include <ew/filesystem/file.hpp>

#include <ew/core/object/buffer.hpp>


namespace ew
{

namespace filesystem
{

class mapped_file : public ew::core::objects::buffer
{
	typedef ew::core::container::memory_mapped_page<u8> mpage;
	typedef ew::core::container::page<u8> simple_page;
	typedef ew::core::objects::buffer base_class;
private:
	file   *   _file;
	open_mode  _mode; // alloc sync to disk

public:

	typedef base_class::iterator iterator;


	bool open(open_mode mode = mode::read_only)
	{

		_mode = mode;

		/* the underlying file is readonly */
		if (!_file->open(mode::read_only)) {
			std::cerr << "cannot open " << _file->name() << "\n";
			return false;
		}


		u64 file_size = _file->size();

		base_class::set_initial_partition_size(file_size);
		base_class::build_partitions();

		u64 off = 0;
		while (off < file_size) {
			u64 local_off = 0;
			node * n = this->find(off, local_off);
			assert(n);

			u64 page_sz = n->size(); // std::min(remain,  base_class::max_page_size());
			simple_page * p = new mpage(_file, page_sz, off, 0);
			n->set_page(p);

			off    += page_sz;
		}

		return true;
	}

public:

	explicit mapped_file(const char * filename, u32 nrPagePerBlock = 1)
		: base_class(4096 * nrPagePerBlock, 4096 * nrPagePerBlock)
	{
		_file = new file(filename);
	}

	~mapped_file()
	{
		if (_file) {
			_file->close();
			delete _file;
			_file = nullptr;
		}
	}

	// TODO: move to base class
	// will allow buffer with no underlying file
	virtual const char * filename() const
	{
		if (_file)
			return _file->name();

		return 0; // "" ?
	}


	template <class OutputIterator>
	bool read(iterator before, OutputIterator first_out, OutputIterator last_out, u64 * nrRead)
	{
		return base_class::copy(before, base_class::end(), first_out, last_out, nrRead);
	}

	template <class OutputIterator>
	bool read(u64 offset, OutputIterator first_out, u64 nr_to_read, u64 * nr_read)
	{
		// TODO: cache last read it rator
		iterator before(base_class::new_iterator(offset));
		OutputIterator last_out(first_out + nr_to_read);
		return base_class::copy(before, base_class::end(), first_out, last_out, nr_read);
	}


	// move to base_class

	/* save */
	class save_func_object_t
	{
	public:
		file * new_file;
		u64 nr_written;

		save_func_object_t()
		{
			new_file = 0;
			nr_written = 0;
		}

		~save_func_object_t()
		{
			new_file = 0;
			nr_written = 0;
		}

		bool open(const char * new_filename)
		{
			new_file = new file(new_filename);
			assert(new_file != 0);

			if (! new_file->create(mode::read_write)) {
				std::cerr << "cannot create " << new_file->name() << "\n";
				delete new_file;
				new_file = 0;
				return false;
			}

			return true;
		}

		bool close()
		{
			delete new_file;
			new_file = 0;
			return true;
		}


		// write page
		bool operator()(u8 * pg_begin, u8 * pg_end)
		{
			u32 pg_size = pg_end - pg_begin;

			bool ret = new_file->write(pg_begin, pg_size, &nr_written);
			assert(ret == true);
			if (ret == true) {
				if (nr_written != pg_size) {
					ret = false;
				}
			}

			return ret;
		}

	};



	bool save(const char * new_filename)
	{

		save_func_object_t fobj;

		bool ret = fobj.open(new_filename);
		if (ret == false) {
			return false;
		}

		fobj = this->foreach_pages(fobj);

		ret = fobj.close();
		if (ret == false) {
			return false;
		}

		// TODO: check fobj.ret
		return true;


		return true;
	}

	bool save()
	{
		return save(filename());
	}

};

}
}
