#include <iostream>
#include <ew/codecs/text/unicode/utf8/utf8.hpp>

#include  <assert.h>

using namespace ew::core::types;

namespace ew
{
namespace codecs
{
namespace text
{
namespace unicode
{
namespace utf8
{

/*
  [utf8 STATE TRANSITION TABLE]

  -------------------------------------------------------------------------------------------------------------------
  State \ byte | 0-7xf | 80-8F | 90 | 9F | A0-BF | C0-C1 | C2-DF | E0  | E1-EC | ED | EF  | F0 | F1-F3 | F4 | F5-FF |
  -------------------------------------------------------------------------------------------------------------------
  S0           |   F   |       |    |    |       |       |  S1   | S6  |  S2   | S3 | S2  | S4 | S7    | S5 |       |
  -------------------------------------------------------------------------------------------------------------------
  S1           |       |   F   | F  | F  |   F   |       |       |     |       |    |     |    |       |    |       |
  -------------------------------------------------------------------------------------------------------------------
  S2           |       |   S1  | S1 | S1 |   S1  |       |       |     |       |    |     |    |       |    |       |
  -------------------------------------------------------------------------------------------------------------------
  S3           |       |   S1  | S1 | S1 |       |       |       |     |       |    |     |    |       |    |       |
  -------------------------------------------------------------------------------------------------------------------
  S4           |       |       | S2 | S2 |   S2  |       |       |     |       |    |     |    |       |    |       |
  -------------------------------------------------------------------------------------------------------------------
  S5           |       |   S3  |    |    |       |       |       |     |       |    |     |    |       |    |       |
  -------------------------------------------------------------------------------------------------------------------
  S6           |       |       |    |    |   S1  |       |       |     |       |    |     |    |       |    |       |
  -------------------------------------------------------------------------------------------------------------------
  S7           |       |       | S2 | S2 |       |       |       |     |       |    |     |    |       |    |       |
  -------------------------------------------------------------------------------------------------------------------

  S = start state
  F = final state : correct utf8 sequence
  E (or empty)  = error
  T(State, Byte) = next::state (until F or E)
  256 * 8 state = 2048 bytes / (1024 bytes with struct utf8_state_t)
*/

// TODO: merge tables ?
// static u8 utf8_mask_table[256] = { 0 };

static u8 * utf8_mask_table = nullptr;

// TODO: state values use 4bits -> compress
struct utf8_state_t {
	utf8_state_t(size_t nr_state)
	{
		_nr_state = nr_state;
		_size = nr_state;
		utf8_mask_table = new u8[256];
		_val  = new u8[_size];
	}

	~utf8_state_t()
	{
		delete [] _val;
		delete [] utf8_mask_table;
	}

	u8 operator [](const size_t index)
	{
		return _val[index];
	}

