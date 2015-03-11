#pragma once

#include <iostream>
#include <string.h>
#include <assert.h>
#include <cstdlib>
#include <cstring>

#include <ew/core/types/types.hpp>

using namespace ew::core::types;



/*
 TODO: use c++11 is_pod, is_pointer, etc...
       and delete typeinfo.hpp

  check align

  very buggy
*/

namespace ew
{
namespace core
{
namespace container
{

template <class T>
struct simple_array_base {
	T * _ptr   = nullptr;
	T * _size  = nullptr;
	T * _alloc = nullptr;
	T * _base  = nullptr;

	T * _base_debug  = nullptr;

};

template <typename InputIterator>
inline size_t get_distance(InputIterator start, InputIterator end)
{
	if (! std::is_pointer<InputIterator>()) {
		size_t n = 0;
		InputIterator d = start;
		while (d != end) {
			++d;
			++n;
		}
		return n;
	} else {
		assert(start <= end);
		return (size_t)(end - start); // abs ? ssize_t
	}
}

/*
   TODO : add
   - resize policy
   - alloc policy
   - ref count policy
*/
template <class T>
class simple_array : protected simple_array_base<T>
{
protected:
	using simple_array_base<T>::_ptr;
	using simple_array_base<T>::_size;
	using simple_array_base<T>::_alloc;
	using simple_array_base<T>::_base;
	using simple_array_base<T>::_base_debug;

	size_t _init: 1; // only in debug
	bool   _is_mapped;
public:
	typedef size_t size_type;
	typedef ssize_t difference_type;
	typedef T value_type;
	typedef T * pointer;
	typedef T & reference;
	typedef const T & const_reference;

	typedef pointer iterator;
	typedef const pointer const_iterator;

public:

	/* Creates an empty vector. */
	inline simple_array()
		:
		_init(0),
		_is_mapped(false)
	{
		_init = 1;
	}

	/* Creates a vector with n elements. */
	inline simple_array(size_type n)
		:
		_init(0),
		_is_mapped(false)
	{
		reserve(n);
		_init = 1;
	}

	/* Creates a vector with n copies of t. */
	simple_array(size_type n, const T & t)
		:
		_init(1),
		_is_mapped(false)
	{
		abort();
	}

	/* TODO : update this old code */
	inline simple_array(const simple_array<T> & orig)
		:
		_init(0),
		_is_mapped(false)
	{
		_init = 1;
		*this = orig;
	}

	template <class InputIterator>
	simple_array(InputIterator first, InputIterator last)
		:
		_init(0),
		_is_mapped(false)
	{
		insert(first, last);
		_init = 1;
	}

	/* Destructor */
	virtual inline ~simple_array()
	{
		// std::cerr << __FUNCTION__ << " : this = " << this << "\n";
		release();
	}

	/* iterators */
	inline iterator begin() const
	{
		return _ptr;
	}
	inline iterator end() const
	{
		return _size;
	}

	inline reference front()
	{
		return _ptr[0];
	}
	inline const_reference front() const
	{
		return _ptr[0];
	}

	inline reference back()
	{
		return _size[-1];
	}
	inline const_reference back() const
	{
		return _size[-1];
	}

	inline void push_back(const T & x)
	{
		if (_size != _alloc) {
			if (! std::is_pod<T>()) {
				::new(_size) T(x);
				++_size;
			} else {
				*_size = x;
				++_size;
			}
		} else {
			insert(end(), x);
		}
	}

	inline bool push_front(const T & x)
	{
		return insert(begin(), x);
	}

	/* TODO : update this old code */
	inline simple_array<T> & operator = (const simple_array<T> & orig)
	{
		if (this != &orig) {
			assert(orig._ptr);

			release();
			reserve(orig.allocated());
			insert(begin(), orig.begin(), orig.end());
		}
		return *this;
	}

	inline size_type size() const
	{
		return _size - _ptr;
	}

	inline size_type max_size() const
	{
		return _size - _ptr;
	}

	inline size_type capacity() const
	{
		return _alloc - _ptr;
	}

	inline size_type allocated() const
	{
		return _alloc - _base;
	}

	inline size_type remain() const
	{
		return _alloc - _size;
	}

	inline bool      empty() const
	{
		return size() == 0;
	}

	inline size_type availableSpace() const
	{
		return skipped() + remain();
	}

	inline bool      canInsert(size_type n) const
	{
		return n <= availableSpace();
	}

	/* access */
	inline T & operator[](size_type index)
	{
		assert(index < size());
		return _ptr[index];
	}

	inline const T & operator[](size_type index) const
	{
		assert(index < size());
		return _ptr[index];
	}

