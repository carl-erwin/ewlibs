#pragma once

#include <ew/core/object/buffer.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/threading/thread.hpp>

using namespace ew::core::types;
using namespace ew::core::threading;
#include <ew/core/threading/mutex_locker.hpp>

#include <ew/core/time/time.hpp>
using namespace ew::core::time;

#include <ew/filesystem/file.hpp>
#include <ew/filesystem/mapped_file.hpp>
using namespace ew::filesystem;


#include <ew/codecs/text/textcodec.hpp>
using namespace ew::codecs::text;

#include <ew/codecs/text/unicode/utf8/utf8.hpp>
using namespace ew::codecs::text::unicode;


#include <ew/maths/maths.hpp>

#include "core/log.hpp"

#include "text_page_data.hpp"

#define  INVALID_CP_CACHE (-20)

#include "text_buffer_indexer.hpp"


#include "../api/include/byte_buffer.h"
#include "../api/include/buffer_log.h"


// move to counter.h
/* TODO: add over/underflow checks */
template <class T>
class Counter
{
public:
	Counter() : _count(0) { }

	void increment(u64 val = 1)
	{
		_count += val;
	}

	void decrement(u64 val = 1)
	{
		_count -= val;
	}

	void set(u64 val)
	{
		_count = val;
	}

	T operator()() const
	{
		return _count;
	}

private:
	T _count;
};


namespace eedit
{

// forward


class  text_buffer_indexer;
class  text_buffer;
struct index_context;
///////////////////////////////////////////////////////////////////////////////////////////////////



static void index_text_buffer(text_buffer * text_buff)
{
	text_buffer_indexer::build_index(text_buff);
}

class text_buffer : public mutex
{
public:
	class iterator;
	friend class iterator;
	friend class text_buffer_indexer;

public:
// new apis
	struct index_context * index_ctx;

public:
	text_buffer(ew::core::objects::buffer * buffer_,  byte_buffer_id_t _bid)
	{
		d.owner = this;
		d.buffer = buffer_;
		d.codec = ew::codecs::text::unicode::utf8::codec(); // TODO: codec detection
		d.is_valid = true;
		d.bid = _bid;
		buffer_log_init((byte_buffer_id_t)this, &d.log);
	}

	~text_buffer()
	{
		{
			mutex_locker lock(this);
			d.is_valid = false;
		}

		stop_indexer();

		delete d.line_cache;
		d.line_cache = 0;

		buffer_log_destroy(d.log);
	}

	buffer_log_id_t log_id()
	{
		return d.log;
	}

	bool save_buffer()
	{
		bool ret = false;

		const char * file_name = 0;

		file_name = d.buffer->filename();
		if (file_name != 0) {
			char   tmp_file_name[1024];
			char   old_file_name[1024];

			snprintf(tmp_file_name, sizeof(tmp_file_name), "%s.eedit.tmp", file_name);
			snprintf(old_file_name, sizeof(old_file_name), "%s.eedit.old", file_name);

			app_log << "saving " << tmp_file_name << "\n";

			// TODO: check exits before remove
			::unlink(tmp_file_name);

			ret = d.buffer->save(tmp_file_name);
			if (ret == true) {

				// TODO: hard link old file
				// sys_ret = link(file_name, old_file_name);

				// remove filename
				int sys_ret = unlink(file_name);
				if (sys_ret != 0) {
					// remove hardlink
					return false;
				}

				// rename tmp file
				sys_ret = rename(tmp_file_name, file_name);
				if (sys_ret != 0) {
					sys_ret = ::unlink(tmp_file_name); // remove temporary file
					return false;
				}
			}
		}

		return ret;
	}

	bool start_indexer()
	{
		mutex_locker lock(this);

		if (d.th_indexer)
			return true;

		d.th_indexer = new thread((thread::func_t)index_text_buffer, (void *)this, "file indexer");
		bool ret = d.th_indexer->start();

		return ret;
	}



