#pragma once

#include <unistd.h>
#include <ew/core/types/types.hpp>

#include "default_page.hxx"

namespace ew
{
namespace core
{
namespace container
{

using namespace ew::core::types;

/////////
// Bsp //
/////////

// TODO: list_node<T> { T * prev; T * next } to replace manual list handling...
// list_node_set_next(list_node);
// list_node_set_prev(list_node);

#define _USE_PG_FIRST_LAST_PTR 1
#define __FLEXIBLE_ARRAY_CHECK_ITERATOR__ 1
#define ITERATOR_BASE_HAVE_OFFSET_MEMBER 1   // => for slow ++iterator

template <class T, class P = page<T> >
class flexible_array
{
public:
	typedef T value_type;
	typedef P page_type;

public:
	class node;
	struct node_meta_data { };

	typedef enum { node_insert, node_modified, node_unlink } node_event;
	typedef bool (*node_event_cb)(node * n, node_event ev, void * user_context);
	struct node_event_context { };

	// replace user_context by node_event_context *)
	bool set_node_event_callback(node_event_cb cb, void * user_context)
	{
		on_node_event = cb;
		node_event_user_context = user_context;
		return true;
	}

private:
	node_event_cb on_node_event;
	void * node_event_user_context;

public:
	class iterator_base;
	class iterator;
	class reverse_iterator;

public:

	ALWAYS_INLINE
	bool check_invariants() const
	{
		return true;
	}

public:

	flexible_array(std::size_t max_page_size = 4096, std::size_t min_page_size = 4096);
	virtual ~flexible_array();

	std::size_t min_page_size() const
	{
		return m_min_page_size;
	}

	std::size_t max_page_size() const
	{
		return m_max_page_size;
	}

	virtual const char * filename() const
	{
		return nullptr;
	}
	virtual bool  save(const char * filename)
	{
		return false;
	}

	bool set_initial_partition_size(std::size_t size);
	bool build_partitions();

	void dump();

	//
	node * root_node() const
	{
		return m_root_node;
	}

	// dichotomic walk
	template <class Function>
	Function find(Function fn);

	// linear walk
	template <class Function>
	Function update_hierarchy(node * n, Function fn);

	node * find(u64 offset, u64 & local_offset);

	bool get_iterator_by_offset(u64 off, iterator * pout);

	bool get_offset_by_node_local_offset(node * n, u64 local_offset, u64 * offset)
	{

		if (!n) return false;

		auto p = n->parent();
		while (p) {

			if (n == p->right()) {
				auto l = p->left();
				if (l)
					local_offset += l->size();
			}
			n = p;
			p = p->parent();
		}

		*offset = local_offset;

		return true;
	}

private:
	bool build_iterator_base(u64 off, iterator_base * pout);
	bool increment_hierarchy_size(node * n, size_t m_add_value);
	bool decrement_hierarchy_size(node * n, size_t m_sub_value);


public:
	iterator new_iterator(u64 offset);

	iterator begin();
	iterator end();
	reverse_iterator rbegin();
	reverse_iterator rend();

	inline u64 size() const;

	/* TODO: Begin { */
	inline const T operator[](const u64 i);

	template <typename InputIterator>
	inline bool insert(iterator before, InputIterator first, InputIterator last);

	inline bool insert(iterator before, const T * items, u32 nr_items);

	inline bool insert(iterator before, const T & val);

	inline bool insert(u64 offset, const T * items, u32 nr_items);

	inline bool insert(u64 offset, const T & val);

	inline bool insert(u64 offset, const T * items, u32 nr_items, u64 * nrWritten);

	// NEW TO TEST
	template <typename OutputIterator>
	inline bool copy(iterator first_in, iterator last_in, OutputIterator first_out, OutputIterator last_out, u64 * nr_copied = nullptr);

	// NEW TO TEST
	template <typename OutputIterator>
	inline bool copy(iterator first, iterator last, OutputIterator out);


	inline bool erase(iterator first, iterator last);

	inline bool push_back(const T & val);
	inline bool pop_back(T * val);
	inline bool push_front(const T & val);
	inline bool pop_front(T * val);


	// MOVE AWAY to mapped_file
	inline bool write(u64 offset, const T * items, size_t nr_items, u64 * nrWritten);
	inline bool read(u64 offset, T * items, size_t nr_items, u64 * nrWritten);

	//
	template <class Function >
	inline Function foreach_item(Function fn);

	template <class Function >
	inline Function reverse_foreach_item(Function fn);

	template <class Function >
	inline Function for_each(const iterator & begin, const iterator & end, Function fn);

	template <class Function >
	inline Function foreach_pages(Function fn);

	/* } End */

private:
	/* insert helper */
	template <typename InputIterator>
	inline bool copy_forward(node * cur_node, InputIterator first, InputIterator last, node ** out_node);

	inline bool alloc_sub_tree_pages(node * cur_node);

	inline bool erase_node(node * cur_node);

protected:
	bool split_node(node * n, bool debug = false);
	bool build_partitions(node * n, std::size_t min_page_size, bool debug = false);

	bool build_leafs_list(node * root_node,
			      node ** p_first_n,
			      node ** p_last_n);

	bool unlink_node(node * cur_node);

	// dichotomic walk
	template <class Function>
	Function find(node * n, Function fn);

	node * find(node * root_node, u64 offset, u64 * local_offset = nullptr);

private:
	//
	node * m_root_node;
	node * m_begin_node;
	node * m_end_node;

	// cache
	void clear_cache()
	{
		m_last_node = 0;
		m_last_start_offset = 0;
		m_last_end_offset = 0;
	}

	node * m_last_node;
	u64 m_last_start_offset;
	u64 m_last_end_offset;

	// page size limits
	std::size_t m_min_page_size;
	std::size_t m_max_page_size;
};


////////////////
// Node
////////////////


// TODO: add a revision mecanism
// #ifdef USE_REVISION
// each insert/errase ++m_revision
// and cache last offset() call

template <class T, class P>
class flexible_array<T, P>::node
{
public:

	friend class flexible_array<T, P>;
	friend class flexible_array<T, P>::iterator_base;

	inline node(std::size_t size)
		: m_parent(nullptr)
		, m_left(nullptr)
		, m_right(nullptr)
		, m_prev(nullptr)
		, m_next(nullptr)
		, m_size(size)
		, m_refcount(0)
		, m_page(nullptr)
		, m_stat(nullptr)
	{
	}

