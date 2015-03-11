#pragma once

#include <ew/core/types/types.hpp>
using ew::core::types::u8;

#include <ew/core/object/buffer.hpp>


#include <ew/codecs/text/textcodec.hpp>

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


// must found a way to use template for all utf8 functions
// based o, iterators
// and for special containers use specialization :-)
// remove the virtual keyword

// buffer
using namespace ew::core::objects;

bool init();

ew::codecs::text::text_codec * codec();

EW_CODECS_TEXT_UNICODE_UTF8_EXPORT u32 decode(u8 * in, size_t size); // INTERNAL DO NOT USE

class utf8_codec : public ew::codecs::text::text_codec
{
public:
	utf8_codec(); // call init
	virtual ~utf8_codec();

	virtual size_t maximum_codepoint_size()
	{
		return 4;
	}

	// new
	virtual const u8 * get_state_table();
	virtual const u8 * get_byte_mask_table();
	virtual u32  get_cp_shift();
	virtual u32  get_state_shift();

	virtual int  get_starting_state();
	virtual int  get_finish_state();
	virtual int  get_error_state();
	virtual int  step(const byte byte, int * state, int * cp);

	// TODO: rstep(byte byte, int * state, int * cp);
	// TODO: put_step(const int cp, int * state, byte * out);



	/* read backward */
	virtual s32 rget_cp(const u8 * start, const u8 * in, u8 ** out); // out- > next/prev ?

	/* encode */
	virtual bool (put_cp)(const s32 codepoint, u8 * out, u32 * out_size);


	/* like search but count number of codepoint <c> in range */
	// virtual int count(const u8 * buffer, const u32 len, const s32 c, u64 * nr_ok, u64 * nr_error, u32 * count, u8 ** next);

	/*
	  offsets : for each code point its start offset will be stored
	*/
	// virtual int read(const u8 * buffer, const u32 len,  s32 * cpvec, u32 max, u64 * offset, u32 * nrcp,  u64 * nr_ok, u64 * nr_error, u8 ** next);
	// virtual int search_n(int n, u8 * start, u8 * end, s32 cp, u64 * nr_cp, u64 * nr_ok, u64 * nr_err, u8 ** out, u8 ** next);
	// REMOVE THIS
	// virtual int rsearch_n(int n, u8 * rstart, u8 * begin, s32 cp, u64 * nr_cp,  u64 * nr_ok, u64 * nr_err, u8 ** out, u8 ** next);

	// REMOVE THIS
	virtual int count(const u8 * buffer, const u32 len, const s32 c, u64 * nr_ok, u64 * nr_error, u32 * count, u8 ** next);

	////////////////////////////////////////////////////////////////////////////////
	// buffer::iterators
	// buffer::iteartor insteat of pointers
	////////////////////////////////////////////////////////////////////////////////
	//virtual s32(get_cp)(const buffer::iterator & end,  const buffer::iterator & in , buffer::iterator * next);
	virtual s32  rget_cp(const buffer::iterator & start,  const buffer::iterator & in , buffer::iterator * out);

	virtual int  search_n(int n, const buffer::iterator & itb, const buffer::iterator  & itb_end, s32 cp, u64 * nr_cp, u64 * nr_ok, u64 * nr_err,
			      buffer::iterator * out, buffer::iterator * next);

	virtual int  rsearch_n(int n, const buffer::iterator & itb, const buffer::iterator & itb_begin, s32 cp, u64 * nr_cp,  u64 * nr_ok,  u64 * nr_err,
			       buffer::iterator * out, buffer::iterator * next);

	// e - b = 4Gib max
	virtual int  count_new_line(const u8 * b, const u8 * e, u8 & prev_byte, u64 & counter);
};

}
}
}
}
}