	bool stop_indexer()
	{
		if (d.th_indexer) {
			// bool ret = d.th_indexer->stop();
			d.th_indexer->join();

			mutex_locker lock(this);
			delete d.th_indexer;
			d.th_indexer = nullptr;
		}

		return true;
	}

	bool is_indexed() const
	{
		return this->d.is_indexed;
	}

	bool is_valid() const
	{
		return d.is_valid;
	}

	byte_buffer_id_t id() const
	{
		return d.bid;
	}

	// remove this ?
	ew::core::objects::buffer * buffer() const
	{
		return d.buffer;
	}

	// codec
	text_codec * codec() const
	{
		return d.codec;
	}

	// move to indexer ???
	// line
	Counter<u64> numberOfLines; // nr of '\n' + 1
	Counter<u64> numberOfCodepoints;

	// ? rename haveLineNumber(u64 n)
	u64 checkLine(u64 num)
	{
		if (!num)
			return 1;
		if (num > numberOfLines())
			num = numberOfLines();
		return num;
	}

	// only if indexed , REMOVE ?
	/*
	 *  This function works only on indexed page
	 *  TODO: add a flag in indexed pages
	 *  column WILL not go beyond end-of-line
	 *  if the line was not found return the last indexed line start
	 */
	// TODO: return bool , take output iterator as argument
	iterator get_line(u64 line, u64 column)
	{
		//  cerr << __FUNCTION__ << " line = " << line << ", " << "column = " << column << "\n";
		// this->check_line_number();
		assert(line <= numberOfLines());
		if (line > numberOfLines()) {
			column = 1;
			line = numberOfLines();
		}

		bool ret;

		// init
		text_buffer::iterator it_ret;
		it_ret.d.owner = this;   // it_ret.d.init(this); helper ?

		// get line start
		// use line cache move to helper start of_line
		ret = d.find_start_of_line(line, &it_ret);
		assert(ret == true);
		if (ret == false) {
			// TODO: return the last index line start
		}

		if (column == 1) {
			return it_ret;
		}

		// find_line_column()
		// ret = d.find_line_column(column, &it_ret);
		// TODO:
		// read page nr cp;
		// if (local_new_line == page->nr_new_lines // look in next pages
		// search for '\n' in this page

		while (it_ret.d.column != column) {
			++it_ret;
			if (it_ret.d.line != line) {
				--it_ret;
				break;
			}
		}

		// it_ret.check_invariant();

		return it_ret;
	}

	// text

	iterator begin()
	{
		iterator it;

		it.d.owner = this;
		it.d.buff_it = d.buffer->begin();

		if (this->is_indexed() == true) {
			it.d.line   = 1;
			it.d.column = 1;
		} else {
			it.d.line   = 0;
			it.d.column = 0;
		}
		return it;
	}

	iterator end()
	{
		iterator it;

		it.d.owner = this;
		it.d.buff_it = d.buffer->end();

		if (this->is_indexed() == true) {
			it.d.line   = numberOfLines();
			it.d.column = (u64) - 1; // this->get_column(it);
		} else {
			it.d.line   = 0;
			it.d.column = 0;
		}

		return it;
	}

	iterator get_iterator(u64 off)
	{
		text_buffer::iterator it;
		it.d.owner = this;
		it.d.buff_it = d.buffer->begin() + off;
		// TODO: fill other fields : line/comlumn
		return it;
	}

	//
	bool remove(u64 buffer_id, const text_buffer::iterator & it0, const text_buffer::iterator & it1, int do_log = 1)
	{
		if (do_log) {
			// SAVE
			size_t size = it1.offset() - it0.offset();
			u8 * data = new u8[size+1];
			memset(data, 0, size+1);
			auto b = this->d.buffer->begin() + it0.offset();
			auto e = this->d.buffer->begin() + it1.offset();
			this->d.buffer->copy(b, e, data);

			buffer_commit_rev_t commit;
			buffer_log_remove(d.log, it0.offset(), data, size, &commit);
			delete [] data;
		}

		this->d.buffer->erase(it0.d.buff_it, it1.d.buff_it);
		return true;
	}

