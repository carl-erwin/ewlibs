#ifdef _MSC_VER
typedef unsigned __int8  uint8_t;
typedef unsigned __int32 uint32_t;
#else
#include <stdint.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#include <cstdlib>
#include <iostream>
#include <algorithm> // std::for_each

#include <ew/filesystem/mapped_file.hpp>
using namespace ew::filesystem;

#include "../../include/ew/core/types/types.hpp"
#include "../../include/ew/codecs/text/unicode/utf8/utf8.hpp"
#include "../../include/ew/core/container/flexible_array.hpp"

using namespace ew::core::types;

namespace ew
{

namespace test
{

namespace unicode
{

namespace utf8
{


namespace TOOLS
{
template <typename T> T min(T a, T b)
{
	return (a < b) ? a : b;
}
template <typename T> T max(T a, T b)
{
	return (a > b) ? a : b;
}
}

u64 get_time_diff(struct timeval * start, struct timeval * stop)
{
	u64 t0 = (start->tv_sec * 1000000) + start->tv_usec;
	u64 t1 = (stop->tv_sec * 1000000) + stop->tv_usec;
	return t1 - t0;
}


template <typename T>
std::ostream & print_bin(std::ostream & stream, T val)
{
	for (int i = (sizeof(T) * 8) - 1; i >= 0; i--) {
		stream << ((val >> i) & 1);
	}
	return stream;
}


namespace OTHER
{

// from http://bjoern.hoehrmann.de/utf-8/decoder/dfa/

#define ASCII_IN_TABLE 1

/*
  The first 128 entries are tuples of 4 bit values. The lower bits
  are a mask that when xor'd with a byte removes the leading utf-8
  bits. The upper bits are a character class number. The remaining
  160 entries are a minimal deterministic finite automaton. It has
  10 states and each state has 13 character class transitions, and
  3 unused transitions for padding reasons. When the automaton en-
  ters state zero, it has found a complete valid utf-8 code point;
  if it enters state one then the input sequence is not utf-8. The
  start state is state nine. Note the mixture of octal and decimal
  for stylistic reasons. The ASCII_IN_TABLE macro makes the array
  bigger and the code simpler--but not necessarily faster--if set.
*/

static const uint8_t utf8d[] = {

#if ASCII_IN_TABLE
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#endif

	070, 070, 070, 070, 070, 070, 070, 070, 070, 070, 070, 070, 070, 070, 070, 070,
	050, 050, 050, 050, 050, 050, 050, 050, 050, 050, 050, 050, 050, 050, 050, 050,
	030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030,
	030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030,
	204, 204, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188,
	188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188,
	174, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 142, 126, 126,
	111, 95, 95, 95, 79, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,

	0, 1, 1, 1, 8, 7, 6, 4, 5, 4, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 8, 7, 6, 4, 5, 4, 3, 2, 1, 1, 1, 1,

};

uint32_t
decode(uint8_t * s, size_t size)
{
	uint8_t data, byte, stat = 9;
	uint32_t unic = 0;
	uint32_t len = 0;
	uint8_t * end = s + size;

	for (; s != end; ++s) {
		if ((byte = *s) == 0) {
			break;
		}

		// Each byte is associated with a character class and a mask;
		// The character class is used to advance a finite automaton;
		// The mask is used to strip off leading bits from the byte;
		// The remaining bits are combined into a Unicode code point;
		// A code point is complete if the DFA enters the final state.

#if ASCII_IN_TABLE

		data = utf8d[ byte ];
		stat = utf8d[ 256 + (stat << 4) + (data >> 4)];
		byte = (byte ^ (uint8_t)(data << 4));

#else
		if (byte < 0x80) {
			stat = utf8d[ 128 + (stat << 4)];
		} else {
			data = utf8d[ byte - 0x80 ];
			stat = utf8d[ 128 + (stat << 4) + (data >> 4)];
			byte = (byte ^ (uint8_t)(data << 4));
		}
#endif

		unic = (unic << 6) | byte;

		if (!stat) {
			// unic is now a proper code point, we just print it out.
			// printf("U+%04X\n", unic);
			unic = 0;
		} else if (stat == 1) {
			//   // the byte is not allowed here; the state would have to
			//   // be reset to continue meaningful reading of the string
			unic = 0;
			stat = 9;
		}

		++len;
	}

	return len;
}

}


namespace OTHER2
{
// from http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

static const uint8_t utf8d[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 00..1f
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 20..3f
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 40..5f
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 60..7f
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, // 80..9f
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, // a0..bf
	8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // c0..df
	0xa, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x3, // e0..ef
	0xb, 0x6, 0x6, 0x6, 0x5, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, // f0..ff
	0x0, 0x1, 0x2, 0x3, 0x5, 0x8, 0x7, 0x1, 0x1, 0x1, 0x4, 0x6, 0x1, 0x1, 0x1, 0x1, // s0..s0
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, // s1..s2
	1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, // s3..s4
	1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, // s5..s6
	1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // s7..s8
};

__attribute__((always_inline)) static inline uint32_t
decode_step(uint32_t * state, uint32_t * codep, uint32_t byte)
{
	uint32_t type = utf8d[byte];

	*codep = (*state != UTF8_ACCEPT) ?
		 (byte & 0x3fu) | (*codep << 6) :
		 (0xff >> type) & (byte);

	/*
	  uint32_t type = utf8d[byte];
	  if (state != UTF8_ACCEPT) {
	  cp = (byte & 0x3fu) | (*codep << 6) :
	  } else {
	  cp = (0xff >> type) & (byte);
	  }
	  *state = utf8d[256 + *state*16 + type];
	  */

	*state = utf8d[256 + *state * 16 + type];
	return *state;
}

uint32_t decode(uint8_t * s, size_t size)
{
	uint32_t codepoint = 0;
	uint32_t state = 0;
	uint32_t len = 0;
	uint8_t * end = s + size;

	for (; s != end; ++s) {
		if (*s == 0)
			break;

		decode_step(&state, &codepoint, *s);
		if (state == UTF8_ACCEPT) {
			codepoint = 0;
			state = 0;
		}

		if (state == UTF8_REJECT) {
			codepoint = 0;
			state = 0;
		}

		++len;
	}
	return len;
}

}



class Test_functor
{
public:
	uint32_t codepoint;
	uint32_t state;
	uint32_t len;