	inline virtual void defered_delete()
	{
		assert(m_refcount >= 0);
		m_refcount *= -1;
	}

	inline
	virtual ~node()
	{
		if (m_refcount == 0) {

			delete m_left;
			m_left = nullptr;

			delete m_right;
			m_right = nullptr;

			delete m_page;
			m_page = nullptr;

			delete m_stat;
			m_stat = nullptr;
		}
	}

	ALWAYS_INLINE
	void set_page(P * p)
	{
		m_page = p;
	}

	ALWAYS_INLINE
	P  * page() const
	{
		return m_page;
	}

	ALWAYS_INLINE
	node * parent() const
	{
		return m_parent;
	}

	ALWAYS_INLINE
	void set_parent(node * p)
	{
		m_parent = p;
	}

	ALWAYS_INLINE
	node * left() const
	{
		return m_left;
	}

	ALWAYS_INLINE
	void set_left(node * l)
	{
		m_left = l;
	}

	ALWAYS_INLINE
	node * right() const
	{
		return m_right;
	}

	ALWAYS_INLINE
	void set_right(node * r)
	{
		m_right = r;
	}

	ALWAYS_INLINE
	node * next() const
	{
		return m_next;
	}

	ALWAYS_INLINE
	void set_next(node * n)
	{
		m_next = n;
	}

	ALWAYS_INLINE
	node * prev() const
	{
		return m_prev;
	}

	ALWAYS_INLINE
	void set_prev(node * n)
	{
		m_prev = n;
	}

	ALWAYS_INLINE
	std::size_t size() const
	{
		return m_size;
	}

	ALWAYS_INLINE
	node_meta_data * get_meta_data() const
	{
		return m_stat;
	}

	ALWAYS_INLINE
	void  set_meta_data(node_meta_data * new_stat)
	{
		delete m_stat;
		m_stat = new_stat;
	}

	ALWAYS_INLINE
	u64 offset()
	{
		node * n = this;

		u64 n_off = 0;

		while (n) {

			node * p = n->parent();

			if (p == nullptr)
				break;

			node * p_left  = p->left();
			node * p_right = p->right();

			if (n == p_right) {
				if (p_left) {
					n_off += p_left->size();
				}
			}

			n = p;
		}

		return n_off;
	}

	ALWAYS_INLINE
	void check_invariants()
	{
		if (m_page != nullptr) {
			assert(m_size <= m_page->size());
		}
	}

private:

	void set_size(std::size_t sz)
	{
		m_size = sz;
	}

	// tree
	node     *     m_parent;
	node     *     m_left;
	node     *     m_right;

	// iterator
	node     *     m_prev;
	node     *     m_next;

	//
	std::size_t    m_size;
	s32            m_refcount; // FIXME: use u32 + asserts

	// data
	P       *      m_page;

	// meta data
	node_meta_data  * m_stat;
};



////////////////
// Iterator Base
////////////////

template <class T, class P>
class flexible_array<T, P>::iterator_base
{
	friend class flexible_array<T, P>;
private:

	void unref_current_node()
	{

		if (m_current_node) {

			if (m_current_node->m_refcount >= 0) {
				m_current_node->m_refcount--;

				if (m_current_node->m_refcount == 0) {
					if (m_current_node->page()) {
						m_current_node->page()->unmap();
					}
				}

			} else {

				m_current_node->m_refcount++;
				if (m_current_node->m_refcount == 0) {
					if (m_current_node->page()) {
						m_current_node->page()->unmap();
					}

					delete m_current_node; // real deletion if last iterator
				}
			}
		}
	}

	void ref_current_node()
	{

		if (m_current_node) {
			if (m_current_node->m_refcount == 0) {
				assert(m_current_node->page());
				m_current_node->page()->map();
			}

			m_current_node->m_refcount++;
		}
	}

	void set_node(node * new_node)
	{
		unref_current_node();

		m_current_node = new_node;
		ref_current_node();

		P * pg = m_current_node->page();
		assert(pg);
		set_cur_pg_begin(pg->begin());
		set_cur_pg_end(pg->end());
	}

public:
	iterator_base()
	{
		m_tree = nullptr;
		m_current_node = nullptr;

		set_cur_pg_begin(nullptr);
		set_cur_pg_end(nullptr);
		m_cur = nullptr;
#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER
		m_node_offset = 0;
#endif
	}

	iterator_base(const iterator_base & from)
	{
		m_tree = nullptr;
		m_current_node = nullptr;
		m_cur = nullptr;
		set_cur_pg_begin(nullptr);
		set_cur_pg_end(nullptr);
#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER
		m_node_offset = 0;
#endif
		(*this) = from;
	}

	ALWAYS_INLINE
	bool check_invariants() const
	{
#ifdef __FLEXIBLE_ARRAY_CHECK_ITERATOR__
		if (m_current_node) {

			// std::cerr << "m_node_offset("<< m_node_offset << ") , ";
			// std::cerr << "m_current_node->offset() = " << m_current_node->offset() << "\n";

			assert(m_tree);
			assert(m_current_node->page());

			// std::cerr << "get_cur_pg_begin() = " << (void*)get_cur_pg_begin() << "\n";
			// std::cerr << "get_cur_pg_end() = " << (void*)get_cur_pg_end() << "\n";

			// std::cerr << "m_current_node->page()->begin() = " << (void*)m_current_node->page()->begin()<< "\n";

			// std::cerr << "m_current_node->page()->end() = " << (void*)m_current_node->page()->end()<< "\n";

			assert(get_cur_pg_begin() == m_current_node->page()->begin());
			assert(get_cur_pg_end()   == m_current_node->page()->end());
			assert(m_cur);
			assert(m_cur >= get_cur_pg_begin());

			assert(m_node_offset == m_current_node->offset());

			if (m_current_node->next()) {
				assert(m_cur < get_cur_pg_end());
#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER
				assert(m_node_offset + m_current_node->size() <= m_tree->size());
#endif

			} else {
				// at end
				assert(m_cur <= get_cur_pg_end());
#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER

				// std::cerr << "m_node_offset("<< m_node_offset << ") , ";
				// std::cerr << "m_current_node->size() = " << m_current_node->size() << ", ";
				// std::cerr << "(get_cur_pg_end() - get_cur_pg_begin() = " << (get_cur_pg_end() - get_cur_pg_begin()) << " , ";
				// std::cerr << "m_tree->size() = " << m_tree->size() << "\n";

				// the page can be bigger than the node
				assert(m_node_offset + m_current_node->size() <= m_tree->size());
#endif
			}
		}
#endif
		return true;
	}