	bool insert(const text_buffer::iterator & it, const u8 * data, u64 data_size);


	/* TODO: should take a cursor as parameter
	 *  must have TextBuffer::iterator
	 */
	bool remove_previous_char(u64 buffer_id, const text_buffer::iterator & it)
	{
		if (it.offset() == 0) {
			return true;
		}
		iterator itprev(it);
		--itprev;
		return remove_current_char(buffer_id, itprev);
	}
	// TODO: add buffer->remove(const buffer::iterator & , const buffer::iterator & );
	// TODO: add buffer->remove(const buffer::iterator, const u64 len);
	bool remove_current_char(u64 buffer_id, const text_buffer::iterator & it0, int do_log = 1)
	{
		iterator it1(it0);
		++it1;
		if (it0 == it1)
			return false;

		return remove(buffer_id, it0, it1, do_log);
	}

	// extern ?
	bool buffer_insert(u64 buffer_id, u64 offset, u8 * payload, size_t len)
	{
		return 0;
	}

	/* TODO: must move line offsets */
	bool insert_codepoint(u64 buffer_id, const s32 codepoint, const text_buffer::iterator & it, int do_log = 1)
	{
		u64 nrWritten;

		// current codec
		u8 utf8[8];
		u32 utf8_sz = 7;
		switch (codepoint) {
		default:
			if (d.codec->put_cp(codepoint, utf8, &utf8_sz)) {
			} else {
				app_log << "INSERT UNICODE ERROR !!!!!!\n";
			}
		}



		u64 off = it.d.buff_it.offset();

		if (do_log) {
			buffer_commit_rev_t commit;
			buffer_log_insert(d.log, off, utf8, utf8_sz, &commit);
		}

		d.buffer->insert(it.d.buff_it.offset(), utf8, utf8_sz, &nrWritten);
		// d.buffer->insert(it.d.buff_it, utf8, utf8 + utf8_sz, &nrWritten);
		// it.d.buff_it.check_invariants(); only if auto refresh

		assert(nrWritten == utf8_sz);
		return nrWritten != 0;
	}

	/* no error reported YET */
	/* TODO: use direct member access */
	// move to text codec template ?

	bool read(const text_buffer::iterator & textbuffer_it,
		  s32 * cps,
		  u64 max,
		  u64 * cp_offset,
		  u32 * nr_cp_read)
	{
		textbuffer_it.check_invariants();

		auto s = textbuffer_it.d.buff_it;
		auto end = d.buffer->end();

		u32 & nr_fwd_cp = *nr_cp_read;
		u32 nr_fwd_ok = 0;
		u32 nr_fwd_error = 0;
		s32 unicode;
		*cp_offset++ = s.offset();
		while (s < end) {
			auto next   = ew::codecs::text::get_cp<decltype(s)>(d.codec, s, end, &unicode);
			*cps++   = unicode;
			*cp_offset++ = next.offset();
			assert(next > s);

			++nr_fwd_cp;
			if (unicode < 0) {
				++nr_fwd_error;
			} else {
				++nr_fwd_ok;
			}
			if (nr_fwd_cp == max)
				break;
#if 0
			app_log << "offset = " << (s - buf) << "\n";
			app_log << "unicode " << unicode << ", ";
			app_log << "unicode 0x" << std::hex << unicode << std::dec << ", ";
			app_log << "unicode char " << (char)unicode << "\n";
			app_log << "advance = " << (next - s) << "\n";
#endif
			s = next;
		}

		return true;
	}


public:
	class iterator
	{
	public:
		iterator() {}

		inline iterator(const iterator & orig)
		{
			*this = orig;
		}


		~iterator() {}

		text_buffer * textBuffer() const
		{
			return d.owner;
		}

		text_buffer * owner() const
		{
			return d.owner;
		}

		buffer::iterator & get_buffer_iterator()
		{
			return d.buff_it;
		}