	uint32_t sum;

	Test_functor()
	{
		codepoint = 0;
		state = 0;
		len = 0;
		sum = 0;
	}

	~Test_functor()
	{
		len = 0;
	}

	__attribute__((always_inline))
	inline bool operator()(const u8 byte)
	{

		if (byte == 0) {
			return false;
		}

		OTHER2::decode_step(&state, &codepoint, byte);
		if (state == UTF8_ACCEPT) {
			codepoint = 0;
			state = 0;
		}

		if (state == UTF8_REJECT) {
			codepoint = 0;
			state = 0;
		}


		sum += byte;
		++len;
		return true;
	}

};




class Test_functor2
{
public:

	uint32_t len;

	uint32_t sum;

	ew::codecs::text::text_codec * C;

	int cp;
	int state ;
	int finish_state;
	int error_state;

	const u8  * state_tab ;
	const u8  * byte_mask;
	u32 cp_shift;
	u32 state_shift;

	Test_functor2()
	{
		cp = 0;
		state = 0;
		len = 0;
		sum = 0;

		C   = ew::codecs::text::unicode::utf8::codec();

		state        = C->get_starting_state();
		finish_state = C->get_finish_state();
		error_state  = C->get_error_state();

		state_tab   = C->get_state_table();
		byte_mask   = C->get_byte_mask_table();
		cp_shift     = C->get_cp_shift();
		state_shift  = C->get_state_shift();
	}

	~Test_functor2()
	{
		len = 0;
	}

