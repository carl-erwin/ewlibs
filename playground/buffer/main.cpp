#include <memory>
#include <thread>
#include <mutex>
#include <list>
#include <assert.h>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>

#include <ew/filesystem/file.hpp>
#include <ew/filesystem/mapped_file.hpp>
using namespace ew::filesystem;

#include <ew/core/time/time.hpp>
using namespace ew::core::time;


namespace test
{
namespace core
{
namespace container
{

#define PAGE_MEGA(X) (((X)*(1024*1024))/(4096))

u64 nrPages = PAGE_MEGA(1); // move to env

void do_buffer_cat(mapped_file * buff)
{
	u64 off = 0;
	u64 n = buff->size();

	u8 * d = 0;
	u64 dsz = 4096 * PAGE_MEGA(64);
	d = new u8 [dsz + 1];

#if 0
	u64 NR_ITS = 2 * 1000000;
	std::vector <mapped_file::iterator *> iterators;

	std::cerr << "register iterators...";
	iterators.reserve(NR_ITS);
	for (u64 i = 0; i < NR_ITS; i++) {
		mapped_file::iterator * it = new mapped_file::iterator;
		it->setAutomaticUpdate(true);
		*it = buff->begin();
		iterators.push_back(it);
	}
	std::cerr << "END\n";
#endif

	// std::cerr << "buff->size() = " << buff->size() << "\n";

	while (n) {
		u64 nrRead;

		buff->read(off, d, dsz, &nrRead);
		n -= nrRead;
		off += nrRead;

		// std::cerr << "cat nrRead = " << nrRead << "\n";
		// std::cerr << "'";

		// ::write(2, d, nrRead);

		// std::cerr << "'\n";
		// std::cerr << "n   = " << n << "\n";
		// std::cerr << "off = " << off << "\n";
	}

	delete [] d;
}

void test_cat(int ac, char ** av)
{
	std::cerr  << "test_cat\n";
	mapped_file * buff = new mapped_file(av[1], nrPages);
	bool ret = buff->open();
	if (ret == true)
		do_buffer_cat(buff);
	delete buff;
}


void word_count(const u8 * data, u64 data_size, u64 & nr_lines, u64 & nr_cp , int & in_word, u64 & nr_word)
{
	const u8 * it = data;
	const u8 * it_end = data + data_size;

	while (it != it_end) {
		s32 c = *it++;
		++nr_cp;
#if 1
		if (c == '\n') {
			//X = 0;
			++nr_lines;
		}
#endif

#if 1
		if (c == ' ' || c == '\t' || c == '\n') {
			in_word = 0;
		} else {
			if (in_word == 0)
				++nr_word;
			in_word = 1;
		}
#endif
	}
	// max(X) = largest line
	// min(X) = slimest line
}

void word_count_it(mapped_file::iterator & it, mapped_file::iterator & it_end, u64 & nr_lines, u64 & nr_cp , int & in_word, u64 & nr_word)
{

	while (it != it_end) {
		s32 c = *it++;
		++nr_cp;
		if (c == '\n') {
			//X = 0;
			++nr_lines;
		}

#if 1
		if (c == ' ' || c == '\t' || c == '\n') {
			in_word = 0;
		} else {
			if (in_word == 0)
				++nr_word;
			in_word = 1;
		}
#endif
	}
	// max(X) = largest line
	// min(X) = slimest line
}

struct wc_func_obj {
	u64 nr_lines = 0;
	u64 nr_cp    = 0;
	int in_word  = 0;
	u64 nr_word  = 0;

