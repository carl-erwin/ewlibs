#pragma once

#include <ew/core/types/types.hpp>

using namespace ew::core::types;

inline bool insert_byte(u8 * p,  u8 * pend, u8 ** out, byte val)
{
	bool ret = false;
	if (p < pend) {
		*p++ = val;
		ret = true;
	}

	*out = p;
	return ret;
}

inline bool insert_c_string(u8 * p,  u8 * pend, u8 ** out, const char * str, size_t len)
{
	bool ret = false;
	u8 * limit = p + len + 1;

	if (limit < pend) {
		memcpy(p, str, len);
		p[len] = '\0';
		p += len + 1;
		ret = true;
	}

	*out = p;
	return ret;
}

inline bool expect_byte(u8 * p,  u8 * pend, u8 ** out, byte val)
{
	bool ret = false;
	if (p < pend) {
		if (*p == val) {
			++p;
			ret = true;
		}
	}

	*out = p;
	return ret;
}

inline bool extract_bytes(u8 * p, u8 * pend, u8 ** pout, void * val, u32 size)
{
	bool res = false;
	char * pbyte = (char *)val;

	if ((p + size) <= pend) {
		memcpy(pbyte, p, size);
		*pout = p + size;
		res = true;
	}

	assert(res == true);
	return res;
}

inline bool extract_byte(u8 * p, u8 * pend, u8 ** pout, u8 * val)
{
	return extract_bytes(p, pend, pout, val, sizeof(u8));
}

inline bool extract_char(u8 * p, u8 * pend, u8 ** pout, char * val)
{
	return extract_bytes(p, pend, pout, val, sizeof(char));
}

inline bool extract_u16(u8 * p, u8 * pend, u8 ** pout, u16 * val)
{
	bool res = extract_bytes(p, pend, pout, val, sizeof(u16));
	if (res == true) {
		*val = ntohs(*val);
	}
	return res;
}

inline bool extract_u32(u8 * p, u8 * pend, u8 ** pout, u32 * val)
{
	bool res = extract_bytes(p, pend, pout, val, sizeof(u32));
	if (res == true) {
		*val = ntohl(*val);
	}
	return res;
}