	__attribute__((always_inline))
	inline bool operator()(const u8 byte)
	{

//        if (byte == 0) {
//            return false;
//        }

		//C->step(byte, &state, &cp);

		cp = (cp << cp_shift) | byte_mask[byte];
		state = state_tab[(state << state_shift) + byte];
		if (state == finish_state) {
			cp = 0;
			state = 0;
		}

		if (state == error_state) {
			cp = 0;
			state = 0;
		}

		sum += byte;
		++len;
		return true;
	}

};


namespace FAKE
{

size_t nr_bytes;

uint32_t setup(u8 * p, size_t nr)
{
	nr_bytes = nr;
	return 0;
}

uint32_t decode(uint8_t * s, size_t size)
{
	uint32_t codepoint = 0;
	uint32_t state = 0;
	uint32_t  len = 0;
	uint8_t  * send = s + size;

	for (; s != send; ++s) {

		if (*s == 0)
			break;

		OTHER2::decode_step(&state, &codepoint, *s);
		if (state == UTF8_ACCEPT) {
			codepoint = 0;
			state = 0;
		}

		if (state == UTF8_REJECT) {
			codepoint = 0;
			state = 0;
		}


		++len;
	}
	return len;
}

}


namespace ARRAY
{

std::vector<u8> vec;

uint32_t setup(u8 * p, size_t nr_bytes)
{
	u8 * pend = p + nr_bytes;

	uint32_t len = 0;
	for (; p != pend; ++p) {
		vec.push_back(*p);
		++len;
	}
	vec.push_back((u8)0);

	std::cerr << " array setup len = " << vec.size() << "\n";

	return len;
}

uint32_t quit()
{
	vec.resize(0);
	vec.clear();
	return 0;
}

uint32_t decode(uint8_t * filename, size_t size)
{
	auto it = vec.begin();
	auto it_end = vec.end();


	uint32_t len = 0;
	uint32_t codepoint = 0;
	uint32_t state = 0;

	for (; it != it_end; ++it) {

		if (*it == 0)
			break;

		OTHER2::decode_step(&state, &codepoint, *it);
		if (state == UTF8_ACCEPT) {
			codepoint = 0;
			state = 0;
		}

		if (state == UTF8_REJECT) {
			codepoint = 0;
			state = 0;
		}



		++len;
	}

	return len;
}

}


namespace FLEX
{

typedef class ew::core::container::flexible_array<u8, ew::core::container::page<u8> > flexArray;

flexArray * buff = 0;

//class Test_functor t;
class Test_functor2 t;

uint32_t setup(u8 * p, size_t nr_bytes)
{
	u8 * pend = p + nr_bytes;
	// uint32_t len = 0;
	// for (; p != pend; ++p) {
	//   buff.push_back(*p);
	//   ++len;
	// }
	buff = new flexArray(32 * 1024, 32 * 1024);
	buff->insert(buff->begin(), p, pend);
	buff->push_back((u8)0);

	std::cerr << " flex_array setup len = " << buff->size() << "\n";

	return buff->size();
}

uint32_t quit()
{
	std::cerr << " --- flex quit ----\n";
	std::cerr << " flex sum = " << t.sum << "\n";
	std::cerr << " flex_array setup len = " << buff->size() << "\n";
	delete  buff;
	return 0;
}


uint32_t decode(uint8_t * filename, size_t size)
{
	t = Test_functor2();
	t = buff->foreach_item(t);
	return t.len;
}

}


namespace FLEXIT
{

typedef class ew::core::container::flexible_array<u8, ew::core::container::page<u8> > flexArray;

flexArray * buff = 0;

class Test_functor t;

uint32_t setup(u8 * p, size_t nr_bytes)
{
	u8 * pend = p + nr_bytes;
	// uint32_t len = 0;
	// for (; p != pend; ++p) {
	//   buff.push_back(*p);
	//   ++len;
	// }
	buff = new flexArray();
	buff->insert(buff->begin(), p, pend);
	buff->push_back((u8)0);
	std::cerr << " flexit_array setup len = " << buff->size() << "\n";
	return buff->size();
}

uint32_t quit()
{
	std::cerr << " --- flexit quit ---------\n";
	std::cerr << " flexit sum = " << t.sum << "\n";
	std::cerr << " flexit_array setup len = " << buff->size() << "\n";
	delete  buff;
	return 0;
}


uint32_t decode(uint8_t * filename, size_t size)
{
	auto it = buff->begin();
	auto it_end = buff->end();

#if 0

	u32 len;
	ew::codecs::text::decode<decltype(it)>(ew::codecs::text::unicode::utf8::codec(), it, it_end, &len);
	return len;

#else

#define USE_EW_TEXT_CODEC 1

#ifdef USE_EW_TEXT_CODEC
	auto codec = ew::codecs::text::unicode::utf8::codec();
	int32_t cp = 0;
	int start_state = codec->get_starting_state();
	int finish_state = codec->get_finish_state();
	int error_state  = codec->get_error_state();
	int state = start_state;

	auto state_tab   = codec->get_state_table();
	auto byte_mask   = codec->get_byte_mask_table();
	auto cp_shift    = codec->get_cp_shift();
	auto state_shift = codec->get_state_shift();

#else

	uint32_t cp = 0;
	uint32_t start_state = 0;
	uint32_t finish_state = UTF8_ACCEPT;
	uint32_t error_state  = UTF8_REJECT;
	uint32_t state = start_state;

#endif


	t.len = 0;

#ifdef USE_EW_TEXT_CODEC
	// auto codec = ew::codecs::text::unicode::utf8::codec();
#endif

	for (; it != it_end; ++it) {
		u8 byte = *it;
#ifdef USE_EW_TEXT_CODEC

		//codec->step(byte, &state, &cp);
		cp = (cp << cp_shift) | byte_mask[byte];
		state = state_tab[(state << state_shift) + byte];
#else
		OTHER2::decode_step(&state, &cp, byte);
#endif
		if (state == finish_state) {
			// prev_cp = cp;
			state = start_state;
			cp = 0;
		}

		if (state == error_state) {
			// prev_cp = cp;
			state = start_state;
			cp = 0;
		}

		++t.len;
	}

	return t.len;
#endif
}

}




namespace EW_UTF8_FROM_RAM
{

size_t nr_bytes;

uint32_t setup(u8 * p, size_t nr)
{
	nr_bytes = nr;
	return 0;
}

uint32_t decode(uint8_t * s, size_t size)
{
	u32  len = 0;
	uint8_t  * send = s + size;

//    u64 count = 0;
	ew::codecs::text::decode<decltype(s)>(ew::codecs::text::unicode::utf8::codec(), s, send, &len/*, &count*/);
	//  std::cerr << "count = " << count << "\n";
	return len;
}

}

using namespace ew::codecs::text::unicode;

// TODO: #ifdef __EW_UNICODE_TEST_MAIN__
#ifndef __EW_NO_MAIN__

struct decoder_info {
	decoder_info(const char * name, uint32_t (*decode_fn)(uint8_t * s, size_t size))
	{
		min = 0xFFFFFFFF;
		max = 0;
		sum = 0.0;

		_decode_fn = decode_fn;
		_name = name;
	}

