#include <assert.h>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <vector>
#include <bitset>

#include <unistd.h>
#include <cassert>
#include <iostream>
#include <cstring>
#include <list>
#include <unistd.h>

#include <ew/core/container/flexible_array.hpp>

#include <ew/core/object/buffer.hpp>
using namespace ew::core::objects;

#include <ew/filesystem/file.hpp>
using namespace ew::filesystem;

#include <ew/Maths.hpp>

#include <ew/core/Time.hpp>


#define USE_BITSET 1

//#define HACK_TERA_FILE 1

// #define DISABLE_LINE_COUNT 1

namespace test
{
namespace core
{
namespace container
{

using ew::core::container::flexible_array;

typedef ew::core::container::memory_mapped_page<u8> mpage;

typedef ew::core::container::page<u8> page;


class  flexarray_file : public flexible_array<u8, page>
{
public:
	flexarray_file(std::size_t max, std::size_t min)
		: flexible_array<u8, page>(max, min)
	{

		set_node_event_callback(on_node_event, this);
	}

	static bool on_node_event(node * n, node_event ev, void * priv)
	{
		class  flexarray_file * flex = static_cast<flexarray_file *>(priv);

		switch (ev) {
		case node_unlink: {
			return flex->on_node_unlink(n);
		}
		case node_insert: {
			return flex->on_node_insert(n);
		}
		case node_modified: {
			return flex->on_node_modified(n);
		}
		}

		return false;
	}


	virtual bool on_node_unlink(node * n);
	virtual bool on_node_insert(node * n);
	virtual bool on_node_modified(node * n);
};



/* TODO: add other ops : - , + */

static int nr_page_info = 0;

struct page_info : public flexarray_file::node_meta_data {
	std::size_t   m_nr_eol = 0;

	page_info()
	{
		reset();
		++nr_page_info;
	}

	virtual ~page_info()
	{
	}

	virtual void reset()
	{
		m_nr_eol = 0;
	}



#ifdef USE_BITSET
	std::bitset<256> byte_flag; // to update in ops ? failed search ?
#endif
	page_info & operator += (const page_info & to_add)
	{

		this->m_nr_eol += to_add.m_nr_eol;
		return *this;
	}

	page_info & operator -= (const page_info & to_sub)
	{
		assert(this->m_nr_eol >= to_sub.m_nr_eol);
		this->m_nr_eol -= to_sub.m_nr_eol;
		return *this;
	}