		inline iterator & operator = (const iterator & orig)
		{

			if (this != &orig) {
				d.owner = orig.d.owner;
				d.buff_it = orig.d.buff_it;

				d.line = orig.d.line;
				d.column = orig.d.column;
				d.cache_c = orig.d.cache_c;
			}
			return *this;
		}

		void check_invariants() const
		{
			// add more check
			this->d.buff_it.check_invariants();
		}


		inline buffer::iterator read_codepoint(int * cp)
		{
			u64 maximum_codepoint_size = d.owner->d.codec->maximum_codepoint_size();

			u64 off = d.buff_it.offset();

			u64 diff = d.owner->d.buffer->size() - off;

			diff = ew::maths::min(diff, maximum_codepoint_size);

			buffer::iterator cp_limit(d.buff_it);
			while (diff--)
				++cp_limit;

			return ew::codecs::text::get_cp<buffer::iterator>(d.owner->d.codec, d.buff_it, cp_limit, cp);
		}

		inline s32  operator *()
		{
			if (d.cache_c != INVALID_CP_CACHE) {
				return d.cache_c;
			}

			read_codepoint(&d.cache_c);
			return d.cache_c;
		}

		inline iterator & operator++()
		{
			// u64 buff_sz = d.owner->buffer()->size();
			u64 buff_sz = d.owner->d.buffer->size();
			u64 prev_offset = d.buff_it.offset();

			if (prev_offset >= buff_sz) {
				d.cache_c = INVALID_CP_CACHE;
				return *this;
			}

			// read code point
			d.cache_c = 0;
			auto it_old = d.buff_it;
			d.buff_it = read_codepoint(&d.cache_c);

			assert(d.buff_it.offset() > it_old.offset());
			assert(d.buff_it.offset() <= buff_sz);

			// update column ?
			if (prev_offset != d.buff_it.offset()) {
				if (d.column) {
					d.column++;
				}

				if (this->isEol(d.cache_c)) {
					d.column = 1;
					if (d.line) {
						d.line++;
					}
				}

				d.cache_c = INVALID_CP_CACHE;
			}

			return *this;
		}


// it++ remove ?
		inline iterator operator++(int)
		{
			assert(0);
			text_buffer::iterator it(*this);
			this->operator ++ ();
			return it;
		}

// --it
		inline iterator & operator--()
		{
			u64 off = d.buff_it.offset();

			if (off == 0) {
				d.column = 1;
				d.line   =  1;
				return *this;
			}

			buffer::iterator it_old(d.buff_it);

			u64 maximum_codepoint_size = 4; // TODO: u32 codec->maximum_codepoint_size();
			u64 diff = off;
			diff = ew::maths::min(diff, maximum_codepoint_size);
			buffer::iterator cp_limit(it_old);
			while (diff--)
				--cp_limit;

			s32 cp = d.owner->d.codec->rget_cp(cp_limit, it_old, &d.buff_it);
			if (cp == '\n') {
				if (d.line)
					d.line--;
				d.column = 0;
				// TODO: index column
				// get start_of_line_(this->line)
				// column = count_cp(start_off_line, this->buff_it.offset());
			}

			assert(off != d.buff_it.offset());

			if (off != d.buff_it.offset()) {

				if (d.column) {
					d.column--;
				}

				d.cache_c = INVALID_CP_CACHE;
			}

			return *this;
		}

// it--
		iterator operator--(int)
		{
			assert(0);
			text_buffer::iterator it(*this);
			this->operator -- ();
			return it;
		}

		bool operator == (const iterator & it2) const
		{
			assert(this->d.owner == it2.d.owner);
			return (this->d.buff_it == it2.d.buff_it);
		}

		bool operator != (const iterator & it2) const
		{
			assert(this->d.owner == it2.d.owner);
			return (this->d.buff_it != it2.d.buff_it);
		}


		bool operator > (const iterator & it2) const
		{
			assert(this->d.owner == it2.d.owner);
			return (this->d.buff_it > it2.d.buff_it);
		}