	bool operator()(u8 * b, u8 * e)
	{
		word_count(b, e - b , nr_lines, nr_cp , in_word, nr_word);
		return true;
	}

};

void test_wc_regular(int ac, char ** av)
{
	auto buff  = new file(av[1]);


	bool ret = buff->open(ew::core::objects::stream::mode::read_only);
	if (ret) {

		u64 nr_lines = 0;
		u64 nr_cp = 0;
		u64 nr_word = 0;
		int in_word = 0;

		struct data_block {
			data_block(u8 * data_, u64 size_) :data(data_),size(size_) { }
			~data_block()
			{
				delete [] data;
			}

			u8 * data;
			u64 size;
		};

		const size_t blocksize = 1024*1024;
		data_block * bl = new data_block(new u8[blocksize], 0);
		madvise(bl->data, blocksize, MADV_SEQUENTIAL);

		while (true) {

			u64 nrRead;
//
			buff->read(bl->data, blocksize, &nrRead);
			bl->size = nrRead;

			word_count(bl->data, bl->size, nr_lines, nr_cp , in_word, nr_word);


			if (nrRead == 0)
				break;
		}
		delete bl;
		std::cerr << " wait for indexer\n";
		std::cout << " " << nr_lines << " " << nr_word << " " << nr_cp << " " << av[1] << "\n";
	}

}


void test_wc_mapped(int ac, char ** av)
{
	mapped_file * buff  = new mapped_file(av[1], nrPages);

	bool ret = buff->open();
	if (ret) {
		mapped_file::iterator it = buff->begin();
		mapped_file::iterator it_end = buff->end();

#if 1
		u64 nr_lines = 0;
		u64 nr_cp = 0;
		u64 nr_word = 0;

#if 1
		wc_func_obj wc;
		wc = buff->foreach_pages(wc);

		nr_lines = wc.nr_lines;
		nr_word = wc.nr_word;
		nr_cp = wc.nr_cp;
#else
		int in_word = 0;

		u64 nrRead;

		u8 data[64 * 1024];
		while (it != it_end) {

			buff->read(it.offset(), data, (size_t)sizeof(data), &nrRead);
			it += nrRead;
			word_count(data, nrRead, nr_lines, nr_cp , in_word, nr_word);
		}
#endif

#else
		word_count_it(it, it_end, nr_lines, nr_cp , in_word, nr_word);
#endif
		std::cout << " " << nr_lines << " " << nr_word << " " << nr_cp << " " << av[1] << "\n";
	}
}


void test_read_write(int ac, char ** av, mapped_file * buff)
{
	u64 off = 0;
	u64 dsz = 4096 * nrPages;
	u8 * d = new u8[dsz + 1];
	u64 nrRead;

	std::cerr << "buff->size() = " << buff->size() << "\n";
	// --
	off = 0;
	buff->read(off, d, 32, &nrRead);
	std::cerr << "READ " << nrRead << " bytes\n";
	std::cerr << "'";
	size_t n = ::write(2, d, nrRead);
	if (n != nrRead)
		std::cerr << "write error ...\n";
	std::cerr << "'\n";

	// --
	std::cerr << "----\n\n";
	std::cerr << "RM @" << off << "\n";
	// buff->remove(off, 6, 0);
	std::cerr << "buff->size() = " << buff->size() << "\n";

	buff->read(0, d, 32, &nrRead);
	std::cerr << "buffer = '";
	n = write(2, d, nrRead);
	if (n != nrRead)
		std::cerr << "write error ...\n";
	std::cerr << "'\n";

	// --
	std::cerr << "\n";
	std::cerr << "----- WRITE\n";
	u64 nrWrite = 4096 * nrPages;
	u64 nrWritten;
	for (u64 i = 0; i < nrWrite; i++) {
		d[i] = '0' + (i % 10);
	}

	buff->write(buff->size(), d, nrWrite, &nrWritten);
	std::cerr << "wrote " << nrWritten << " bytes\n";

	std::cerr << "buff->size() = " << buff->size() << "\n";
	std::cerr << "----- (RE)READ\n";

	do_buffer_cat(buff);

	// off = 0;
	// buff->read(off, d, buff->size(), &nrRead);
	// std::cerr << "buffer[" << nrRead << "] = '";
	// write(2, d, nrRead);
	// std::cerr << "'\n";

	delete [] d;
	d = 0;
}

void test_insert(int ac, char ** av, mapped_file * buff)
{
	u64 nrw;

	std::cerr << "before\n";
	do_buffer_cat(buff);

	//    mapped_file::iterator it0 = buff->begin();
	//    mapped_file::iterator it1 = buff->begin();
	//   mapped_file::iterator it2 = buff->end();

	// it0.set_automatic_update(true);
	// it1.set_automatic_update(true);
	// it2.set_automatic_update(true);

	// buff->write(0, (u8 *)"0", 1, &nrw);
	// assert(nrw == 1);

	buff->write(1, (u8 *)"C", 1, &nrw);
	assert(nrw == 1);

	buff->write(2, (u8 *)"E", 1, &nrw);
	assert(nrw == 1);

	buff->write(3, (u8 *)"G", 1, &nrw);
	assert(nrw == 1);

	std::cerr << "after\n";
	mapped_file::iterator it_cat = buff->begin();
	mapped_file::iterator itend = buff->end();
	while (it_cat != itend) {
		std::cerr << "*it_cat[" << it_cat.offset() << "] == " << *it_cat++ << "\n";
	}
}

void test_cp(int ac, char ** av)
{
	bool ret;
	std::cerr << " test_cp \n";

	if (ac != 3) {
		std::cerr << "usage: " << av[0] << " src dst\n";
		::exit(1);
	}

	std::cerr << " copying " << av[1] << " ---> " << av[2] << "\n";

	mapped_file * buff = new mapped_file(av[1], nrPages);

	ret = buff->open();
	if (ret == false) {
		std::cerr << "cannot open " << av[1] << "\n";
	} else {
		ret = buff->save(av[2]);

		if (ret == false)
			std::cerr << "save failed\n";
		else
			std::cerr << "save ok\n";
	}

	delete buff;
}


void test_simple_replace(int ac, char ** av)
{
	bool ret;
	std::cerr << " test_simple_replace \n";

	if (ac != 4) {
		std::cerr << "usage: " << av[0] << " file charIn charOut\n";
		::exit(1);
	}

	u8 charIn = av[2][0];
	u8 charOut = av[3][0];

	std::cerr << "charIn  = '" << charIn  << "'\n";
	std::cerr << "charOut = '" << charOut << "'\n";

	mapped_file * buff = new mapped_file(av[1], nrPages);

	ret = buff->open();
	if (ret == false) {
		std::cerr << "cannot open " << av[1] << "\n";
	} else {
		mapped_file::iterator it = buff->begin();
		//it.set_automatic_update(true);
		while (it != buff->end()) {
			u8 byte = *it;
			if (byte == charIn) {
				//        *it = charOut; // TODO:
				int sz = 1;
				u64 nr;
				u64 off = it.offset();

				// buff->remove(it.offset(), sz, &nr);
				buff->erase(it, it + sz);
				buff->get_iterator_by_offset(off, &it);
				// must refresh it ?
				// buff->insert(it, charOut); // crash
				// buff->insert(it.offset(), &charOut, 1); // crash
				off = it.offset();
				buff->write(it.offset(), &charOut, 1, &nr);
				buff->get_iterator_by_offset(off, &it);

				// buff->get_iterator_by_offset(it.offset(), &it);
			}
			it++; // crash
		}

		ret = buff->save("./test");

	}

	delete buff;
}

void test_base(int ac, char ** av)
{
	std::cerr  << __FUNCTION__ << "\n";
	mapped_file * buff = new mapped_file(av[1], nrPages);
	bool ret = buff->open();
	if (ret == false) {
		delete buff;
		return;
	}

	{
		auto it_begin = buff->begin();
		auto it_end = buff->end();
		auto it = it_begin;
		u64 count = 0;
		u32 t0;
		u32 t1;

#if 1
		std::cerr << "begin().offset() = " << it.offset() << "\n";
		std::cerr << "end().offset() = " << it_end.offset() << "\n";
		t0 = get_ticks();
		it = it_begin;
		byte last = 0;
		while (it != it_end) {

			byte c = *it;

			if (c == '\r') {
				++count;
			}

			if (c == '\n') {
				if (last != '\r')
					++count;
			}

			last = c;

			//std::cerr << "it.offset() = " << it.offset() << "\n";
			//std::cerr << c;
			++it;
		}
		t1 = get_ticks();
		std::cerr << "\n";
		std::cerr << " forward walk in " << (t1 - t0) << " milliseconds\n";
		std::cerr << " nr '\\n' = " << count << "\n";
		std::cerr << "\n";
#else
		t0 = get_ticks();
		it = it_end;
		byte last = 0;
		do {
			--it;
			byte c = *it;
			if (c == '\n') {
				++count;
			}

			if (c == '\r') {
				if (last != '\n')
					++count;
			}

			last = c;

			//std::cerr << "it.offset() = " << it.offset() << "\n";
			// std::cerr << c;

		} while (it != it_begin);
		t1 = get_ticks();
		std::cerr << "\n";
		std::cerr << " backward walk in " << (t1 - t0) << " milliseconds\n";
		std::cerr << " nr '\\n' = " << count << "\n";
		std::cerr << "\n";
#endif
	}

	//  crash if there are remaining iterators
	delete buff;
}


void test_delete_while_mapped(int ac, char ** av)
{
	mapped_file * buff  = new mapped_file(av[1], nrPages);

	bool ret = buff->open();
	if (ret == false) {
		std::cerr << "cannot open file\n";
		return;
	}

	int loop = 0;
	int sum = 0;
	while (true) {
		mapped_file::iterator it = buff->begin();
		mapped_file::iterator it_end = buff->end();
		while (it != it_end) {
			std::cout << *it;
			sum += *it;
			++it;
		}
		std::cout << "-----------------\n";
		std::cout << " loop = " << loop << "\n";
		++loop;
	}
}

int main(int ac, char ** av)
{
//	test_delete_while_mapped(ac, av);
	// test_base(ac, av);
	// test_cat(ac, av);
	test_wc_mapped(ac, av);
	// test_read_write(ac, av);
	// test_insert(ac, av);
	// test_cp(ac, av);
	// test_simple_replace(ac, av);

	return 0;
}

} // ! namespace test
} // ! namespace core
} // ! namespace container

int main(int ac, char ** av)
{
	return test::core::container::main(ac, av);
}