	void dump()
	{
		std::cerr << " page info : nr_eol = " << m_nr_eol << "\n";
	}

};


/////////////////////////////////////////////////////////////////////////////////


void index_node(flexarray_file::node * n, u64 * nr_lines)
{
	page_info * pi = static_cast<page_info *>(n->get_meta_data());

	if (pi == nullptr) {
		pi = new page_info;
		n->set_meta_data((flexarray_file::node_meta_data *)pi);
	}

	page * p = n->page();

	bool ret = p->map();
	assert(ret == true);
	if (ret == false) {

	}

	u8 * b = p->begin();
	u8 * e = p->end();
	u32  nr_eol = 0;

	// std::cerr << "node(" << n << ") size = " << (e - b) << "\n";

	while (b != e) {

		if (*b == '\n') {
			++nr_eol;
		}

#ifndef DISABLE_LINE_COUNT


#endif
		++b;
	}

	ret = p->unmap();
	assert(ret == true);

#ifndef DISABLE_LINE_COUNT
	pi->m_nr_eol = nr_eol;
#endif

	//std::cerr << "  nr indexed line = " << nr_eol << "...\n";



	*nr_lines += nr_eol;
}


bool flexarray_file::on_node_unlink(node * n)
{
	page_info * n_pi = static_cast<page_info *>(n->get_meta_data());
	node * p = n->parent();
	while (p) {
		page_info * pi = static_cast<page_info *>(p->get_meta_data());

		*pi -= *n_pi;

		p = p->parent();
	}
	return true;

}

bool flexarray_file::on_node_insert(node * n)
{
	u64 nr_lines = 0;
	index_node(n, &nr_lines);

//  std::cerr << __PRETTY_FUNCTION__ << " n = " << n << ", index " << nr_lines << " lines\n";

	page_info * n_pi = static_cast<page_info *>(n->get_meta_data());
	auto p = n->parent();
	while (p) {
		page_info * pi = static_cast<page_info *>(p->get_meta_data());

		if (pi == nullptr) {
			pi = new page_info;
			p->set_meta_data((flexarray_file::node_meta_data *)pi);
		}

		*pi += *n_pi;

		//  std::cerr << __PRETTY_FUNCTION__ << " p = " << p << ", index " << pi->m_nr_eol << " lines\n";


		p = p->parent();
	}


	return true;
}

bool flexarray_file::on_node_modified(node * n)
{
	on_node_unlink(n);
	on_node_insert(n);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////

#ifndef DISABLE_LINE_COUNT

struct find_line {
	u64 line;

	flexarray_file::node * find_node;

	find_line(const u64 _line)
	{
		line = _line;
		find_node = nullptr;
	}

	int operator()(flexarray_file::node * parent, flexarray_file::node ** next_parent)
	{
		assert(parent);
		flexarray_file::node * l = parent->left();
		flexarray_file::node * r = parent->right();

		if (!l) {
			assert(!r);
			find_node = parent;
			return 0;
		}

		page_info * pi = static_cast<page_info *>(l->get_meta_data());

		if (line < pi->m_nr_eol) {
			*next_parent = l;
			return -1;
		}

		*next_parent = r;
		line -= pi->m_nr_eol;
		return 1;
	}
};


#endif



// TODO: find a way to pass index callback to template
void index_flexarray(flexarray_file * flex_array)
{
	flexarray_file::iterator it = flex_array->begin();
	flexarray_file::iterator it_end = flex_array->end();

	u64 nr_bytes = 0;
	u32 t0 = ew::core::time::get_ticks();
	u32 t1;
	u64 last_size = 0;
	u64 nr_lines = 0;

	std::cerr << " index_flexarray ...\n";

	flexarray_file::node * n = it.get_node();
	while (n) {

		n = it.get_node();
		index_node(n, &nr_lines);

		// std::cerr << " n->size() = " << n->size() << "\n";
		// std::cerr << " nr_bytes = " << nr_bytes << "\n";
		// std::cerr << " nr_lines = " << nr_lines << "\n";

		nr_bytes += n->size();
		it += n->size();

		t1 = ew::core::time::get_ticks();

		if ((t1 - t0) >= 1000) {

			float percent = ((float)last_size / (float)flex_array->size()) * 100.0f;

			u64 nr_byte_per_second = nr_bytes - last_size;
			float cur_time = (float(t1) - float(t0)) / 1000.0f;

			u64 buffer_size_mib   = flex_array->size() / (1024.0f * 1024.0f);
			u64 nr_kib_per_second = ((float)nr_byte_per_second / 1024.0f) / cur_time;
			u64 nr_mib_per_second = ((float)nr_byte_per_second / (1024 * 1024)) / cur_time;

			u64 total_mib_read = nr_bytes / (1024.0f * 1024.0f);

			std::cerr << "indexing file : " <<  total_mib_read << " / " << buffer_size_mib << " Mib, " << nr_lines << " lines (" << percent << "%), ";
			std::cerr << "~" << nr_mib_per_second << " Mibs/s, ";
			std::cerr << "~" << nr_kib_per_second << " kibs/s, ";
			std::cerr << "~" << nr_byte_per_second << " bytes/s          ";
			std::cerr << "\r";

			t0 = t1;
			last_size = nr_bytes;
		}


		//   n = n->next();
		if (n == it_end.get_node())
			break;
	}

	std::cerr << "\n";
}

void lines_test(flexarray_file * flex_array)
{
#ifndef DISABLE_LINE_COUNT

	flexarray_file::node * root = flex_array->root_node();
	page_info * pi = static_cast<page_info *>(root->get_meta_data());

	u64 max_lines = pi->m_nr_eol + 1;

	u32 max_time = 0;
	u32 min_time = (u32) - 1;

	u32 total_t0 = ew::core::time::get_ticks();
	{
		for (u64 line_index = 1; line_index < max_lines; ++line_index) {
			u32 l_t0 = ew::core::time::get_nanoseconds_since_startup();

			find_line fl(line_index - 1);
			fl = flex_array->find(fl);
			assert(fl.find_node);
			u32 l_t1 = ew::core::time::get_nanoseconds_since_startup();

			min_time = ew::maths::min((l_t1 - l_t0), min_time);
			max_time = ew::maths::max((l_t1 - l_t0), max_time);


		}
	}
	u32 total_t1 = ew::core::time::get_ticks();
	std::cerr << "line test : " << (total_t1 - total_t0) << " ms\n";
	std::cerr << "total lines : " << max_lines << "\n";

	std::cerr << "min search time  : " << min_time << "\n";
	std::cerr << "max search time  : " << max_time << "\n";

	u32 diff = total_t1 - total_t0 ;
	if (diff) {
		std::cerr << "line/ms : " << (max_lines) / (total_t1 - total_t0) << "\n";
	}
#endif
}


void dump_test(flexarray_file * flex_array)
{
	flexarray_file::iterator it = flex_array->begin();
	flexarray_file::iterator it_end = flex_array->end();
	u64 idx = 0;

	std::cerr << "8<------------------\n";
	while (it != it_end) {
		flexarray_file::node * n = it.get_node();

		page * p = n->page();
		if (p) {
			p->map();
			{
				u8 * b = p->begin();
				u8 * e = p->end();
				std::cout.write((const char *)b, e - b);
			}
			p->unmap();
		} else {
			std::cerr << " error in page[" << idx << " ].size = " << n->size() << "\n";
		}

		it += n->size();
		++idx;
	}

	std::cerr << "<EOF>\n8<------------------\n";
}

void offset_test(flexarray_file * flex_array)
{
	std::cerr << " offset_test\n";

	flexarray_file::node * root = flex_array->root_node();
	u64 file_size = root->size();

	for (u64 off = 0; off < file_size; off += 1) {
		u32 t0 = ew::core::time::get_ticks();
		u64 local_offset = 0;
		flexarray_file::node * n = flex_array->find(off , local_offset);
		u32 t1 = ew::core::time::get_ticks();


		if ((off % 4096) == 0) {
			std::cerr << " ------------------------------- page ? -----------------------\n";
		}

		std::cerr << " offset " << off << " in node " << n << "\n";


		u32 time_diff = t1 - t0;
		if (time_diff > 1) {
			std::cerr << " time to find page containing offset(" << off << ") : " << (time_diff) << "\n";
		}
	}

}


void reverse_iterator_test(flexarray_file * flex_array)
{
	flexarray_file::reverse_iterator it = flex_array->rbegin();
	flexarray_file::reverse_iterator it_end = flex_array->rend();

	while (it != it_end) {
		flexarray_file::node * n = it.get_node();

		std::cerr << "it->offset() = " << it.offset() << ", ";
		std::cerr << "n->size() = " << n->size() << ", ";
		std::cerr << "reverse ++it\n";


		++it;
	}

}

void forward_iterator_test(flexarray_file * flex_array)
{
	flexarray_file::iterator it = flex_array->begin();
	flexarray_file::iterator it_end = flex_array->end();


	while (it != it_end) {
		flexarray_file::node * n = it.get_node();
		it.check_invariants();

		if (0) {
			std::cerr << "it->offset() = " << it.offset() << ", ";
			std::cerr << "*it = '" << (char)*it << "' ', ";
			std::cerr << "n  = " << n << ", ";
			std::cerr << "n->size() = " << n->size() << ", ";
			std::cerr << "forward ++it\n";
		}

		++it;
	}

}

void test_array_properties(flexarray_file * flex_array)
{
	u32 min_time = (u32) - 1;
	u32 max_time = 0;

	u8 val;
	for (u64 i = 0; i < flex_array->size(); ++i) {
		u32 l_t0 = ew::core::time::get_nanoseconds_since_startup();

		val = (*flex_array)[i];
//        std::cerr << "op[" << i << "] = '" << val << " , int = " << (int)val << "'\n";

		u32 l_t1 = ew::core::time::get_nanoseconds_since_startup();
		u32 diff = l_t1 - l_t0;
		min_time = std::min(min_time, diff);
		max_time = std::max(max_time, diff);
	}

	std::cerr << "op[] min time  : " << min_time << " ns\n";
	std::cerr << "op[] max time  : " << max_time << " ns\n";
	if (val) {
	}

}

#define STR_SIZE (1024)
char * str = nullptr;

void test_insert(flexarray_file * flex_array)
{
	std::cerr << __FUNCTION__ << "\n";

	str = new char[STR_SIZE];
	const char * hello = "\n\n\n\n\n\n\n\n\n\n123456789!";
	size_t len = strlen(hello);

	::strncpy(str, hello, len);

	std::cerr << "sizeof (str) = " <<  STR_SIZE << "\n";
	size_t before_sz = flex_array->size();
	std::cerr << " Before flex_array->size() " << before_sz << "\n";

	flexarray_file::iterator it = flex_array->begin();

	//++it;
	u32 l_t0 = ew::core::time::get_nanoseconds_since_startup();

	flex_array->insert(it, str, str + len);

	u32 l_t1 = ew::core::time::get_nanoseconds_since_startup();
	u32 time_diff = l_t1 - l_t0;

	std::cerr << " time to insert = "
		  << (time_diff) << " ns, "
		  << (time_diff / 1000) << " micros , "
		  << (time_diff / 1000000) << " milli\n";

	size_t after_sz = flex_array->size();

	std::cerr << " After flex_array->size() " << after_sz << "\n";

	std::cerr << " sz(After - Before) = " << (after_sz - before_sz) << "\n";

	delete [] str;
}

void test_erase(flexarray_file * flex_array)
{
	std::cerr << __FUNCTION__ << "\n";

	size_t before_sz = flex_array->size();
	std::cerr << " Before flex_array->size() " << before_sz << "\n";

	flexarray_file::iterator it = flex_array->begin();

	//++it;
	flexarray_file::iterator it2(it + 5);

	// it2 = flex_array->end();

	u32 l_t0 = ew::core::time::get_nanoseconds_since_startup();
	flex_array->erase(it, it2);
	u32 l_t1 = ew::core::time::get_nanoseconds_since_startup();
	u32 time_diff = l_t1 - l_t0;

	std::cerr << " time to erase = "
		  << (time_diff) << " ns, "
		  << (time_diff / 1000) << " micros , "
		  << (time_diff / 1000000) << " milli\n";

	size_t after_sz = flex_array->size();
	std::cerr << " After flex_array->size() " << after_sz << "\n";
}


void build_stats(flexarray_file * flex_array)
{
	flexarray_file::iterator it = flex_array->begin();
	flexarray_file::iterator it_end = flex_array->end();
	u64 idx = 0;

	std::cerr << "8<------------------\n";
	while (it != it_end) {
		flexarray_file::node * n = it.get_node();

		flex_array->on_node_insert(n);

		it += n->size();
		++idx;
	}

}

bool populate_flex_array(ew::filesystem::file * file, std::size_t max_page_size, flexarray_file * flex_array)
{
	u64 file_size = file->size();

#ifdef HACK_TERA_FILE
	file_size = (u64)((u64)(1 * 128) << (40));
#endif

	if (!file_size) {
		std::cerr << "file size\n";
		exit(1);
	}

	if (!max_page_size) {
		std::cerr << "invalid page size\n";
		exit(1);
	}

	std::cerr << " create flex_array\n";
	auto t0 = ew::core::time::get_ticks();
	flex_array->set_initial_partition_size(file_size);
	flex_array->build_partitions();
	auto t1 = ew::core::time::get_ticks();
	flex_array->dump();
	std::cerr << " flex_array...done " << (t1 - t0) << " ms\n";

	u64 flex_array_size = flex_array->size();
	std::cerr << " flex_array->size() = " << (flex_array_size) << " bytes\n";
	std::cerr << " flex_array->size() = " << (flex_array_size >> 10) << " Kibytes\n";
	std::cerr << " flex_array->size() = " << (flex_array_size >> 20) << " Mibytes\n";
	std::cerr << " flex_array->size() = " << (flex_array_size >> 30) << " Gibytes\n";
	std::cerr << " flex_array->size() = " << (flex_array_size >> 40) << " Tibytes\n";

	return true;
}

bool build_flex_array_pages(ew::filesystem::file * file, std::size_t max_page_size, flexarray_file * flex_array)
{
	u64 off = 0;
	u64 flex_array_size = flex_array->size();

	u64 remain = flex_array_size;
	while (off < flex_array_size) {
		u64 page_sz = ew::maths::min(remain, max_page_size);

		u64 local_off = 0;
		flexarray_file::node * n = flex_array->find(off, local_off);
		assert(n);
		// std::cerr << " looking for off == " << off << "\n";
		// std::cerr << " mapped size = " << page_sz << "\n";


		page * p = nullptr;

#ifdef HACK_TERA_FILE
		// p = new page(page_sz);
		if (page_sz > file->size()) {
			std::cerr << "for the hack to work, the underlying file must be greater or equal to the max page size :-)\n";
			exit(1);
		}
		p = new mpage(file, page_sz, 0, 0);
#else
		p = new mpage(file, page_sz, off, 0);
#endif
		assert(p);
		n->set_page(p);

		// std::cerr << "  new node(" << n << ") size = " << n->size() << "\n";

		off    += page_sz;
		remain -= page_sz;
	}

	return true;
}


int main(int ac, char ** av)
{
	if (ac != 3) {
		std::cerr << "usage : " << av[0] << " file max_page_size\n";
		exit(1);
	}

	ew::filesystem::file file(av[1]);
	bool ret = file.open(ew::core::objects::stream::mode::read_only);
	if (ret == false) {
		std::cerr << " open error\n";
		exit(1);
	}

	ew::core::time::init();

	std::size_t max_page_size = atoi(av[2]);
	flexarray_file * flex_array =  new flexarray_file(max_page_size, max_page_size);


	u32 t0 = ew::core::time::get_ticks();
	{
		populate_flex_array(&file, max_page_size, flex_array);
		build_flex_array_pages(&file, max_page_size, flex_array);
	}
	u32 t1 = ew::core::time::get_ticks();

	std::cerr << " populate done in " << (t1 - t0) << " ms\n";

	std::cerr << " start indexing\n";

	std::cerr << " nr_page_info  = " << nr_page_info << "\n";

	t0 = ew::core::time::get_ticks();
	{
		index_flexarray(flex_array);
	}
	t1 = ew::core::time::get_ticks();
	std::cerr << " indexing done in " << (t1 - t0) << " ms\n";

	std::cerr << "-----\n";
	std::cerr << " build stat\n";

	// call this only when all the leafs are filled
	t0 = ew::core::time::get_nanoseconds_since_startup();
	{
		build_stats(flex_array);
	}
	t1 = ew::core::time::get_nanoseconds_since_startup();
	std::cerr << " build stats done in " << (t1 - t0) << " ns\n";
	std::cerr << " build stats done in " << (t1 - t0) / (1000) << " µs\n";
	std::cerr << " build stats done in " << (t1 - t0) / (1000000) << " ms\n";

	// dump_test(flex_array);

	lines_test(flex_array);

	// offset_test(flex_array);

//   forward_iterator_test(flex_array);

	// std::cerr << "-----\n";
	// std::cerr << " test array\n";

//    test_array_properties(flex_array);

	// dump_test(flex_array);
	std::cerr << "\n------------\n";
	std::cerr << " test insert\n";
	test_insert(flex_array);

	lines_test(flex_array);


	std::cerr << "\n------------\n";
	std::cerr << " dump_test\n";
	//  dump_test(flex_array);

	std::cerr << "\n------------\n";
	std::cerr << " test erase\n";
	test_erase(flex_array);

	lines_test(flex_array);


	std::cerr << "\n------------\n";
	std::cerr << " dump_test\n";
//    dump_test(flex_array);

	std::cerr << "pause... hit ctrl-c to quit\n";
	//  ::pause();

	delete flex_array;
	return 0;
}

}
}
}

int main(int ac, char ** av)
{
	return test::core::container::main(ac, av);
}