	~iterator_base()
	{
		unref_current_node();
		m_tree = nullptr;
		m_current_node = nullptr;
		m_cur = nullptr;
		set_cur_pg_begin(nullptr);
		set_cur_pg_end(nullptr);
	}

	ALWAYS_INLINE
	typename flexible_array<T, P>::iterator_base & operator = (const iterator_base & from)
	{

		/* XXX: as of now
		 * the invariant can be broken
		 * the automatic refresh of iterators must be implemented
		 * to call check_invariants();
		 */

		assert(from.check_invariants());

		if (this == &from) {
			return *this;
		}

		// set_node(from.m_current_node); // is possible the only diff is in fisrt/last init
		{
			unref_current_node();
			m_current_node = from.m_current_node;
			ref_current_node();
			set_cur_pg_begin(from.get_cur_pg_begin());
			set_cur_pg_end(from.get_cur_pg_end());
		}

		m_cur = from.m_cur;
#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER
		m_node_offset = from.m_node_offset;
#endif
		m_tree = from.m_tree;

		assert(check_invariants());

		return *this;
	}

	ALWAYS_INLINE
	bool operator == (const iterator_base & it) const
	{
		assert(check_invariants());
		assert(it.check_invariants());
		return (m_current_node == it.m_current_node) && (m_cur == it.m_cur);
	}

	ALWAYS_INLINE
	bool operator != (const iterator_base & it) const
	{
		assert(check_invariants());
		assert(it.check_invariants());
		return (m_current_node != it.m_current_node) || (m_cur != it.m_cur);
	}

	ALWAYS_INLINE
	bool operator > (const iterator_base & it) const
	{
		assert(check_invariants());
		assert(it.check_invariants());
		return (this->offset() > it.offset());
	}

	ALWAYS_INLINE
	bool operator < (const iterator_base & it) const
	{
		assert(check_invariants());
		assert(it.check_invariants());
		return (this->offset() < it.offset());
	}

	ALWAYS_INLINE
	bool operator >= (const iterator_base & it) const
	{
		assert(check_invariants());
		assert(it.check_invariants());
		return (this->offset() >= it.offset());
	}

	ALWAYS_INLINE
	bool operator <= (const iterator_base & it) const
	{
		assert(check_invariants());
		assert(it.check_invariants());
		return (this->offset() <= it.offset());
	}

	ALWAYS_INLINE
	typename flexible_array<T, P>::iterator_base  &  operator ++()
	{

		assert(check_invariants());

		++m_cur;

		// end of current page ?
		if (m_cur == get_cur_pg_end()) {

			node * next = m_current_node->next();
			if (next) {
#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER
				m_node_offset += get_cur_pg_end() - get_cur_pg_begin();
#endif

				set_node(next);
				m_cur = get_cur_pg_begin();
			}
		}

		assert(check_invariants());
		return *this;
	}

	ALWAYS_INLINE
	typename flexible_array<T, P>::iterator_base  &  operator --()
	{

		assert(check_invariants());

		// begin of current page ?
		if (m_cur == get_cur_pg_begin()) {

			node * prev = m_current_node->prev();
			if (prev) {
				set_node(prev);
				m_cur = get_cur_pg_end();
#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER
				m_node_offset -= get_cur_pg_end() - get_cur_pg_begin();
#endif
			} else {
				// nothing to do
				return *this;
			}
		}

		--m_cur;
		assert(check_invariants());
		return *this;
	}

	ALWAYS_INLINE
	inline iterator_base operator ++ (int)
	{

		assert(check_invariants());

		iterator_base out(*this);
		this->operator ++ ();

		assert(out.check_invariants());
		return out;
	}

	ALWAYS_INLINE
	inline iterator_base operator -- (int)
	{

		assert(check_invariants());

		iterator out(*this);
		this->operator -- ();

		assert(out.check_invariants());

		return out;
	}


	template <class Integer>
	ALWAYS_INLINE
	inline iterator_base & operator += (Integer val)
	{

		/* in current node ? */
		if (((m_cur + val) < get_cur_pg_end()) && ((m_cur + val) >= get_cur_pg_begin())) {
			m_cur +=  val;
			assert(check_invariants());
			return *this;
		}

		u64 off = m_current_node->offset() + (m_cur - get_cur_pg_begin());

		// TODO: check under/overflow
		off += val;

		m_tree->build_iterator_base(off, this);

		assert(check_invariants());
		return *this;
	}


	template <class Integer>
	ALWAYS_INLINE
	inline iterator_base & operator -= (Integer val)
	{
		// check under flow
		return this->operator+=(-val);
	}

	ALWAYS_INLINE
	inline s64  operator - (const iterator & it2)
	{
		assert(check_invariants());
		assert(it2.check_invariants());
		assert(offset() >= it2.offset());
		return (offset() - it2.offset());
	}

	ALWAYS_INLINE
	inline s64  operator + (const iterator & it2)
	{
		assert(check_invariants());
		assert(it2.check_invariants());
		return (offset() + it2.offset());
	}

	ALWAYS_INLINE
	inline node  * get_node() const
	{
		assert(check_invariants());
		return m_current_node;
	}

	ALWAYS_INLINE
	inline T operator * ()
	{
		assert(check_invariants());
		return *m_cur;
	}

	ALWAYS_INLINE
	inline T operator * () const
	{
		assert(check_invariants());
		return *m_cur;
	}

	/* slow !!!! */
	ALWAYS_INLINE
	inline u64 offset() const
	{
		assert(check_invariants());

		if (!m_current_node)
			return 0;

#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER
		assert(m_node_offset == m_current_node->offset()); // force check: TODO: paste in other function
		assert(m_node_offset + (u64)(m_cur - get_cur_pg_begin()) <= m_tree->size());
		return m_node_offset + m_cur - get_cur_pg_begin();
#endif

		if (m_current_node->page()) {
			return m_current_node->offset() + m_cur - get_cur_pg_begin();
		}

		return m_tree->size();
	}

	ALWAYS_INLINE
	inline iterator_base * base()
	{
		assert(check_invariants());
		return this;
	}

