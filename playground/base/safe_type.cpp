#include <iostream>

#include "ew/core/types/types.hpp"
using namespace ew::core::types;

namespace playground
{


template <typename T> class /*ew::core::types::*/ safe_base_type /* : public ew::core::objects::IObject ? */ ;

typedef safe_base_type<u8>  U8;
typedef safe_base_type<u16> U16;
typedef safe_base_type<u32> U32;
typedef safe_base_type<u64> U64;

typedef safe_base_type<s8>  S8;
typedef safe_base_type<s16> S16;
typedef safe_base_type<s32> S32;
typedef safe_base_type<s64> S64;


template <typename T> bool is_signed(T)
{
	return true;
}

template <> bool is_signed(u8 v)
{
	return false;
}
template <> bool is_signed(u16 v)
{
	return false;
}
template <> bool is_signed(u32 v)
{
	return false;
}
template <> bool is_signed(u64 v)
{
	return false;
}

template <typename T>
class /*ew::core::types::*/ safe_base_type   /* : public ew::core::objects::IObject ? */
{
public:
	inline safe_base_type(const T val = 0) : _val(val) {}
	inline safe_base_type(const safe_base_type<T> & orig) : _val(orig._val) {}

	inline T & operator=(T & val)
	{
		_val = val;
		return *this;
	}

	///-----
	inline T   operator+(u8  & val)
	{
		return _val + val;
	}
	inline T   operator+(u16 & val)
	{
		return _val + val;
	}
	inline T   operator+(u32 & val)
	{
		return _val + val;
	}
	inline T   operator+(u64 & val)
	{
		return _val + val;
	}

	///-----
	inline T   operator-(u8  & val)
	{
		return _val - val;
	}
	inline T   operator-(u16 & val)
	{
		return _val - val;
	}
	inline T   operator-(u32 & val)
	{
		return _val - val;
	}
	inline T   operator-(u64 & val)
	{
		return _val - val;
	}

	///----- to expand
	inline T   operator*(T & val)
	{
		return _val * val;
	}
	inline T   operator/(T & val)
	{
		return _val / val;
	}
	inline T   operator%(T & val)
	{
		return _val % val;
	}
	inline T   operator|(T & val)
	{
		return _val | val;
	}
	inline T   operator&(T & val)
	{
		return _val & val;
	}
	inline T   operator^(T & val)
	{
		return _val ^ val;
	}
	inline T   operator~()
	{
		return ~_val;
	}
	//


	inline T  & operator*=(T & val)
	{
		_val *= val;
		return *this;
	}
	inline T  & operator/=(T & val)
	{
		_val /= val;
		return *this;
	}
	inline T  & operator%=(T & val)
	{
		_val %= val;
		return *this;
	}
	inline T  & operator|=(T & val)
	{
		_val |= val;
		return *this;
	}
	inline T  & operator&=(T & val)
	{
		_val &= val;
		return *this;
	}
	inline T  & operator^=(T & val)
	{
		_val ^= val;
		return *this;
	}


	// yahoooo !!!!!!!!!!!!!!!!!!!!
	inline T  & operator-=(u8 & val)
	{
		_val -= val;
		return *this;
	}
	inline T  & operator-=(u16 & val)
	{
		_val -= val;
		return *this;
	}
	inline T  & operator-=(u32 & val)
	{
		_val -= val;
		return *this;
	}
	inline T  & operator-=(u64 & val)
	{
		_val -= val;
		return *this;
	}
	inline T  & operator-=(s8 & val)
	{
		_val -= val;
		return *this;
	}
	inline T  & operator-=(s16 & val)
	{
		_val -= val;
		return *this;
	}
	inline T  & operator-=(s32 & val)
	{
		_val -= val;
		return *this;
	}
	inline T  & operator-=(s64 & val)
	{
		_val -= val;
		return *this;
	}


	inline T  & operator+=(u8 & val)
	{
		_val += val;
		return *this;
	}
	inline T  & operator+=(u16 & val)
	{
		_val += val;
		return *this;
	}
	inline T  & operator+=(u32 & val)
	{
		_val += val;
		return *this;
	}
	inline T  & operator+=(u64 & val)
	{
		_val += val;
		return *this;
	}
	inline T  & operator+=(s8 & val)
	{
		_val += val;
		return *this;
	}
	inline T  & operator+=(s16 & val)
	{
		_val += val;
		return *this;
	}
	inline T  & operator+=(s32 & val)
	{
		_val += val;
		return *this;
	}
	inline T  & operator+=(s64 & val)
	{
		_val += val;
		return *this;
	}

	//  inline T &  operator~=(T & val) { _val ~= val; return *this; }

	inline T & operator++()
	{
		return ++_val;
	}
	inline T & operator--()
	{
		return --_val;
	}