	bool set(const size_t index, const u8 value)
	{
		_val[index] = value;
		return true;
	}

private:
public:
	u8  *  _val;
	size_t _nr_state;
	size_t _size;
};

static utf8_state_t STATE(256 * 8);

EW_ALWAYS_INLINE
static inline u8 & utf8_mask_slot_rw(const u8 byte)
{
	return utf8_mask_table[byte];
}

EW_ALWAYS_INLINE
static inline const u8 & utf8_mask_slot(const u8 byte)
{
	return utf8_mask_table[byte];
}

enum state {
	F =  0,
	//
	S0 = 0,
	S1 = 1,
	S2 = 2,
	S3 = 3,
	S4 = 4,
	S5 = 5,
	S6 = 6,
	S7 = 7,
	//
	E  = 8
};

// sub system init
EW_CODECS_TEXT_UNICODE_UTF8_EXPORT bool init(void);
EW_CODECS_TEXT_UNICODE_UTF8_EXPORT bool quit(void);

static utf8_codec codec_instance;
EW_CODECS_TEXT_UNICODE_UTF8_EXPORT ew::codecs::text::text_codec * codec()
{
	return &codec_instance;
}

utf8_codec::utf8_codec()
{
	init();
}
utf8_codec::~utf8_codec() { }

bool init()
{
	static bool was_init = false;
	if (was_init)
		return  true;
	was_init = true;

	size_t i;

	// set all to error state
	for (i = 0; i < 256 * 8; i++) {
		STATE.set(i, E);
	}

	// F(S0) =
	for (i = 0x00; i <= 0x7f; i++) {
		STATE.set(i, F);
	}
	for (i = 0xc2; i <= 0xdf; i++) {
		STATE.set(i, S1);
	}
	for (i = 0xe0; i <= 0xe0; i++) {
		STATE.set(i, S6);
	}
	for (i = 0xe1; i <= 0xec; i++) {
		STATE.set(i, S2);
	}
	for (i = 0xed; i <= 0xed; i++) {
		STATE.set(i, S3);
	}
	for (i = 0xef; i <= 0xef; i++) {
		STATE.set(i, S2);
	}
	for (i = 0xf0; i <= 0xf3; i++) {
		STATE.set(i, S7);
	}
	for (i = 0xf1; i <= 0xf3; i++) {
		STATE.set(i, S4);
	}
	for (i = 0xf4; i <= 0xf4; i++) {
		STATE.set(i, S5);
	}

	// S1
	for (i = 0x80; i <= 0xBF; i++) {
		STATE.set((256 * S1) + i, F);
	}
	// S2
	for (i = 0x80; i <= 0xBF; i++) {
		STATE.set((256 * S2) + i, S1);
	}
	// S3
	for (i = 0x80; i <= 0x9f; i++) {
		STATE.set((256 * S3) + i, S1);
	}
	// S4
	for (i = 0x80; i <= 0xBF; i++) {
		STATE.set((256 * S4) + i, S2);
	}
	// S5
	for (i = 0x80; i <= 0x8f; i++) {
		STATE.set((256 * S5) + i, S3);
	}
	// S6
	for (i = 0xA0; i <= 0xBF; i++) {
		STATE.set((256 * S6) + i, S1);
	}
	// S7
	for (i = 0xA0; i <= 0xBF; i++) {
		STATE.set((256 * S7) + i, S1);
	}

	// len 1
	for (i = 0; i <= 0x7f; i++) {
		utf8_mask_slot_rw(i) = i & 0x7f;
	}
	for (i = 0x80; i <= 0xBF; i++) {
		utf8_mask_slot_rw(i) = i & 0x3f;
	}
	// len 2
	for (i = 0xC0; i <= 0xDF; i++) {
		utf8_mask_slot_rw(i) = i & 0x1f;
	}
	// len 3
	for (i = 0xE0; i <= 0xef; i++) {
		utf8_mask_slot_rw(i) = i & 0xf;
	}
	// len 4
	for (i = 0xF0; i <= 0xf7; i++) {
		utf8_mask_slot_rw(i) = i & 0x7;
	}

	return true;
}

bool quit()
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int  utf8_codec::get_starting_state()
{
	return S0;
}

int  utf8_codec::get_finish_state()
{
	return F;
}

int  utf8_codec::get_error_state()
{
	return E;
}

const u8 * utf8_codec::get_state_table()
{
	return STATE._val;
}

const u8 * utf8_codec::get_byte_mask_table()
{
	return utf8_mask_table;
}

u32  utf8_codec::get_cp_shift()
{
	return 6;
}

u32  utf8_codec::get_state_shift()
{
	return 8;
}



#if 0

//
// some compiler generate slow code if static inline is used
//

EW_ALWAYS_INLINE
static inline void utf8_decode_step(const u8 byte, s32 & state, s32 & cp_accum)
{
	cp_accum = (cp_accum << 6) | utf8_mask_slot(byte);
	state = STATE[(state << 8) + byte];
}
#else

#define utf8_decode_step(B, S, CP)                \
    do {                                          \
    CP = ((CP) << 6) | utf8_mask_slot((B));   \
    S = STATE[((S) << 8) + (B)];              \
} while (0);
#endif



int  utf8_codec::step(byte byte, int * state, int * cp)
{
	utf8_decode_step(byte, *state, *cp);
	return *state;
}

#if 0
EW_ALWAYS_INLINE
static inline s32 inline_get_cp(const u8 * end, const u8 * in, u8 ** out)
{
	s32 s = S0;
	s32 unic = 0;

	while (in != end) {
		utf8_decode_step(*in, s, unic);
		if (s == F) {
			++in;
			*out = (u8 *)in;
			return unic;
		} else if (s == E) {
			*out = (u8 *)in;
			return -1;
		}
		// update pointer if no error
		++in;
	}
	*out = (u8 *)in;
	return -2; // stream truncated
}
#endif

// refactor main loop with search and use a func table for get_cp / search / count
int utf8_codec::count(const u8 * buffer, const u32 len, const s32 c, u64 * nr_ok, u64 * nr_error, u32 * count, u8 ** next)
{
	s32 s = S0;
	s32 unic = 0;

	size_t n_error = 0;
	size_t n_ok = 0;
	size_t local_count = 0;

	const u8 * buffer_end = buffer + len;

	while (buffer != buffer_end) {
		utf8_decode_step(*buffer, s, unic);
		++buffer;
		if (s == F) {
			n_ok++;
			if (unic == c) {
				local_count++;
			}
			// previous = buffer;
			s = S0;
			unic = 0;
		} else if (s == E) {
			n_error++;
			if (buffer != buffer_end) {
				s = S0;
				unic = 0;
			}
			// previous = buffer;
		}
	}

	*next = (u8 *)buffer;
	*nr_ok = n_ok;
	*nr_error = n_error;
	*count = local_count;

	/* error in stream */
	if (s == E)
		return -1;

	/* stream truncated */
	if (s != S0)
		return -2;

	return 0;
}

/* ignore errors :-) */
int  utf8_codec::count_new_line(const u8 * b, const u8 * e, u8 & prev_byte, u64 & counter)
{

	while (b < e) {

		if (*b == '\r') {
			++counter;
		} else if ((*b == '\n') && (prev_byte != '\r')) {
			++counter;
		}

		prev_byte = *b;
		++b;
	}

	return 0;
}

EW_ALWAYS_INLINE
static inline s32 inline_rget_cp(const u8 * start, const u8 * in, u8 ** out)
{
	assert(0);

	if (in <= start) {
		*out = (u8 *)start;
		return -1; // error reverse_eof
	}

	auto last_byte = in;
	--in;
	while (in > start) {
		if ((*in & 0xC0) != 0x80) {
			// start of seq found
			break;
		}
		--in;
	}


	std::cerr << "rewind = " << std::distance(in, last_byte) << "\n";
	std::cerr << "*in = " << std::hex << *in << std::dec << "\n";

	*out = (u8 *)in;

	s32 cp = 0;

	s32 state = S0;

	auto prev_begin = in;
	auto cur_begin  = in;

	while (in < last_byte) {

		utf8_decode_step(*in, state, cp);
		++in;
		if (state == F) {
			std::cerr << "decode " << cp << " @ " << std::distance(cur_begin, last_byte) << "\n";
			cp = 0;
			state = S0;
			prev_begin = cur_begin;
			cur_begin = in;
		}

		if (state == E) {
			state = S0;
			cp = 0;
			prev_begin = cur_begin;
			cur_begin = in - 1;
		}
	}

	std::cerr << " final rewind " << std::distance(prev_begin, last_byte) << "\n";
	*out = (u8 *)prev_begin;

	return cp;
}

s32 utf8_codec::rget_cp(const u8 * start, const u8 * in, u8 ** out)
{
	return inline_rget_cp(start, in, out);
}


// TODO: error checking
s32 utf8_codec::rget_cp(const ew::core::objects::buffer::iterator & rend,
			const ew::core::objects::buffer::iterator & _in,
			ew::core::objects::buffer::iterator * out)
{
#if 1
	s32 cp = 0;

	if (_in <= rend) {
		*out = rend;
		return 0;
	}

	ew::core::objects::buffer::iterator in(_in);
	--in;

	static size_t shift_table[] = { 0 , 6 , 12 , 18 };
	size_t shift_index = 0;

	// transform in do { } while (...);
	while ((in > rend) && ((*in & 0xC0) == 0x80)) {
		cp |= (*in & 0x3F) << (shift_table[shift_index++]);
		--in;
	}

	cp |= utf8_mask_slot(*in) << (shift_table[shift_index]);
	*out = in;
	return cp;

#else

	// check
	ew::core::objects::buffer::iterator cur_pos;

	if ((in.offset() - rend.offset()) >= 4 /* maximum code point size */) {
		// improve this
		cur_pos = in;

		--cur_pos;
		--cur_pos;
		--cur_pos;
		--cur_pos;
	} else {
		cur_pos = rend;
	}

	//  ...[rend_pos][x][x][x][in]
	//
	//  ...[rend_pos][err3][err2][err1][a]
	//  ...[rend_pos][err2][err1][a]
	//  ...[rend_pos][err1][a]
	//  ...[rend_pos][a]
	//  ...[rend_pos][]

	ew::core::objects::buffer::iterator tmp_out(cur_pos);
	ew::core::objects::buffer::iterator last_pos(cur_pos);

	s32 state = S0;
	s32 cp = 0;
	s32 last_cp = 0;

	while (cur_pos < in) {
		utf8_decode_step(*cur_pos, state, cp);
		++cur_pos;
		if (state == F) {
			last_pos = tmp_out;
			tmp_out = cur_pos;
			last_cp = cp;
			cp = 0;

			state = S0;
		}

		if (state == E) {
			last_pos = tmp_out;
			tmp_out = cur_pos;

			last_cp = -1;

			state = S0;
			cp = 0;
		}
	}

	*out = last_pos;
	return last_cp;
#endif
}


// TODO: check size
bool utf8_codec::put_cp(const s32 codepoint, u8 * utf8, u32 * sz)
{
	if (codepoint < 0x80) {
		// DEBUG ?
		if (*sz < 2)
			return false;

		*sz = 1;
		utf8[0] = codepoint & 0x7F;
		utf8[1] = 0;
		return true;
	}

	if (codepoint < 0x800) {
		if (*sz < 3)
			return false;

		*sz = 2;
		utf8[0] = 0xC0 | (codepoint >> 6);
		utf8[1] = 0x80 | (codepoint & 0x3F);
		utf8[2] = 0;
		return true;
	}

	if (codepoint < 0xFFFF) {
		if (*sz < 4)
			return false;

		*sz = 3;
		utf8[0] = 0xE0 | (codepoint >> 12);
		utf8[1] = 0x80 | (codepoint >> 6);
		utf8[2] = 0x80 | (codepoint & 0x3F);
		utf8[3] = 0;
		return true;
	}

	if (codepoint < 0x10FFFF) {
		if (*sz < 5)
			return false;

		*sz = 4;
		utf8[0] = 0xF0 | (codepoint >> 18);
		utf8[1] = 0x80 | (codepoint >> 12);
		utf8[2] = 0x80 | (codepoint >> 6);
		utf8[3] = 0x80 | (codepoint & 0x3F);
		utf8[4] = 0;
		return true;
	}

	return false;
}

#if 0
// SLOW
int utf8_codec::search_n(int n,
			 const buffer::iterator & itb0,
			 const buffer::iterator & itb_end,
			 s32 cp,
			 u64 * nr_cp,
			 u64 * nr_ok,
			 u64 * nr_err,
			 buffer::iterator * out,
			 buffer::iterator * next)
{
	int ret = 0;
	u64 n_ok = 0;
	u64 n_err = 0;

	if (n <= 0) {
		return 0;
	}

	buffer::iterator itb_next;
	buffer::iterator itb(itb0);

	// std::cerr << "---------------------------------------------\n";
	// std::cerr << __PRETTY_FUNCTION__ << "\n";

	// std::cerr << "search cp = '" << (char)cp << "'\n";
	// std::cerr << "start off    = " << itb0.offset() << "\n";

	assert(itb <= itb_end);
	while (true) {
		if (itb >= itb_end) {
			*out = itb_next;
			*next = itb;
			break;
		}

		itb_next = itb;


		// std::cerr << "decode @ " << itb_next.offset() << "\n" ;
		s32 c = inline_get_cp(itb_end, itb_next, &itb);
		// std::cerr << "decode cp = '" << (char)c << "'\n";
		if (c >= 0) {
			n_ok++;
		} else {
			n_err++;
		}

		if (c == cp) {
			if (--n == 0) {
				*out = itb_next;
				*next = itb;
				ret = 1;
				// std::cerr << "cp found : \n";
				// std::cerr << " cp   @ " << out->offset() << "\n" ;
				// std::cerr << " next @ " << next->offset() << "\n" ;
				goto out;
			}
		}
	}
out:
	*nr_ok = n_ok;
	*nr_err = n_err;
	*nr_cp = n_ok + n_err;

	// std::cerr << __PRETTY_FUNCTION__ << " , END \n";
	// std::cerr << "---------------------------------------------\n";

	return ret;
}


#endif


#if 0

// TODO: fix out/next iterator
// use for_each

int utf8_codec::search_n(int n,
			 const buffer::iterator & it0,
			 const buffer::iterator & it_end,
			 s32 cp,
			 u64 * nr_cp,
			 u64 * nr_ok,
			 u64 * nr_err,
			 buffer::iterator * out,
			 buffer::iterator * next)
{
	int ret = 0;
	u64 n_ok = 0;
	u64 n_err = 0;
	u64 n_cp = 0;

	if (n <= 0) {
		return 0;
	}

	s32 unic = 0;
	s32 state = S0;

	__flexible_array<unsigned char, memory_mapped_page<unsigned char> > * buff = it0.get_buffer();

	u64 offset = it0.offset();
	u64 prev_offset = offset;

	u64 remain = it_end.offset() - it0.offset();
	if (remain) {
		ew::core::objects::buffer::internal_page * ipg;
		ew::core::objects::buffer::real_page_type * pg;

		ew::core::objects::buffer::index_t pg_idx = it0.get_page_index();

		ipg = buff->get_page(pg_idx);
		pg   = ipg->real_page();
		pg->map();
		u8 * p = it0.get_page_iterator();
		u8 * pend = pg->end();

		while (true) {

			// goto next page ?
			if (p == pend) {
				pg->unmap();
				++pg_idx;
				if (pg_idx >= buff->get_nr_pages()) {
					// TODO: build iterator from page
					buff->get_iterator_by_offset(prev_offset, out);
					buff->get_iterator_by_offset(offset, next);
					break;
				}

				ipg = buff->get_page(pg_idx);
				pg   = ipg->real_page();
				pg->map();
				p = pg->begin();
				pend = pg->end();
			}

			utf8_decode_step(*p, state, unic);
			++p;
			++offset;

			if (state == F) {
				n_ok++;

				if (unic == cp) {

					if (--n == 0) {
						// TODO: build iterator from page
						buff->get_iterator_by_offset(prev_offset, out);
						buff->get_iterator_by_offset(offset, next);
						ret = 1;
						goto out;
					}
				}

				prev_offset = offset;

				// reset state
				state = S0;
				unic = 0;
				n_cp++;

			} else if (state == E) {

				state = S0;
				unic = 0;
				n_err++;
			}

		}

	}

	// TODO: build out iterator HERE

out:
	*nr_ok = n_ok;
	*nr_err = n_err;
	*nr_cp = n_ok + n_err;
	return ret;
}

#endif


#if 1

struct utf8_search_func_object_t {