	void run_test(u8 * p, size_t nr_bytes, size_t nb_run)
	{
		_nr_bytes =  nr_bytes;
		len = _decode_fn(p, nr_bytes); // warm cache
		for (size_t i = 0; i < nb_run; i++) {
			gettimeofday(&start, NULL);
			len = _decode_fn(p, nr_bytes);
			gettimeofday(&stop, NULL);
			t1 = get_time_diff(&start, &stop);
			min = test::unicode::utf8::TOOLS::min(min, t1);
			max = test::unicode::utf8::TOOLS::max(max, t1);
			sum += t1;
		}
	}

	void print_report()
	{
		std::cerr << "------\n";
		std::cerr << _name << " : min = " << min << "  max = " << max << "\n";
		std::cerr << "  " << len << " bytes in " << (float)min / 1000000.0f << " sec\n";
		std::cerr << "  max mb " << (float)_nr_bytes / (1024.f * 1024.f) / (float)min * 1000000.f << " Mb/s\n";
		std::cerr << "  min mb " << (float)_nr_bytes / (1024.f * 1024.f) / (float)max * 1000000.f << " Mb/s\n";
	}

public:
	const char * _name;
	uint32_t (*_decode_fn)(uint8_t * s, size_t size);

	u64 min;
	u64 max;
	double sum;
	size_t len;
	size_t _nr_bytes;

	u64 t0;
	u64 t1;