	ALWAYS_INLINE
	flexible_array<T, P> * get_buffer() const
	{
		assert(check_invariants());
		return m_tree;
	}

	// rename in ptr() const
	ALWAYS_INLINE
	T * get_cur_pg_it() const
	{
		return m_cur;
	}

private:

	EW_ALWAYS_INLINE void set_cur_pg_begin(T * b)
	{
#ifdef _USE_PG_FIRST_LAST_PTR
		m_pg_begin = b;
#endif
	}

	EW_ALWAYS_INLINE T * get_cur_pg_begin() const
	{
#ifdef _USE_PG_FIRST_LAST_PTR
		return m_pg_begin;
#else
		return m_current_node->page()->begin();
#endif
	}

	EW_ALWAYS_INLINE void set_cur_pg_end(T * e)
	{
#ifdef _USE_PG_FIRST_LAST_PTR
		m_pg_end = e;
#endif
	}

	EW_ALWAYS_INLINE T * get_cur_pg_end() const
	{
#ifdef _USE_PG_FIRST_LAST_PTR
		return m_pg_end;
#else
		return m_current_node->page()->end();
#endif

	}


private:
	flexible_array<T, P> * m_tree;
	flexible_array<T, P>::node * m_current_node;
	T * m_cur;

#ifdef _USE_PG_FIRST_LAST_PTR
	T * m_pg_begin;
	T * m_pg_end;
#endif

#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER
	u64 m_node_offset;
#endif
	/* TODO: update iterator code
	 * when inserting/removing must refresh iterator of the impacted nodes
	 */
};

////////////////
// Iterator
////////////////

template <class T, class P>
class flexible_array<T, P>::iterator : public flexible_array<T, P>::iterator_base
{
	friend class flexible_array<T, P>;
public:

	inline iterator operator + (long val)
	{

		flexible_array<T, P> * m_tree = this->base()->m_tree;

		iterator out;
		bool ret = m_tree->build_iterator_base(this->offset() + val, &out);

		assert(this->check_invariants());

		if (ret) {
			// TODO: to test
		}

		return out;
	}

};

////////////////
// Reverse Iterator : TODO
////////////////

template <class T, class P>
class flexible_array<T, P>::reverse_iterator : public flexible_array<T, P>::iterator_base
{
	friend class flexible_array<T, P>;
public:

	inline reverse_iterator & operator ++ ()
	{
		iterator_base::operator -- ();
		return *this;
	}

	inline reverse_iterator & operator -- ()
	{
		iterator_base::operator ++ ();
		return *this;
	}

	inline reverse_iterator & operator ++ (int)
	{
		reverse_iterator out;
		iterator_base::operator -- ();
		return out;
	}