	inline T operator++(int)
	{
		T ret = _val;
		_val += 1;
		return ret;
	}

	inline T operator--(int)
	{
		T ret = _val;
		_val -= 1;
		return ret;
	}


	// cast ops
	operator int()
	{
		if (is_signed(_val))
			return (int)_val;
		std::cerr << "ERROR 1 \n" << __FUNCTION__ << "\n";
		throw "bad cast";
	};

	inline operator u8()     // std::cerr << "operator u8  () ";
	{
		return static_cast<u8>(_val);
	}

	inline operator u16()    // std::cerr << "operator u16 () ";
	{
		return static_cast<u16>(_val);
	}

	inline operator u32()   //  std::cerr << "operator u32 () ";
	{
		if (is_signed(_val)) {
			std::cerr << "ERROR 2 \n";
			throw "bad cast";
		}

		if (sizeof(T) > sizeof(u32)) {
			if (_val & 0xffffffff00000000LL) {
				std::cerr << "ERROR 3 \n";
				throw "bad cast";
			}
		}

		return static_cast<u32>(_val);
	}

	inline operator u64()   //  std::cerr << "operator u64 () ";
	{
		return static_cast<u64>(_val);
	}


	//private:
public:
	T _val;

public:
	//friend
	template <class A> friend std::ostream & operator<< (std::ostream & stream, const safe_base_type<A> & val);

	friend std::ostream & operator<< (std::ostream & stream, const U8 & val);

	template <class A> friend bool operator< (safe_base_type<A> & a,  safe_base_type<A> & b);
	template <class A> friend bool operator<= (safe_base_type<A> & a,  safe_base_type<A> & b);
	template <class A> friend bool operator> (safe_base_type<A> & a,  safe_base_type<A> & b);
	template <class A> friend bool operator>= (safe_base_type<A> & a,  safe_base_type<A> & b);
};

template <> bool is_signed(U8 v)
{
	return false;
}
template <> bool is_signed(U16 v)
{
	return false;
}
template <> bool is_signed(U32 v)
{
	return false;
}
template <> bool is_signed(U64 v)
{
	return false;
}

template <class T>
bool operator< (safe_base_type<T> & a, safe_base_type<T> & b)
{
	return a._val < b._val;
}

template <class T>
bool operator<= (const safe_base_type<T> & a, const safe_base_type<T> & b)
{
	return a._val <= b._val;
}

template <class T>
bool operator> (const safe_base_type<T> & a, const safe_base_type<T> & b)
{
	return a._val > b._val;
}

template <class T>
bool operator >= (const safe_base_type<T> & a, const safe_base_type<T> & b)
{
	return a._val >= b._val;
}

//
std::ostream & operator<<(std::ostream & stream, const U8 & val)
{
	stream << (u32)val._val;
	return stream;
}

std::ostream & operator<<(std::ostream & stream, const U16 & val)
{
	stream << val._val;
	return stream;
}


std::ostream & operator<<(std::ostream & stream, const U32 & val)
{
	stream << val._val;
	return stream;
}

std::ostream & operator<<(std::ostream & stream, const U64 & val)
{
	stream << val._val;
	return stream;
}

std::ostream & operator<<(std::ostream & stream, const S8 & val)
{
	stream << (s32)val._val;
	return stream;
}

std::ostream & operator<<(std::ostream & stream, const S16 & val)
{
	stream << val._val;
	return stream;
}


std::ostream & operator<<(std::ostream & stream, const S32 & val)
{
	stream << val._val;
	return stream;
}

std::ostream & operator<<(std::ostream & stream, const S64 & val)
{
	stream << val._val;
	return stream;
}

//////

S32 Memory_Block[  ] = {0, 1 , 2, 3, 4, 5, 6, 7, 8, 9};
#define SIZE (sizeof(Memory_Block)/sizeof(Memory_Block[0]))

int main(int ac, char ** av)
{
	U8 _u8;
	U16 _u16;
	U32 _u32;
	U64 _u64(64);

	_u8 = 8;
	_u16 = 16;
	_u32 = 32;
	_u64 = 64;


	//  unsigned char c = ;
	//  _u8 /= 0;

	std::cerr << "U8 "  << _u8  << "\n";
	std::cerr << "U16 " << _u16 << "\n";
	std::cerr << "U32 " << _u32 << "\n";
	std::cerr << "U64 " << _u64 << "\n";

	std::cerr << "SIZE " << SIZE << "\n";

	if (!is_signed(_u8)) {
		std::cerr << "_u8 is not signed \n";
	}

	for (size_t i = 0; i < SIZE; i++) {
		std::cerr << "i =  " << i << "\n";
		// Memory_Block[ i ] += s32(1); // TODO: fix build
	}

	return 0;
}


} // ! namespace playground