		bool operator < (const iterator & it2) const
		{
			assert(this->d.owner == it2.d.owner);
			return (this->d.buff_it < it2.d.buff_it);
		}

		bool operator >= (const iterator & it2) const
		{
			assert(this->d.owner == it2.d.owner);
			return (this->d.buff_it >= it2.d.buff_it);
		}

		bool operator <= (const iterator & it2) const
		{
			assert(this->d.owner == it2.d.owner);
			return (this->d.buff_it <= it2.d.buff_it);
		}

		u64 offset() const
		{
			return d.buff_it.offset();
		}


		u64 line() const
		{
			if (d.line == 0) {
				d.owner->d.find_line_by_offset(offset(), const_cast<u64 *>(&d.line));
			}

			return d.line;
		}


		u64 column() const
		{
			return d.column;
		}

		// returns an iterator relative to mark position
		iterator move(s64 line_inc, s64 column_inc)
		{
			u64 l = line();
			u64 c = column();

			// if l == 0 or c == 0
			//

			if (line_inc < 0) {
				u64 l2 = l + line_inc;
				if (l > l2) {
					line_inc = 0;
				}
			}

			if (column_inc < 0) {
				u64 c2 = c + line_inc;
				if (c > c2) {
					column_inc = 0;
				}
			}

			l += line_inc;
			c += column_inc;

			*this = d.owner->get_line(l, c);

			return *this;
		}

		bool move_to_offset(u64 offset)   // will compute (line, column) only if indexed
		{
			d.line = 0;
			d.column = 0;

			// set new data
			this->d.owner->buffer()->get_iterator_by_offset(offset, &d.buff_it);

			u64 line;

			// BROKEN: move to index mode
			if (0) {
				this->owner()->d.find_line_by_offset(offset, &line);
			}

			return true;
		}

		void dump()
		{
			app_log << "(" << line() << ", " << column() << ") @ " << offset() << "\n";
		}

		// end of line
		inline bool isEol(const s32 cp) const
		{
			if (cp == '\r') // other function must skip \n
				return true;

			if (cp == '\n')
				return true;

			return false;
		}

		inline bool isEol()
		{
			s32 c = this->operator * ();
			return this->isEol(c);
		}

		// end of file
		inline bool isEof()
		{
			//  u64 buff_sz = d.owner->buffer()->size();
			u64 buff_sz = d.owner->d.buffer->size();
			if (offset() == buff_sz) {
				return true;
			}

			return false;
		}


		bool is_beginning_of_line() const
		{
			return (d.column == 1);
		}


		/*
		 *  FIXME: use r_search + indexer info if available
		 */
		bool toBeginningOfLine()
		{
//            app_log << __PRETTY_FUNCTION__ << "\n";
			// search by indexer info + codec

			if (offset() == 0) {
				d.column = 0;
				return true;
			}
			// utf8 rsearch BUGGY FOR NOW

			d.cache_c = INVALID_CP_CACHE;

			buffer::iterator & itb0(d.buff_it);

			// FIXME: TODO bin search
			//        encode '\r'
			//        encode '\n'

			// search by iterator
			// slow but correct
			while (true) {
				--itb0;
				if (itb0.offset() == 0) {
					d.column = 0;
					return true;
				}

				s32 c = *itb0;
				if (c == '\r') {
					// skip end of previous line
					++itb0;
					if (*itb0 == '\n') {
						// skip end of previous line
						this->operator ++ ();
					}
					d.column = 0;
					return true;
				}

				if (c == '\n') {
					// skip end of previous line
					++itb0;
					d.column = 0;
					return true;
				}

			}

			assert(0);
			return true;
		}