	struct timeval start;
	struct timeval stop;
};

int main(int ac, char ** av)
{
	int test_num = 0;

	if (ac < 3) {
		std::cerr << "usage : " << av[0] << " nb_run utf8_file.txt [ test_num ]\n";
		::exit(1);
	}

	if (ac == 4) {
		test_num = ::atoi(av[3]);
		std::cerr << " test_num = " << test_num << "\n";
	}

	goto start;

error:
	std::cerr << av[0] << " error\n";
	return 1;

start:

	std::cerr << av[0] << " utf8 init\n";
	ew::codecs::text::unicode::utf8::init();

	int nb_run = ::atoi(av[1]);
	if (nb_run <= 0)
		goto error;

	int fd = ::open(av[2], O_RDONLY);
	if (fd < 0) {
		std::cerr  << "cannot open " << av[2] << "\n";
		goto error;
	}

	u32 nr_pages = 1;
	mapped_file * buff = new mapped_file(av[2], nr_pages);
	bool ret = buff->open();
	if (ret == false) {
		std::cerr << "cannot open " << av[1] << "\n";
		delete buff;
		return 0;
	}

	struct stat st;
	if (fstat(fd, &st) < 0)
		goto error;

	u8 * buf = new u8[st.st_size + 1];
	std::cerr << "reading file (" << av[2] << ") size : " << st.st_size << " bytes\n";
	int n = ::read(fd, buf, st.st_size);
	if (n < 0) {
		std::cerr << "read error\n";
		return 1;
	}


	buf[st.st_size] = 0;

	if (n < 0 || n != st.st_size)
		goto error;

	// start
	u8 * p = buf;
	// int c;
	// int nbc = 0;

	decoder_info dec1("utf8::decode", ew::codecs::text::unicode::utf8::decode); // FIXME: reimplement here ?
//    decoder_info dec1("utf8::decode", EW_UTF8_FROM_RAM::decode);
	decoder_info dec2("::OTHER::decode", OTHER::decode);
	decoder_info dec3("::OTHER2::decode", OTHER2::decode);
	decoder_info dec4("::FAKE::decode", FAKE::decode);
	decoder_info dec5("::ARRAY::decode", ARRAY::decode);
	decoder_info dec6("::FLEX::decode", FLEX::decode);
	decoder_info dec7("::FLEXIT::decode", FLEXIT::decode);
	decoder_info dec8("ew utf8 from memory ", EW_UTF8_FROM_RAM::decode);

	std::cerr << "nb run = " << nb_run << "\n";

	// factorize
	// use index + mask

	if (test_num == 0 || test_num == 1) {
		dec1.run_test(p, st.st_size, nb_run);
	}
	if (test_num == 0 || test_num == 2) {
		dec2.run_test(p, st.st_size, nb_run);
	}
	if (test_num == 0 || test_num == 3) {
		dec3.run_test(p, st.st_size, nb_run);
	}

	if (test_num == 0 || test_num == 4) {
		FAKE::setup(p, st.st_size);
		dec4.run_test(p, st.st_size, nb_run);
		// ::FAKE::quit();
	}

	if (test_num == 0 || test_num == 5) {
		ARRAY::setup(p, st.st_size);
		dec5.run_test(p, st.st_size, nb_run);
		ARRAY::quit();
	}

	if (test_num == 0 || test_num == 6) {
		FLEX::setup(p, st.st_size);
		dec6.run_test((u8 *)buff, st.st_size, nb_run);
		FLEX::quit();
	}

	if (test_num == 0 || test_num == 7) {
		FLEXIT::setup(p, st.st_size);
		dec7.run_test((u8 *)buff, st.st_size, nb_run);
		FLEXIT::quit();
	}

	if (test_num == 0 || test_num == 8) {
		EW_UTF8_FROM_RAM::setup(p, st.st_size);
		dec8.run_test(p, st.st_size, nb_run);
		// EW_UTF8_FROM_RAM::quit();
	}

	std::cerr << "------\n";
	// for loop + pointers + index
	if (test_num == 0 || test_num == 1)
		dec1.print_report();

	if (test_num == 0 || test_num == 2)
		dec2.print_report();

	if (test_num == 0 || test_num == 3)
		dec3.print_report();

	if (test_num == 0 || test_num == 4)
		dec4.print_report();

	if (test_num == 0 || test_num == 5)
		dec5.print_report();

	if (test_num == 0 || test_num == 6)
		dec6.print_report();

	if (test_num == 0 || test_num == 7)
		dec7.print_report();

	if (test_num == 0 || test_num == 8)
		dec8.print_report();

	return 0;
}

}
}
}
}


int main(int ac, char ** av)
{
	return ew::test::unicode::utf8::main(ac, av);
}

#endif