	utf8_search_func_object_t(const u64 start_offset,
				  const s32 _search_cp,
				  int _n,
				  u64 * _nr_cp,
				  u64 * _nr_ok,
				  u64 * _nr_err)
	{
		ret = 0;
		search_cp = _search_cp;
		n      = _n;
		nr_cp  = _nr_cp;
		nr_ok  = _nr_ok;
		nr_err = _nr_err;


		pfound_offset = nullptr;

		s = S0;
		unic = 0;
		n_cp = 0;
		n_err = 0;
		n_ok = 0;

		tmp_offset    =  start_offset;
		last_start_offset =  tmp_offset;
	}


	bool operator()(u8 byte)
	{

		bool ret = true;

		utf8_decode_step(byte, s, unic);
		++tmp_offset;
		if (s == F) {

			n_ok++;

			if (unic == search_cp) {
				--n;
				if (n == 0) {
					// found
					pfound_offset = &last_start_offset;
					return false;
				}
			}

			// reset state
			s = S0;
			unic = 0;
			n_cp++;

			// save new codepoint start
			last_start_offset = tmp_offset;


		} else if (s == E) {

			n_err++;

			// reset state
			s = S0;
			unic = 0;
			n_cp++;

			// save new codepoint start
			last_start_offset = tmp_offset;

		}


		return ret;
	}

public:
	int ret;
	int n;
	u64 * nr_cp;
	u64 * nr_ok;
	u64 * nr_err;