	/*
	  will allocate more space (capacity() is smaller than) whitout calling the default ctor on new slots

	  _base
	  _ptr // last
	  _size
	  _alloc
	*/
	inline void reserve(size_type n)
	{

		if (n <= capacity())
			return;

		// assert(size() == 0);

		// call dtor on old elements
		size_type old_size     = size();

		//       std::cerr << __FUNCTION__ << " old_size("<<old_size<<") > try to reserve("<<n<<") ? \n";
		if (old_size > n) {
			// call inplace dtor on ]new_size; old_size] ?
			return;
		}

		// raw memory allocation
		// std::cerr << " allocating new array of "
		//           <<  n << " elements, sizeof (T) = " << sizeof(T) << "\n";

		T * new_array_start = static_cast<T *>(::operator new(n * sizeof(T)));
		T * new_ptr = new_array_start;

		auto p    = begin();
		auto last = end();
		if (! std::is_pod<T>()) {
			while (p != last) {
				new(new_ptr++) T(*p++);
			}

		} else {
			if (old_size) {
				assert(0);
				::memcpy(new_ptr, p, old_size);
			}
		}

		set_data(new_array_start, n, old_size);
	}

	/*

	 */
	inline bool resize(size_type sz)
	{
		if (sz > size()) {
			reserve(sz);
		}

		set_used(sz);
		return true;
	}

	inline bool release()
	{

		if (_is_mapped == false) {
			assert(_init == true);
			erase(begin(), end());

			assert(_base == _base_debug);
			::operator delete((T *)_base);
		}

		_ptr  = _size = _alloc = _base = 0;
		_base_debug = 0;
		_init = false;
		return true;
	}

	inline bool set_data(T * data, size_type alloc_size, size_type item_used = 0, bool __is_mapped = false)
	{
		_init = true;

		if (!release())
			return false;

		_base  = data;
		_base_debug = data;
		_ptr   = _base;
		_size  = _base + item_used;
		_alloc = _base + alloc_size;
		_init  = true;
		_is_mapped = __is_mapped;
		return true;
	}

	inline T * data() const
	{
		assert(_ptr);
		return _ptr;
	}

	inline size_type skipped() const
	{
		return _ptr - _base;
	}

	inline bool skip(size_type nrelm)
	{
		assert(0);

		if (_ptr + nrelm > _size)
			return false;

		_ptr += nrelm;
		// TODO: call dtor on skipped items
		return true;
	}

	inline bool unskip(size_type nrelm)
	{
		if (nrelm > skipped())
			return false;

		_ptr -= nrelm;
		// TODO: call default ctor on new items
		return true;
	}

	/* used by resize */
	inline bool set_used(size_type index)
	{

		T * pindex = _ptr + index;

		assert(pindex <= _alloc);

		if (! std::is_pod<T>()) {
			if (pindex < _size) {
				while (_size != pindex) {
					(--_size)->~T();
				}
			} else {
				while (_size != pindex) {
					new(_size++) T();
				}
			}
		} else {
			_size = pindex;
		}

		return true;
	}

private:
	// next power of 2 : move elsewhere // rename in next_power_of_2(val)
	template <class U>
	U nexthigher(U start, U k)
	{

		if (start == 0)
			start = 1;

		while (start < k)
			start <<= 1;

		return start;
	}

	/* Helpers
	   must check first last range to do backward copy one item at time
	*/
	template <typename InputIterator>
	inline void insert_before(const iterator before, InputIterator first, InputIterator last, const size_type nr_items)
	{
		assert((size_type)(_alloc - _size) >= nr_items);

		if (! std::is_pod<T>()) {

			T * dst = _size + nr_items;
			const T * src = _size;
			const T * src_end = src - nr_items;
			while (src != src_end) {
				new(--dst) T(*--src);
			}
			while (src != before) {
				*--dst = *--src;
			}
		} else {
			// check first last ! in array ?
			::memmove(before + nr_items, before, (_size - before) * sizeof(T));
		}

		if (! std::is_pointer<InputIterator>()) {
			T * dst = before + nr_items;
			InputIterator src = last;
			while (src != first) {
				*--dst = *--src;
			}
		} else {
			// check first last ! in array ?
			::memmove(before, first, nr_items * sizeof(T));
		}
		_size += nr_items;
	}

	template <typename InputIterator>
	inline void insert_alloc(const iterator before, InputIterator first, InputIterator last, const size_type nr_items)
	{
		// allocator
		T * p = static_cast<T *>(::operator new(nr_items * sizeof(T)));
		T * ptr = p;

		if (! std::is_pointer<InputIterator>()) {
			while (first != last) {
				new(ptr++) T(*first++);
			}
		} else {
			assert(0);
			::memmove(ptr, first, nr_items * sizeof(T));
		}

		set_data((T *)p, nr_items, nr_items);
	}