		/*
		 *  FIXME: use search_n
		 */
		bool toEndOfLine()
		{
#if 1
			u64 nr_ok;
			u64 nr_err;

			buffer::iterator out;

			s32 c = this->operator * ();
			if ((c == '\n') || (c == '\r')) {
				return true;
			}

			buffer::iterator itb0(d.buff_it);
			buffer::iterator itb_end(d.owner->buffer()->end());
#if 0
			app_log << " 0 - itb0.offset() = " << itb0.offset() << "\n";
			app_log << " 0 - itb_end.offset() = " << itb_end.offset() << "\n";
#endif
			text_codec  * codec = d.owner->codec();

//            u32 t0 = ew::core::time::get_ticks();
			bool bret = search_n<buffer::iterator>(codec, (u64)1, d.buff_it, itb_end, '\n', nr_ok, nr_err, out);
//           u32 t1 = ew::core::time::get_ticks();


#if 0
			app_log << " time to search_n " << (t1 - t0) << " ms\n";
			app_log << " nr_ok = " << nr_ok << "\n";
			app_log << " nr_err = " << nr_err << "\n";
			app_log << " nr_ok + nr_err = " << nr_ok + nr_err << "\n";
#endif
			if (bret == true) {
//                app_log << " end-of-line was found\n";
				d.buff_it = out;
			} else {
//                app_log << " end-of-line not found\n";
				d.buff_it = itb_end;
			}

			if (d.column) {
				d.column += nr_ok + nr_err;
			}

#if 0
			app_log << " 1 - itb0.offset() = " << itb0.offset() << "\n";
			app_log << " 1 - itb_end.offset() = " << itb_end.offset() << "\n";
			app_log << " 1 - out.offset() = " << d.buff_it.offset() << "\n";
#endif
			// add helper : it.invalidate_cache()
			d.cache_c = INVALID_CP_CACHE;

#if 0
			app_log << " time toEndOfLine : " << t1 - t0 << "ms nr cp ok  = " << nr_ok << "\n";
#endif

#else
			auto it_end = d.owner->end();
			while (*this != it_end) {
				s32 c = this->operator * ();
				if (c == '\n')
					break;

				this->operator ++ ();
			}
#endif

			return true;
		}

	private:
		friend class text_buffer;

	private:
		class private_data
		{
		public:
			private_data() :
				owner(0),
				line(0),
				column(0),
				cache_c(INVALID_CP_CACHE)
			{
			}

			~private_data()
			{
				owner = 0;
				line = 0;
				column = 0;
				cache_c = INVALID_CP_CACHE;
			}

			// data
			text_buffer * owner;
			buffer::iterator buff_it;
			u64 line;
			u64 column;
			s32 cache_c;
		};
		private_data d;
	};

private:

	// text_buffer::private_data
	class private_data
	{
	public:
		private_data()
		{
			buffer = nullptr;
			line_cache = 0;
			line_column_cache = 0;
			codec = 0;
			th_indexer = 0;
			is_valid = false;
			is_indexed = false;
		}

		~private_data()
		{

			is_valid = false;
			is_indexed = false;

			delete th_indexer;
			delete line_cache;
			delete line_column_cache;
		}

		text_buffer * owner = nullptr;
		ew::core::objects::buffer * buffer = nullptr; // not owned

		// cache
		text_buffer::iterator * line_cache        = nullptr;
		text_buffer::iterator * line_column_cache = nullptr;
		ew::codecs::text::text_codec * codec      = nullptr;

		byte_buffer_id_t     bid = 0;
		buffer_log_id_t log = 0;

		//
		thread * th_indexer = nullptr;

		bool is_valid = false;
		bool is_indexed = false;

		struct find_line {
			u64 line;

			buffer::node * find_node;

			find_line(const u64 _line)
			{
				line = _line;
				find_node = nullptr;
			}

			int operator()(buffer::node * parent, buffer::node ** next_parent)
			{
				assert(parent);
				buffer::node * l = parent->left();
				buffer::node * r = parent->right();

				if (!l) {
					assert(!r);
					find_node = parent;
					return 0;
				}

				text_page_data * pi = static_cast<text_page_data *>(l->get_meta_data());

				if (line < pi->m_nr_new_line) {
					*next_parent = l;
					return -1;
				}

				*next_parent = r;
				line -= pi->m_nr_new_line;
				return 1;
			}
		};