	inline reverse_iterator & operator -- (int)
	{
		reverse_iterator out;
		iterator_base::operator ++ ();
		return out;
	}
};


//////////////////////
// Implementation
//////////////////////

template <class T,  class P>
inline flexible_array<T, P>::flexible_array(std::size_t max_page_size, std::size_t min_page_size)
{
	assert(min_page_size);
	assert(max_page_size);

	m_min_page_size = min_page_size;
	m_max_page_size = max_page_size;

	if (min_page_size > max_page_size)
		min_page_size = max_page_size;

	m_root_node  = nullptr;
	m_begin_node = nullptr;
	m_end_node   = nullptr;

	m_last_node  = nullptr;
	m_last_start_offset = 0;
	m_last_end_offset = 0;

	set_node_event_callback(nullptr, nullptr);
}

template <class T,  class P>
inline flexible_array<T, P>::~flexible_array()
{
	delete m_root_node;

	m_root_node  = nullptr;
	m_begin_node = nullptr;
	m_end_node   = nullptr;

	m_last_node  = nullptr;
	m_last_start_offset = 0;
	m_last_end_offset = 0;

}

template <class T,  class P>
inline bool flexible_array<T, P>::set_initial_partition_size(std::size_t size)
{
	node * n = new node(size);
	m_root_node = n;
	return true;
}

template <class T,  class P>
inline bool flexible_array<T, P>::build_partitions()
{
	assert(m_root_node->parent() == nullptr);

	bool ret = build_partitions(m_root_node, m_max_page_size);

	assert(m_root_node->parent() == nullptr);

	// build leafs list
	ret =  build_leafs_list(m_root_node, &m_begin_node, &m_end_node);
	return ret;
}

template <class T,  class P>
inline bool flexible_array<T, P>::build_leafs_list(node * root_node, node ** p_first_n, node ** p_last_n)
{
	node * first_n = nullptr;
	node * last_n = nullptr;

	u64 off = 0;

	node * n = find(root_node, off);
	first_n = n;
	last_n  = n;

	if (n) {
		off = n->size();
	}

	while (n) {
		n = find(root_node, off);

		if (!n) {
			break;
		}

		last_n->set_next(n);
		n->set_prev(last_n);
		last_n = n;
		off    += n->size();
	}

	*p_first_n = first_n;
	*p_last_n = last_n;

	return true;
}

// TODO: min_page_size as parameter
// remove usage of m_max_page_size replace by min page size
//
template <class T,  class P>
inline bool flexible_array<T, P>::build_partitions(node * n, std::size_t min_page_size, bool debug)
{
	if (n->size() <= min_page_size) {
		return true;
	}

	u64 half = n->size() / 2;
	u64 sz = min_page_size;

	while (sz < half) {
		sz += min_page_size;
	}

	std::size_t pg_left_size = sz;
	assert(pg_left_size % min_page_size == 0); // imposible

	std::size_t pg_right_size = n->size() - pg_left_size;

	assert(pg_left_size + pg_right_size == n->size());

	node * left_n = new node(pg_left_size);
	left_n->set_parent(n);
	n->set_left(left_n);
	assert(left_n == n->left());

	if (pg_right_size) {
		node * right_n = new node(pg_right_size);
		right_n->set_parent(n);
		n->set_right(right_n);
		assert(right_n == n->right());
	}

	if (pg_left_size > min_page_size) {
		build_partitions(n->left(), min_page_size);
	}

	if (pg_right_size > min_page_size) {
		build_partitions(n->right(), min_page_size);
	}

	return true;
}

// TODO: pass max page as param
template <class T,  class P>
inline bool flexible_array<T, P>::split_node(node * n, bool debug)
{
	assert(m_min_page_size);
	assert(m_max_page_size);

	assert(n);
	assert(n->left() == nullptr);
	assert(n->right() == nullptr);
	assert(n->prev() == nullptr);
	assert(n->next() == nullptr);

	if (n->size() <= (m_max_page_size / 2)) {
		return true;
	}

	if (n->size() <= (m_min_page_size)) {
		return true;
	}

	u64 half = n->size() / 2;

	std::size_t pg_left_size = half;
	std::size_t pg_right_size = n->size() - half;

	if (pg_left_size == 0) {
		std::swap(pg_left_size, pg_right_size);
	}

	assert(pg_left_size + pg_right_size == n->size());

	node * l = new node(pg_left_size);
	n->set_left(l);
	l->set_parent(n);
	split_node(n->left());

	if (pg_right_size) {

		node * r = new node(pg_right_size);
		n->set_right(r);
		r->set_parent(n);

		split_node(n->right());
	}

	return true;
}

template <class T,  class P>
inline void flexible_array<T, P>::dump()
{
	/* TODO: infix walk + dump fn ? */
}


template <class T,  class P>
template <class Function>
inline Function flexible_array<T, P>::find(node * n, Function fn)
{
	if (n == nullptr)
		return fn;

	while (n) {
		int cmp_ret = fn(n, &n);

		if (cmp_ret == 0)
			break;
	}

	return fn;
}

template <class T,  class P>
template <class Function>
inline Function flexible_array<T, P>::find(Function fn)
{
	return find(m_root_node, fn);
}

template <class T,  class P>
inline typename flexible_array<T, P>::node * flexible_array<T, P>::find(u64 offset, u64 & local_offset)
{
	typename flexible_array<T, P>::node * n;

	// cache
	if (m_last_node != nullptr) {

		if ((offset >= m_last_start_offset) && (offset < m_last_end_offset)) {
			n =  m_last_node;
			local_offset = offset - m_last_start_offset;
			return n;
		}
	}

	n = find(m_root_node, offset, &local_offset);

	if (n) {
		// update cache
		m_last_node    = n;
		m_last_start_offset = offset - local_offset;
		m_last_end_offset   = m_last_start_offset + n->size();
	}

	return n;
}

template <class T,  class P>
inline typename flexible_array<T, P>::node * flexible_array<T, P>::find(node * root_node, u64 offset, u64 * local_offset)
{
	struct walk_function_object_s {

		typename flexible_array<T, P>::node * m_root_node;
		u64    m_local_offset;
		node * m_find_node;

		walk_function_object_s(node * _root_node, const u64 _offset)
		{
			m_root_node    = _root_node;
			m_local_offset = _offset;
			m_find_node    = nullptr;
		}

		~walk_function_object_s()
		{
		}

		int operator()(node * parent, node ** next_parent)
		{

			assert(parent);

			node * l = parent->left();
			node * r = parent->right();

			assert((l == nullptr) || (l->parent() == parent));
			assert((r == nullptr) || (r->parent() == parent));

			if (!l) {
				assert(!r);
				this->m_find_node = parent;

				assert((parent == this->m_root_node) || (parent->parent() != nullptr));
				return 0;
			}


			if (this->m_local_offset < l->size()) {
				*next_parent = l;
				assert(l->parent() == parent);
				return -1;
			}

			*next_parent = r;
			assert(r->parent() == parent);
			this->m_local_offset -= l->size();
			return 1;
		}
	};

	if (root_node == nullptr)
		return nullptr;

	if (offset >= root_node->size())
		return nullptr;

	walk_function_object_s find_obj(root_node, offset);
	find_obj = find(root_node, find_obj);

	if (local_offset) {
		*local_offset = find_obj.m_local_offset;
	}

	return find_obj.m_find_node;
}

// linear walk
template <class T,  class P>
template <class Function>
inline Function flexible_array<T, P>::update_hierarchy(node * n, Function fn)
{
	while (n) {
		fn(n);
		n = n->parent();
	}

	return fn;
}


template <class T,  class P>
inline bool flexible_array<T, P>::get_iterator_by_offset(u64 off, typename flexible_array<T, P>::iterator * pout)
{
	return build_iterator_base(off, pout);
}

template <class T,  class P>
inline bool flexible_array<T, P>::build_iterator_base(u64 off, typename flexible_array<T, P>::iterator_base * pout)
{
	assert(pout);

	if (off >= this->size()) {
		*pout = end();
		return false;
	}

	pout->m_tree = this;

	u64 local_off = 0;

	typename flexible_array<T, P>::node * n = this->find(off, local_off);
	assert(n);

	if (n != pout->m_current_node) {
		pout->unref_current_node();
		pout->m_current_node = n;
		pout->ref_current_node();
	}

	P * pg = n->page();
	assert(pg);
	pout->m_cur   = pg->begin() + local_off;
	pout->set_cur_pg_begin(pg->begin());
	pout->set_cur_pg_end(pg->end());

#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER
	pout->m_node_offset = off - local_off;
#endif

	assert(pout->check_invariants());

	return true;
}




template <class T,  class P>
inline typename flexible_array<T, P>::iterator    flexible_array<T, P>::new_iterator(const u64 offset)
{
	iterator out;

	this->build_iterator_base(offset, &out);
	assert(check_invariants());
	return out;
}

// TODO: construct by move
template <class T,  class P>
inline typename flexible_array<T, P>::iterator    flexible_array<T, P>::begin()
{
	typename flexible_array<T, P>::iterator it;
	typename flexible_array<T, P>::node * n = m_begin_node;

	it.m_tree = this;
	if (n) {
		assert((m_root_node == n) || n->parent());
		it.set_node(n);
		it.m_cur = it.get_cur_pg_begin();
	}
#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER
	it.m_node_offset = 0;
#endif
	assert(it.check_invariants());
	return it;
}


template <class T,  class P>
inline typename flexible_array<T, P>::iterator    flexible_array<T, P>::end()
{
	typename flexible_array<T, P>::iterator it;
	typename flexible_array<T, P>::node * n = m_end_node;

	it.m_tree = this;
	if (n) {
		assert((m_root_node == n) || n->parent());
		it.set_node(n);
		it.m_cur = it.get_cur_pg_end();
	}

#ifdef ITERATOR_BASE_HAVE_OFFSET_MEMBER
	if (n) {
		assert(n->size() <= this->size());
		it.m_node_offset = this->size() - n->size();
	} else {
		it.m_node_offset = this->size();
	}

	assert(it.m_node_offset <= it.m_tree->size());
#endif

	assert(it.check_invariants());
	return it;
}


template <class T,  class P>
inline typename flexible_array<T, P>::reverse_iterator    flexible_array<T, P>::rbegin()
{
	assert(0);
	abort();

	typename flexible_array<T, P>::reverse_iterator it;

	return it;
}


template <class T,  class P>
inline typename flexible_array<T, P>::reverse_iterator    flexible_array<T, P>::rend()
{
	assert(0);
	abort();

	typename flexible_array<T, P>::reverse_iterator it;

	return it;
}


template <class T,  class P>
inline u64    flexible_array<T, P>::size() const
{
	if (m_root_node == nullptr) {
		return 0;
	}

	return m_root_node->size();
}


template <class T,  class P>
inline const T flexible_array<T, P>::operator[](const u64 i)
{
	T t;

	assert(i < size());

	u64 local_offset = 0;
	flexible_array<T, P>::node * n = find(i, local_offset);
	assert(n);
	P * p = n->page();
	p->map();
	{
		t = *(p->begin() + local_offset);
	}
	p->unmap();

	return t;
}

template <class T,  class P>
inline bool  flexible_array<T, P>::push_back(const T & val)
{
	return insert(end(), &val, &val + 1);
}


template <class T,  class P>
inline bool  flexible_array<T, P>::pop_back(T * val)
{
	*val = *end();
	return erase(end() - 1, end());
}

template <class T,  class P>
inline bool  flexible_array<T, P>::push_front(const T & val)
{
	return insert(begin(), &val, &val + 1);
}

template <class T,  class P>
inline bool  flexible_array<T, P>::pop_front(T * val)
{
	*val = *begin();
	return erase(begin(), begin() + 1);
}


/* helper */
template <class T,  class P>
inline bool flexible_array<T, P>::alloc_sub_tree_pages(node * cur_node)
{
	static int debug = 1;

	P * cur_pg = nullptr;
	size_t index = 0;

	while (cur_node) {

		cur_pg = new P(/* cur_node->size() , */ m_max_page_size);
		cur_node->set_page(cur_pg);

		if (debug) {
			std::cerr << " alloc(" << index << ") , n = " << cur_node << ", ";
			std::cerr << " , n->size() = " << cur_node->size() << ", ";
			std::cerr << " , cur_pg->size() = " << cur_pg->size() << ", ";
			std::cerr << " , cur_pg->avail() = " << cur_pg->availableSpace() << "\n";
		}

		index++;
		cur_node = cur_node->next();
	}

	return true;
}


/* copy helper */
template <class T,  class P>
template <typename InputIterator>
inline bool flexible_array<T, P>::copy_forward(node * cur_node, InputIterator first, InputIterator last, node ** out_node)
{
	assert(first <= last);

	size_t remain = std::distance(first, last);

	node * next = cur_node;

	while (first < last) {
		cur_node = next;
		assert(cur_node);

		P * cur_pg   = cur_node->page();

		size_t node_size = cur_node->size();
		size_t pg_size = cur_pg->size();

		size_t copy_size = 0;
		if (node_size >= pg_size) {
			// the node is bigger than the page
			copy_size = node_size - pg_size;
		} else {
			// the page is bigger than the node
			copy_size = pg_size - node_size;
		}
		copy_size = std::min(remain, copy_size);
		assert(copy_size); // ?
		cur_pg->insert(cur_pg->end(), first, first + copy_size);

		remain -= copy_size;
		first += copy_size;
		next = cur_node->next();
	}

	*out_node = cur_node;

	return true;
}

#if 0

// could be implemented using for each

template <class T,  class P>
template <typename InputIterator>
inline bool flexible_array<T, P>::dump_nodes(node  * n)
{
	size_t index = 0;

	while (n) {
		P * cur_pg = n->page();

		if (debug) {
			std::cerr << " cur_node(" << index << ") , n = " << n << ", ";
			std::cerr << " cur_pg->size() = " << n->size()
				  << ", cur_pg->avail() = " << n->availableSpace() << "\n";
		}

		n = n->next();
		++index;
	}
}

#endif

template <class T,  class P>
template <typename InputIterator>
inline bool flexible_array<T, P>::insert(iterator before, InputIterator first, InputIterator last)
{
	before.check_invariants();

	clear_cache();

	P   *  pg = nullptr;
	node * n  = nullptr;

	size_t nr_items = std::distance(first, last);
	size_t new_size = nr_items;
	size_t avail = 0;
	u64 local_offset = 0;

	if (!nr_items) {
		return true;
	}

	if (before.m_current_node) {
		pg = before.m_current_node->page();

		if (pg) {
			new_size += pg->size();
			avail = pg->availableSpace();

			if (before.m_cur)
				local_offset = before.m_cur - pg->begin();
		}
	}

	// enough space in current node ?
	// std::cerr << "avail(" << avail << ") >= nr_items(" << nr_items << ") ?\n";

	if (avail >= nr_items) {
		pg->insert(pg->begin() + local_offset, first, last);

		increment_hierarchy_size(before.m_current_node, nr_items);

		assert(m_begin_node);
		assert(m_end_node);
		begin().check_invariants();
		end().check_invariants();

		if (this->on_node_event) {
			this->on_node_event(before.m_current_node, node_modified, node_event_user_context);
		}
		return true;
	}

	// no enough space in current node , prepare new tree
	node * sub_tree = new node(new_size);
	// reserve nodes
	bool ret = split_node(sub_tree);

	if (ret == false) {
		assert(0);
	}

	node * sub_tree_first_n = nullptr;
	node * sub_tree_last_n = nullptr;

	ret = build_leafs_list(sub_tree, &sub_tree_first_n, &sub_tree_last_n);

	// prepare insert blocks
	size_t left_part_size;

	if (pg) {
		left_part_size = before.m_cur - pg->begin();
		assert(left_part_size <= pg->size());

	} else {
		left_part_size = 0;
	}

	alloc_sub_tree_pages(sub_tree_first_n);

	// copy
	node * cur_node = sub_tree_first_n;

	if (pg) {
		assert(pg->begin());
		copy_forward(cur_node,
			     pg->begin(), pg->begin() + left_part_size,
			     &cur_node);
	}

	copy_forward(cur_node,
		     first, last,
		     &cur_node);

	if (pg) {
		copy_forward(cur_node,
			     before.m_cur,
			     pg->end(),
			     &cur_node);
	}


	//   assert((before.m_current_node == nullptr) || (before.m_current_node->size() + nr_items == sub_tree->size()));


	// TODO: create helper function HERE for clarity
	// replace_node(before.m_current_node, sub_tree)
	// replace_node(n, new)
	// save
	n = before.m_current_node;

	node * p_parent = nullptr;
	node * p_prev   = nullptr;
	node * p_next   = nullptr;
	node * p_left   = nullptr;

	if (n) {
		p_parent   = n->parent();
		p_prev   = n->prev();
		p_next   = n->next();

		assert(n->left() == nullptr);
		assert(n->right() == nullptr);
	}

	if (p_parent) {
		p_left = p_parent->left();
	}


	if (!p_parent) {

		/* set new root */
		m_root_node = sub_tree;
		m_begin_node = sub_tree_first_n;
		m_end_node = sub_tree_last_n;

	} else {

		if (m_begin_node == n) {
			m_begin_node = sub_tree_first_n;
		}

		if (m_end_node == n) {
			m_end_node = sub_tree_last_n;
		}

		if (p_left == n) {
			p_parent->set_left(sub_tree);

		} else {
			p_parent->set_right(sub_tree);
		}
	}

	if (p_prev) {
		p_prev->set_next(sub_tree_first_n);
		sub_tree_first_n->set_prev(p_prev);
	}

	if (p_next) {
		p_next->set_prev(sub_tree_last_n);
		sub_tree_last_n->set_next(p_next);
	}

	if (sub_tree_first_n == sub_tree_last_n)
		sub_tree_first_n->set_parent(p_parent);


	sub_tree->set_parent(p_parent);

	increment_hierarchy_size(p_parent, nr_items);

	/* XXX: the iterator destructor or operator = WILL delete the old node */
	if (n) {
		assert(n->m_refcount > 0);
		n->defered_delete();

		// notify user of node removal
		if (this->on_node_event) {
			this->on_node_event(n, node_unlink, node_event_user_context);
		}
	}

	// notify user of new node insertion
	if (this->on_node_event) {
		node * tmp_node = sub_tree_first_n;
		node * tmp_node_end = sub_tree_last_n->next();
		while (tmp_node != tmp_node_end) {
			this->on_node_event(tmp_node, node_insert, node_event_user_context);
			tmp_node = tmp_node->next();
		}
	}

	assert(m_begin_node);
	assert(m_end_node);

	assert(begin().check_invariants());
	assert(end().check_invariants());

	m_last_node = nullptr; // invalidate cache

	return true;
}

template <class T,  class P>
inline bool flexible_array<T, P>::increment_hierarchy_size(node * n, size_t add_value)
{
	struct increment_node_size_t {

		size_t m_add_value;

		increment_node_size_t(size_t add_value)
			: m_add_value(add_value)
		{
		}

		bool operator()(node * n)
		{
			n->set_size(n->size() + this->m_add_value);
			return true;
		}
	};

	update_hierarchy(n, increment_node_size_t(add_value));

	return true;
}

template <class T,  class P>
inline bool flexible_array<T, P>::decrement_hierarchy_size(node * n, size_t sub_value)
{
	struct decrement_node_size_t {

		size_t m_sub_value;

		decrement_node_size_t(size_t sub_value)
			: m_sub_value(sub_value)
		{
		}

		bool operator()(node * n)
		{
			n->set_size(n->size() - this->m_sub_value);
			return true;
		}
	};

	update_hierarchy(n, decrement_node_size_t(sub_value));

	return true;
}

template <class T,  class P>
inline bool flexible_array<T, P>::insert(iterator before, const T * items, u32 nr_items)
{
	return insert(before, items, items + nr_items);
}

template <class T,  class P>
inline bool flexible_array<T, P>::insert(iterator before, const T & val)
{
	return insert(before, &val, &val + 1);
}

template <class T,  class P>
inline bool flexible_array<T, P>::insert(u64 offset, const T * items, u32 nr_items)
{
	iterator before;
	build_iterator_base(offset, &before);
	return insert(before, items, items + nr_items);
}

template <class T,  class P>
inline bool flexible_array<T, P>::insert(u64 offset, const T & val)
{
	iterator before;
	build_iterator_base(offset, &before);
	return insert(before, &val, &val + 1);
}

template <class T,  class P>
inline bool flexible_array<T, P>::insert(u64 offset, const T * items, u32 nr_items, u64 * nrWritten)
{
	*nrWritten = nr_items;
	iterator before;
	build_iterator_base(offset, &before);
	return insert(before, items, items + nr_items);
}

template <class T,  class P>
inline bool flexible_array<T, P>::unlink_node(node * cur_node)
{
	assert(cur_node);

	node * prev = cur_node->prev();
	node * next = cur_node->next();

	if (prev)
		prev->set_next(next);

	if (next)
		next->set_prev(prev);

	assert(cur_node->left() == nullptr);
	assert(cur_node->right() == nullptr);

	cur_node->set_parent(nullptr);

	return true;
}


/*
         [root...]


         [root]
        /      \
       O        O


         [root]
        /      \
       O        O
      / \
     O   O


         [root]
        /      \
       O        O
               / \
              O   O

*/

template <class T,  class P>
inline bool flexible_array<T, P>::erase_node(node * cur_node)
{

	m_last_node = nullptr; // invalidate cache


	static int debug = 0;

	assert(cur_node);

	node * parent = cur_node->parent();
	P * cur_pg = cur_node->page();

	size_t nr_erased = cur_pg->size();

	if (debug) {
		std::cerr << " Before erase tree size = " << this->size() << "\n";
	}

	if (m_begin_node == cur_node) {
		m_begin_node = cur_node->next();
	}

	if (m_end_node == cur_node) {
		m_end_node = cur_node->prev();
	}

	if (this->on_node_event) {
		this->on_node_event(cur_node, node_unlink, node_event_user_context);
	}

	if (parent) {

		// unlink node
		node * new_parent;
		node * l = parent->left();

		if (cur_node == l) {
			new_parent = parent->right();
			parent->set_left(nullptr);

		} else {
			new_parent = l;
			parent->set_right(nullptr);
		}

		node * gparent = parent->parent();

		if (gparent) {

			if (gparent->left() == parent) {
				gparent->set_left(new_parent);

			} else {
				gparent->set_right(new_parent);
			}

			new_parent->set_parent(gparent);

			decrement_hierarchy_size(gparent, nr_erased);

		} else {

			node * old_root = m_root_node;

			old_root->set_left(nullptr);
			old_root->set_right(nullptr);
			unlink_node(old_root);

			m_root_node = new_parent;
			m_root_node->set_parent(nullptr);

		}

	} else {

		node * old_root = m_root_node;

		old_root->set_left(nullptr);
		old_root->set_right(nullptr);
		unlink_node(old_root);

		m_root_node  = nullptr;
	}

	unlink_node(cur_node);

	// force page unref
	cur_pg->unmap();

	if (debug) {
		std::cerr << " After erase tree size = " << this->size() << "\n";
	}

	return true;
}

// not tested
template <class T,  class P>
template <typename OutputIterator>
inline bool flexible_array<T, P>::copy(iterator first_in, iterator last_in, OutputIterator first_out, OutputIterator last_out, u64 * nr_copied)
{
	u64 count = 0;
	while ((first_in != last_in) && (first_out != last_out)) {
		*first_out = *first_in;

		++first_out;
		++first_in;

		// remove from inner loop use diff
		++count;
	}

	if (nr_copied) {
		*nr_copied = count;
	}

	return true;
}


// not tested
template <class T,  class P>
template <typename OutputIterator>
inline bool flexible_array<T, P>::copy(iterator first, iterator last, OutputIterator out)
{
	assert(first <= last);
	while (first != last) {
		*out = *first;
		++out;
		++first;
	}

	return true;
}


template <class T,  class P>
inline bool flexible_array<T, P>::erase(iterator first, iterator last)
{
	clear_cache();

	// assert(last >= first);

	size_t nr_items_to_erase = last.offset() - first.offset();

	node * cur_node = first.m_current_node;
	u64 local_offset = first.m_cur - cur_node->page()->begin();

	while (nr_items_to_erase) {
		assert(cur_node);
		node * next   = cur_node->next();

		P * cur_pg = cur_node->page();

		size_t nr_erased = 0;
		size_t pg_size   = cur_pg->size();
		size_t pg_remain = pg_size - local_offset;


		/* kill node ? */
		if ((local_offset == 0) && (nr_items_to_erase >= pg_size)) {
			// update parents size before actual removal
			nr_erased = pg_size;

			erase_node(cur_node);

		} else {


			if (nr_items_to_erase > pg_remain) {
				nr_erased = pg_remain;

			} else {
				nr_erased = nr_items_to_erase;
			}

			// copy on write
			cur_pg->copyOnWrite();
			cur_pg->erase(cur_pg->begin() + local_offset, cur_pg->begin() + local_offset + nr_erased);

			decrement_hierarchy_size(cur_node, nr_erased);

			if (this->on_node_event) {
				this->on_node_event(cur_node, node_modified, node_event_user_context);
			}

		}

		nr_items_to_erase -= nr_erased;
		local_offset = 0;

		cur_node = next;
	}

	return true;
}


// NOT GOOD USE template version
template <class T,  class P>
inline bool flexible_array<T, P>::read(u64 offset, T * items, size_t nr_items, u64 * nrWritten)
{
	iterator first;
	build_iterator_base(offset, &first);

//    iterator last = first + nr_items;
	u64 nr_read = 0;
	T * items_end = items + nr_items;

	while (items != items_end) {
		*items = *first;
		++items;
		++first;
		++nr_read;
	}

	*nrWritten = nr_read;
	return true;
}

template <class T,  class P>
inline bool flexible_array<T, P>::write(u64 offset, const T * items, size_t nr_items, u64 * nrWritten)
{
	iterator before;
	build_iterator_base(offset, &before);
	bool ret = insert(before, items, items + nr_items);
	*nrWritten = nr_items;
	return ret;
}

template <class T,  class P>
template <class Function >
inline Function flexible_array<T, P>::reverse_foreach_item(Function fn)
{
	return fn;
}

template <class T,  class P>
template <class Function>
inline Function flexible_array<T, P>::for_each(const iterator & begin, const iterator & end, Function fn)
{
	assert(begin.offset() <= end.offset());

#if 0
	iterator cur(begin);
	while (cur != end) {

		if (fn(*cur) == false) {
			break;
		}

		++cur;
	}

	return fn;

#else

	// will be faster...

	node * n   = begin.m_current_node;
	node * end_node = end.m_current_node;
	u64 off = begin.m_cur - begin.get_cur_pg_begin();
	bool func_ret = false;

	while (n) {
		n->page()->map();
		{
			T * pg_it  = n->page()->begin() + off;
			T * pg_end = n->page()->end();

			while (pg_it != pg_end) {
				func_ret = fn(*pg_it);
				if (func_ret == false) {
					break;
				}
				++pg_it;
			}
		}
		n->page()->unmap();

		if (n == end_node) {
			break;
		}

		if (func_ret == false) {
			break;
		}

		n = n->next();
		off = 0;
	}

	return fn;
#endif
}

template <class T,  class P>
template <class Function >
inline Function flexible_array<T, P>::foreach_item(Function fn)
{
	return for_each(begin(), end(), fn);
}

template <class T,  class P>
template <class Function >
inline Function flexible_array<T, P>::foreach_pages(Function fn)
{
	node * n   = m_begin_node;
	node * end_node = m_end_node;

	bool func_ret = false;

	while (n) {
		n->page()->map();
		{
			T * pg_begin  = n->page()->begin();
			T * pg_end = n->page()->end();
			func_ret = fn(pg_begin, pg_end);
		}
		n->page()->unmap();

		if (n == end_node) {
			break;
		}

		if (func_ret == false) {
			break;
		}

		n = n->next();
	}

	return fn;
}

///////////////////////////////////////////////////////////////////////////////////////

// todo create named_buffer ?
// virtual ~filename() -> nullptr by default
// use it as base class for mapped_file
// or allow mapped_file of non existant file ?
//

// put in NEW file ...

//#include "ew/filesystem/mapped_file.hpp"
//using namespace ew::filesystem;


} // ! namespace container
} // ! namespace core
} // ! namespace ew
