#pragma once

// this file will hold : Abs,Min,Max, floor, Ceil etc

namespace ew
{
namespace maths
{

template <typename T>
inline T abs(T value)
{
	return (value > 0 ? value : -(value));
}

template <typename T>
inline T min(const T a, const T b)
{
	return a < b ? a : b;
}

template <typename T>
inline T max(const T a, const T b)
{
	return a > b ? a : b;
}

template <typename T>
inline T in_range(const T min, const T val, const T max)
{
	if (min > max)
		return in_range(max, val, min);
	if (val < min)
		return min;
	if (val > max)
		return max;

	return val;
}

}
}
