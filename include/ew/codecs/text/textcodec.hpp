#pragma once

#include <ew/core/types/types.hpp>
using namespace ew::core::types;
#include <ew/core/object/buffer.hpp>

namespace ew
{
namespace codecs
{
namespace text
{

using ew::core::objects::buffer;

enum text_codec_status {
	INVALID_CP   = -1,  // last codepoint was invalid
	TRUNCATED_CP = -2,  // last codepoint was truncated
	CP_DECODED   = 0,   // last codepoint was valid
};


class EW_CODECS_TEXT_UNICODE_UTF8_EXPORT text_codec
{
public:
	virtual ~text_codec() {}
	virtual size_t maximum_codepoint_size() = 0;

	/* wip : new text codec api */
	virtual const u8 * get_state_table()     = 0;
	virtual const u8 * get_byte_mask_table() = 0;
	virtual u32  get_cp_shift()              = 0;
	virtual u32  get_state_shift()           = 0;

	virtual int  get_starting_state()        = 0;
	virtual int  get_finish_state()          = 0;
	virtual int  get_error_state()           = 0;

	virtual int  step(const byte byte, int * state, int * cp) = 0;
	// virtual int r_step(byte byte, int * state, int * cp) = 0; /* this will be the core of text codec */

// TODO: REMOVE THIS
	virtual s32 rget_cp(const u8 * start, const u8 * in, u8 ** out) = 0; // out- > next/prev ?

	/* encode */
	// UPDATE THIS
	virtual bool (put_cp)(const s32 codepoint, u8 * out, u32 * out_size) = 0;

	// REMOVE THIS
	virtual s32  rget_cp(const buffer::iterator & start,  const buffer::iterator & in , buffer::iterator * out) = 0;


	// TODO: remove this
	virtual int  rsearch_n(int n, const buffer::iterator & itb, const buffer::iterator  & itb_begin, s32 cp, u64 * nr_cp,  u64 * nr_ok,  u64 * nr_err, buffer::iterator * out, buffer::iterator * next) = 0;

	virtual int  count_new_line(const u8 * b, const u8 * e, u8 & prev_byte, u64 & counter) = 0;
};

/**/
template <class ByteIterator>
inline ByteIterator get_cp(text_codec * C, ByteIterator src, ByteIterator end, int * cp)
{
	*cp              = 0; // C->cp_start();
	int state        = C->get_starting_state();
	const int finish_state = C->get_finish_state();
	const int error_state  = C->get_error_state();

	auto  state_tab   = C->get_state_table();
	auto  byte_mask   = C->get_byte_mask_table();
	const u32 cp_shift     = C->get_cp_shift();
	const u32 state_shift  = C->get_state_shift();

	ByteIterator start = src;
	while (src < end) {

		*cp = (*cp << cp_shift) | byte_mask[*src];
		state = state_tab[(state << state_shift) + *src];
		++src;
		if (state == error_state) {
			*cp = -1;
			return start + 1; // invalid seq resync 1 byte
		}

		if (state == finish_state) {
			return src;
		}
	}

	return src;
}

/* functor ? */
template <class ByteIterator>
inline ByteIterator decode(text_codec * C, ByteIterator src, ByteIterator end, u32 * len /*, u64 * _count = nullptr*/)
{

	int cp                 = 0; // C->cp_start();
	const int start_state  = C->get_starting_state();
	const int finish_state = C->get_finish_state();
	const int error_state  = C->get_error_state();
	int state              = start_state;

	auto state_tab         = C->get_state_table();
	auto byte_mask         = C->get_byte_mask_table();
	const u32 cp_shift     = C->get_cp_shift();
	const u32 state_shift  = C->get_state_shift();

	u64 count = 0;
	*len = 0;
	while (src < end) {
		if (*src == 0)
			break;

		const u8 byte = *src;
#if 0
		C->step(byte, &state, &cp);
#else
		cp = (cp << cp_shift) | byte_mask[byte];
		state = state_tab[(state << state_shift) + byte];
#endif
		if (state == finish_state) {

			++count;
			state = start_state;
			cp = 0;
		}

		if (state == error_state) {
			state = start_state;
			cp = 0;
		}

		++src;
		++(*len);
	}

	/*
	if (_count) {
	  *_count = count;
	}
	*/

	return src;
}

/*
 *  returns  if true + out if found else false
 *
 *  BUGGY
 *
 */
template <class ByteIterator>
inline bool search_n(text_codec * C,
		     u64 occur, ByteIterator from, const ByteIterator end,
		     int cp,
		     u64 & nr_ok, u64 & nr_err, ByteIterator & out)
{
	if (occur == 0)
		return false;

	nr_ok  = 0;
	nr_err = 0;

	int current_cp   = 0; // reset codepoint
	const int start_state = C->get_starting_state();
	const int finish_state = C->get_finish_state();
	const int error_state  = C->get_error_state();

#if 1
	auto state_tab   = C->get_state_table();
	auto byte_mask   = C->get_byte_mask_table();
	const u32 cp_shift     = C->get_cp_shift();
	const u32 state_shift  = C->get_state_shift();
#endif

	int state  = start_state;

	// encode cp
	u8  cp_encoded[8];
	u32 cp_encoded_size = 7;

	C->put_cp(cp, cp_encoded,   &cp_encoded_size);

	assert(cp_encoded_size);
	cp_encoded_size--;

	ByteIterator start = from;
	while (from < end) {
		const u8 byte = *from;
		/// C->step(*from, &state, &current_cp);
		current_cp = (current_cp << cp_shift) | byte_mask[byte];
		state = state_tab[(state << state_shift) + byte];

		if (state == finish_state) {

			nr_ok++;
			if (current_cp == cp) {
				if (--occur == 0) {
					while (cp_encoded_size > 0) {
						--from;
						--cp_encoded_size;
					}
					out = from;
					return true;
				}
			}

			current_cp = 0;
			state      = start_state;
			start      = from;
		}

		if (state == error_state) {
			nr_err++;
			current_cp = 0;
			state      = start_state;
			from       = start;
			++start;
		}

		++from;
	}

	return false;
}


}
}
}