	s32 search_cp;

	u64 tmp_offset;
	u64 last_start_offset;
	u64 * pfound_offset;

	s32 s;
	s32 unic;
	u64 n_cp;
	u64 n_err;
	u64 n_ok;


	buffer::iterator * out;
	buffer::iterator * next;

};



int utf8_codec::search_n(int n,
			 const buffer::iterator & it0,
			 const buffer::iterator & it_end,
			 s32 cp,
			 u64 * nr_cp,
			 u64 * nr_ok,
			 u64 * nr_err,
			 buffer::iterator * out,
			 buffer::iterator * next)
{
	int fret = 0;

	assert(it_end.offset() >= it0.offset());

	// std::cerr << " search_n " << "n = " << n << " , cp = " << (int)cp << "\n";

	utf8_search_func_object_t fobj(it0.offset(), cp, n, nr_cp, nr_ok, nr_err);

	fobj = it0.get_buffer()->for_each(it0, it_end, fobj);
	if (fobj.pfound_offset != nullptr) {
		fret = 1;
	}

	if (fobj.s != S0) {
		fret = -1; // error

		if (fobj.s != E) {
			/* stream truncated */
			fret = -2;
		}
	}

	// get iterator by offset
	*nr_cp  = fobj.n_cp;
	*nr_ok  = fobj.n_ok;
	*nr_err = fobj.n_err;

	// build output iterators
	it0.get_buffer()->get_iterator_by_offset(fobj.last_start_offset, out);
	it0.get_buffer()->get_iterator_by_offset(fobj.tmp_offset, next);

	//  std::cerr << " search_n " << "fret = " << fret << "\n";

	return fret;
}


#endif

#if 0

// BUGGY OUTPUT ITERATOR
// TODO: update for_each and use
// an accumulator + offsets forreverse decode

int utf8_codec::rsearch_n(int n,
			  const buffer::iterator & it0_,
			  const buffer::iterator & it_begin,
			  s32 cp,
			  u64 * nr_cp,
			  u64 * nr_ok,
			  u64 * nr_err,
			  buffer::iterator * out,
			  buffer::iterator * next)
{
	int ret = 0;

	size_t n_cp = 0;
	size_t n_error = 0;
	size_t n_ok = 0;

	s32 s = S0;
	s32 unic = 0;

	if (n <= 0) {
		return 0;
	}

	__flexible_array<unsigned char, memory_mapped_page<unsigned char> > * buff = it0_.get_buffer();
	//  ew::core::objects::buffer * buff = it0.get_buffer();

	ew::core::objects::buffer::real_page_type * pg = 0;
	ew::core::objects::buffer::real_page_type * rend_pg = 0;
	buffer::iterator it0(it0_);

	u64 offset = it0.offset();
	u64 prev_offset = it0.offset();
	if (offset >= buff->size()) {
		--it0;
	}

	u64 remain = it0.offset() - it_begin.offset();
	if (remain) {
		ew::core::objects::buffer::internal_page * ipg;
		ew::core::objects::buffer::index_t pg_idx = it0.get_page_index();

		//
		ew::core::objects::buffer::internal_page * rend_ipg = 0;
		ew::core::objects::buffer::index_t rend_pg_idx = -1;
		//

		ipg  = buff->get_page(pg_idx);
		pg   = ipg->real_page();
		pg->map();
		// bret = pg->map()

		u8 * p = it0.get_page_iterator();
		u8 * pbegin = pg->begin();

		// NEW : prefetch previous page
		size_t diff = p - pbegin;
		u8 * p_rend = p - 4; // maximum codepoint size
		if (diff < 4) {
			if (pg_idx) {
				rend_pg_idx = pg_idx - 1;
				rend_ipg = buff->get_page(rend_pg_idx);
				rend_pg  = ipg->real_page();
				rend_pg->map();
				p_rend = pg->end() - diff;
			}
		} else {
			rend_pg_idx = pg_idx;
			rend_ipg = buff->get_page(rend_pg_idx);
			rend_pg  = ipg->real_page();
			rend_pg->map();
		}

		// NEW
		// std::cerr << __FUNCTION__  << " ------------------------------\n";
		// std::cerr << __FUNCTION__  << " LOOKING FOR '" << (s32)cp << "'\n";
		// std::cerr << __FUNCTION__  << " BEGIN LOOP\n";

		while (remain--) {

			// goto previous page ?
			if (p == pbegin - 1) {
				pg->unmap();
				if (pg_idx == 0) {
					pg = 0;
					buff->get_iterator_by_offset(offset, out);
					buff->get_iterator_by_offset(prev_offset, next);
					break;
				}
				--pg_idx;
				ipg = buff->get_page(pg_idx);
				pg   = ipg->real_page();
				pg->map();
				p = pg->end() - 1;
				pbegin = pg->begin();
			}

			// std::cerr << " read '" << (s32)*p << "'\n";
			// std::cerr << " before unic '" << unic << "' s = " << (int)s << "\n";

			utf8_reverse_decode_step(*p, s, unic);

			// std::cerr << " after unic '" << unic << "' s = " << (int)s << "\n";

			// likely
			if (s == F) {

				n_ok++;

				if (unic == cp) {

					// std::cerr << " found '" << unic << "'\n";

					if (--n == 0) {

						// get iterator by offset
						// TODO: build iterator from page
						buff->get_iterator_by_offset(offset, out);
						buff->get_iterator_by_offset(prev_offset, next);
						ret = 1;
						goto out;
					}
				}

				prev_offset = offset;

				// reset state
				s = S0;
				unic = 0;
				++n_cp;

			} else if (s == E) {

				n_error++;

				// reset state
				s = S0;
				unic = 0;
				++n_cp;
			}

			--p;
			--offset;

		}

		// std::cerr << __PRETTY_FUNCTION__  << " END LOOP\n";

	}
out:

	if (pg)
		pg->unmap();

	*nr_cp    = n_cp;
	*nr_ok    = n_ok;
	*nr_err   = n_error;

	return ret;
}

#else

int utf8_codec::rsearch_n(int n,
			  const buffer::iterator & itb0,
			  const buffer::iterator & itb_begin,
			  s32 cp,
			  u64 * nr_cp,
			  u64 * nr_ok,
			  u64 * nr_err,
			  buffer::iterator * out,
			  buffer::iterator * next)
{
	int ret = 0;
	u64 n_ok = 0;
	u64 n_err = 0;

	if (n <= 0) {
		return 0;
	}

	buffer::iterator itb_next;
	buffer::iterator itb = itb0;

	while (itb != itb_begin) {
		itb_next = itb;

		// TODO: utf8_reverse_decode_step(*it, s, unic);

		s32 c = rget_cp(itb_begin, itb_next, &itb);
		if (c >= 0)
			n_ok++;
		else
			n_err++;

		if (c == cp) {
			if (--n == 0) {
				*out = itb;
				*next = itb_next;
				ret = 1;
				goto out;
			}
		}
	}
out:
	*nr_ok = n_ok;
	*nr_err = n_err;
	*nr_cp = n_ok + n_err;
	return ret;
}


// debug purpose
EW_CODECS_TEXT_UNICODE_UTF8_EXPORT u32 decode(u8 * s, size_t size)
{
	u32 len = 0;
	s32 cp = 0;
	s32 state = S0;
	u8 * end = s + size;

	u8 byte;
	for (; s != end; ++s) {

		if ((byte = *s) == 0) {
			break;
		}

		utf8_decode_step(byte, state, cp);

		if (state == F) {
			cp = 0;
			state = S0;
		}

		if (state == E) {
			cp = 0;
			state = S0;
		}

		++len;
	}

	return len;
}

#endif

}  // ! namespace utf8
}  // ! namespace text
}  // ! namespace unicode
}  // ! namespace codecs
}  // ! namespace ew
