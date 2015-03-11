#undef NDEBUG
#include <assert.h>

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



int main(int ac, char ** av)
{
	u8 * buf = nullptr;

	if (ac != 3) {
		std::cerr << "usage : " << av[0] << " nb_run utf8_file.txt\n";
		::exit(1);
	}

	goto start;


error:

	delete [] buf;
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

	struct stat st;
	if (fstat(fd, &st) < 0)
		goto error;

	buf = new u8[st.st_size];
	u8 * buf_end = buf + st.st_size;
	std::cerr << "reading file (" << av[2] << ") size : " << st.st_size << " bytes\n";
	int n = ::read(fd, buf, st.st_size);
	if (n < 0) {
		std::cerr << "read error\n";
		return 1;
	}

	if (n < 0 || n != st.st_size)
		goto error;


	//////////////////////
	auto codec = ew::codecs::text::unicode::utf8::codec();



	//////////////////
	// decode forward:

	u64 * offsets2 = new u64[st.st_size + 1];
	u64 * offsets2_base = offsets2;

	s32 * cpvec2 = new s32[st.st_size + 1];
	s32 * cpvec2_base = cpvec2;
	std::cerr  << "start forward decoder\n";


	u8 * s = buf;

	u32 nr_fwd_cp = 0;
	u32 nr_fwd_ok = 0;
	u32 nr_fwd_error = 0;
	s32 unicode;
	*offsets2++ = (s - buf);
	while (s < buf_end) {
		auto next   = ew::codecs::text::get_cp<decltype(s)>(codec, s, buf_end, &unicode);
		*cpvec2++   = unicode;
		*offsets2++ = (next - buf);
		assert(next > s);

		++nr_fwd_cp;
		if (unicode < 0) {
			++nr_fwd_error;
		} else {
			++nr_fwd_ok;
		}

#if 0
		std::cerr << "offset = " << (s - buf) << "\n";
		std::cerr << "unicode " << unicode << ", ";
		std::cerr << "unicode 0x" << std::hex << unicode << std::dec << ", ";
		std::cerr << "unicode char " << (char)unicode << "\n";
		std::cerr << "advance = " << (next - s) << "\n";
#endif
		s = next;

	}
	std::cerr << "---------------------\n";

	for (u32 i = 0; i < nr_fwd_cp; ++i) {
		auto cp = cpvec2_base[i];
		auto off = offsets2_base[i];
		std::cerr << "cp["<<i<<"]" << cp << " " << "off["<<i<<"]" << off << "\n";
		assert(cp >= -1);
		assert(cp <  0x1fffffff);
	}

	///////////////////////////////////////


	std::cerr  << "start backward decoder\n";
	s = buf_end;
	std::cerr << " offset = " << (s - buf) << "\n";
	u32 count = 0;
	u64 * offsets3 = new u64[st.st_size + 1];
	while (s > buf) {
		u8 * out;
		unicode = codec->rget_cp(buf, s, &out);
		*offsets3++ = (s - buf);

#if 0
		std::cerr << "offset = " << (s - buf) << "\n";
		std::cerr << "unicode " << unicode << ", ";
		std::cerr << "unicode 0x" << std::hex << unicode << std::dec << ", ";
		std::cerr << "unicode char " << (char)unicode << "\n";
		std::cerr << "advance = " << (s - out) << "\n";
#endif
		s = out;

		++count;
#if 0
		char a = 0;
		std::cerr << "continue y/n";
		std::cin >> a;
		if (a != 'y')
			break;
#endif
	}


	/////////////////
	std::cerr  << "backward decoded " << count << " codepoints\n";




	delete [] buf;
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