		bool find_start_of_line(u64 line, text_buffer::iterator * out)
		{
			// line 1 is offset(0)
			if (line == 1) {
				out->d.line = 1;
				out->d.column = 1;
				out->d.buff_it = buffer->begin();
				return true;
			}

			//
			if (is_indexed == false) {
				app_log << " buffer is not indexed yet...\n";
				return false;
			}

			buffer::node * root = buffer->root_node();
			text_page_data * pi = static_cast<text_page_data *>(root->get_meta_data());

			u64 max_lines = pi->m_nr_new_line + 1;
			if (line > max_lines) {
				assert(0);
				return false;
			}

			if (line == 0) {
				assert(0);
				return false;
			}

			u64 line_index = line - 1;

			app_log << " line index = " << line_index << "\n";

			find_line fl(line_index);
			fl = buffer->find(fl);
			assert(fl.find_node);

			// look ofr fl.line instance
			buffer::page_type * p =  fl.find_node->page();

			line_index = fl.line;
			app_log << " local line index = " << line_index << "\n";

			u64 local_offset = 0;
			p->map();
			{
				u8 * b = p->begin();
				u8 * e = p->end();
				while (b != e) { // can try while (true)
					if (*b == '\n') {
						--line_index;
					}
					++b;

					if (line_index == 0) {
						break;
					}
				}

				local_offset = b - p->begin();
				app_log << " find line " << line << " @ local offset = " << local_offset << "\n";
			}
			p->unmap();

			u64 final_offset = 0;
			buffer->get_offset_by_node_local_offset(fl.find_node, local_offset, &final_offset);

			app_log << " find line " << line << " @ final offset = " << final_offset << "\n";

			buffer::iterator it;
			buffer->get_iterator_by_offset(final_offset, &it);

			//
			out->d.line = line;
			out->d.column = 1;
			out->d.buff_it = it;

			return true;
		}


		// TODO: update cache
		bool update_line_cache(const text_buffer::iterator * out)
		{
			if (!line_cache) {
				line_cache =  new text_buffer::iterator(*out);
				assert(line_cache->d.line == out->d.line);
			} else if (line_cache) {
				*line_cache = *out;
				assert(line_cache->d.line == out->d.line);
			}

			return true;
		}

		bool find_line_by_offset(u64 offset, u64 * line)
		{
			// get the real line num from a given offset
			*line = 0;
			if (this->is_indexed == false)
				return false;

			buffer::iterator tmp_it;
			buffer->get_iterator_by_offset(offset, &tmp_it);

			return find_line_from_buffer_iterator(tmp_it, line);
		}


		// REMOVE THIS
		bool find_line_from_buffer_iterator(const  buffer::iterator & it, u64 * line)
		{
			buffer::node * n = it.get_node();
			if (n == nullptr) {
				*line = 0;
				return false;
			}

			buffer::page_type * p =  n->page();

			u64 line_index = 0;

			p->map();
			{
				u8 * b = p->begin();
				u8 * cur = it.get_cur_pg_it();

				while (b != cur) {
					if (*b == '\n') {
						++line_index;
					}
					++b;
				}
			}
			p->unmap();

			auto  parent = n->parent();

			while (parent) {
				if (n == parent->right()) {
					auto l = parent->left();
					if (l) {
						text_page_data * pi = static_cast<text_page_data *>(l->get_meta_data());
						line_index += pi->m_nr_new_line;
					}
				}

				n = parent;
				parent = parent->parent();
			}

			line_index++;

			*line = line_index;

			return true;
		}

	};

	private_data d;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

inline byte_buffer_id_t create_buffer(const char * filename)
{
	byte_buffer_id_t bid;
	int  ret = byte_buffer_open(filename, filename, 1, &bid);
	if (ret != 0) {
		return 0;
	}

	app_log << "allocated bid['"<< filename  <<"'] = " << bid << "\n";
	return bid;
}


} // ! namespace eedit