	template <typename InputIterator>
	inline void resize_insert(const iterator before, InputIterator first, InputIterator last, const size_type nr_items)
	{
		// resize + intelligent insert, refactor ?
		size_type current_size = size();
		size_type target_size = current_size + nr_items;
		size_type newsize;
		if (!current_size)
			newsize = target_size;
		else
			newsize = nexthigher(current_size * 2, target_size);

		// allocator
		T * p = static_cast<T *>(::operator new(newsize * sizeof(T)));
		T * ptr = p;
		const T * start = _ptr;
		const T * end = _size;

		if (! std::is_pointer<InputIterator>()) {
			while (start != before) {
				new(ptr++) T(*start++);
			}
			while (first != last) {
				new(ptr++) T(*first++);
			}
			while (start != end) {
				new(ptr++) T(*start++);
			}
		} else {
			size_type n = (before - start);
			assert(0);
			::memmove(ptr, start, n * (sizeof(T)));
			ptr += n;
			::memmove(ptr, first, nr_items * (sizeof(T)));
			ptr += nr_items;
			size_type n2 = end - before;
			::memmove(ptr, before, n2 * (sizeof(T)));
		}
		set_data((T *)p, newsize, target_size);
	}

public:
	template <typename InputIterator>
	inline void insert(const iterator before, InputIterator first, InputIterator last, size_type nr_items)
	{
		assert(before >= _ptr);
		assert(before <= _size);

		if (!_ptr) {
			return insert_alloc(before, first, last, nr_items);
		}

		size_type avail_no_skip = remain();
		if (nr_items <= avail_no_skip) {

			if (before == _size) {

				// insert_at_end(before, first, last, nr_items);
				if (! std::is_pod<T>()) {
					T * dst = before + nr_items;
					InputIterator src = last;
					InputIterator src_end = first;
					while (src != src_end) {
						new(--dst) T(*--src);
					}
				} else {
					// std::move() // ?
					::memmove(before, first, nr_items * sizeof(T));
				}
				_size += nr_items;
				return;
			}
			// before != _size
			insert_before(before, first, last, nr_items);
			return;
		}

		return resize_insert(before, first, last, nr_items);
	}

	template <typename InputIterator>
	inline void insert(const iterator before, InputIterator first, InputIterator last)
	{
		assert(before >= _ptr);
		assert(before <= _size);

		size_type nr_items = get_distance(first, last);
		return insert(before, first, last, nr_items);
	}

	inline size_type insert(const iterator before, const T * items, const size_type nr_items)
	{
		insert(before, items, items + nr_items, nr_items);
		return nr_items;
	}

	inline size_type insert(size_type pg_offset, const T * items, const size_type nr_items)
	{
		iterator it = _ptr + pg_offset;
		return insert(it, items, nr_items);
	}

public:
	inline iterator insert(const iterator before, const T & x)
	{
		size_type off = before - begin();

		assert(_size <= _alloc);
		if (_size != _alloc) {
			if (before == _size) {
				if (! std::is_pod<T>()) {
					new(_size++) T(x);
				} else {
					*_size++ = x;
				}
				return begin() + off;

			} else {
				T copy(x);
				insert_before(before, &copy, &copy + 1, 1);
				return begin() + off;
			}
		} else {
			insert(before, &x, &x + 1, 1);
			return begin() + off;
		}
	}

	inline void insert(size_type offset, const T & item)
	{
		iterator it = _ptr + offset;
		insert(it, item);
	}

	// for now
	inline void insert(iterator pos, size_t n, const T & x)
	{
		size_t off = pos - begin();
		while (n--) {
			insert(off, x);
		}
	}

	inline iterator erase(iterator first, iterator last)
	{
		size_t offset = first - begin();
		size_t len = last - first;

		for (size_type i = offset; i < size(); i++) {

			if ((i + len) < size()) {
				_ptr[i] = _ptr[i + len];
			} else {

				if (! std::is_pod<T>()) {
					_ptr[i].~T();
				}
			}
		}
		_size -= len;

		iterator ret = begin() + offset;
		return ret;
	}

	inline iterator erase(iterator it)
	{
		return erase(it, it + 1);
	}

	inline iterator erase(size_type pos)
	{
		return erase(begin() + pos, begin() + pos + 1);
	}

};


} // ! namespace container
} // ! namespace core
} // ! namespace ew
