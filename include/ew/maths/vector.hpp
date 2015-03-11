#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

namespace ew
{
namespace maths
{
using namespace ew::core::types;


template <typename T, u32 size>
class Vector;

typedef Vector<s32, 2> Vector2i;
typedef Vector<s32, 3> Vector3i;
typedef Vector<s32, 4> Vector4i;

typedef Vector<f32, 2> Vector2f;
typedef Vector<f32, 3> Vector3f;
typedef Vector<f32, 4> Vector4f;

typedef Vector<double, 2> Vector2d;
typedef Vector<double, 3> Vector3d;
typedef Vector<double, 4> Vector4d;

// GLSL compatible ??
typedef Vector<s32, 2> ivec2;
typedef Vector<s32, 3> ivec3;
typedef Vector<s32, 4> ivec4;

typedef Vector<f32, 2> vec2;
typedef Vector<f32, 3> vec3;
typedef Vector<f32, 4> vec4;

typedef Vector<double, 2> vec2d;
typedef Vector<double, 3> vec3d;
typedef Vector<double, 4> vec4d;

// GLSL filters ?
typedef Vector<bool, 2> bvec2;
typedef Vector<bool, 3> bvec3;
typedef Vector<bool, 4> bvec4;


template <typename T, u32 size>
class EW_MATHS_EXPORT Vector
{
public:
	inline Vector<T, size>(void);
	inline Vector<T, size>(const T * vec);
	inline Vector<T, size>(const Vector<T, size> & vec);

	inline T * GetDataPtr(void);

	inline const T & operator[](const u32 i) const;
	inline T & operator[](const u32 i);
	inline s32 operator==(const Vector<T, size> & vec);
	inline s32 operator!=(const Vector<T, size> & vec);

	inline Vector<T, size> operator+(const Vector<T, size> & vec);
	inline Vector<T, size> operator-(const Vector<T, size> & vec);

	inline Vector<T, size> operator*(const T & val);
	inline Vector<T, size> operator/(const T & val);
	inline Vector<T, size> operator+(const T & val);
	inline Vector<T, size> operator-(const T & val);

	inline Vector<T, size> & operator*=(const T & val);
	inline Vector<T, size> & operator/=(const T & val);
	inline Vector<T, size> & operator+=(const T & val);
	inline Vector<T, size> & operator-=(const T & val);

	inline Vector<T, size> & operator+=(const Vector<T, size> & vec);
	inline Vector<T, size> & operator-=(const Vector<T, size> & vec);
	inline Vector<T, size> & operator=(const Vector<T, size> & vec);

	inline Vector<T, size> & operator=(const T * vec);

	inline T Length() const;
	inline T Dot(const Vector<T, size> & vec);

	inline Vector<T, size> & Normalize(void);
	inline Vector<T, size> & zero(void);

private:   // should be public ?

	T m_vec[ size ];

};

// ---------------------------------------------------------------------------------------------------

template <typename T>
class EW_MATHS_EXPORT Vector<T, 2>
{
public:
	inline Vector(void);
	inline Vector(const T * vec);
	inline Vector(const Vector<T, 2> & vec);
	inline Vector(const T x_, const T y_);

	inline const T & operator[](const u32 i) const;
	inline T & operator[](const u32 i);

	inline T * GetDataPtr(void);
	inline const T * GetDataPtr(void) const;

	inline s32 operator==(const Vector<T, 2> & vec);
	inline s32 operator!=(const Vector<T, 2> & vec);

	inline Vector<T, 2> operator-() const;

	inline Vector<T, 2> operator+(const Vector<T, 2> & vec) const;
	inline Vector<T, 2> operator-(const Vector<T, 2> & vec) const;

	inline Vector<T, 2> operator*(const T & val) const;
	inline Vector<T, 2> operator/(const T & val) const;
	inline Vector<T, 2> operator+(const T & val) const;
	inline Vector<T, 2> operator-(const T & val) const;

	inline Vector<T, 2> & operator*=(const T & val);
	inline Vector<T, 2> & operator/=(const T & val);
	inline Vector<T, 2> & operator+=(const T & val);
	inline Vector<T, 2> & operator-=(const T & val);

	inline Vector<T, 2> & operator+=(const Vector<T, 2> & vec);
	inline Vector<T, 2> & operator-=(const Vector<T, 2> & vec);

	inline Vector<T, 2> & operator=(const Vector<T, 2> & vec);
	inline Vector<T, 2> & operator=(const T * vec);
	//
	inline T Length() const;
	inline T Dot(const Vector<T, 2> & vec) const;
	inline Vector<T, 2> & Normalize();
	//
	inline Vector<T, 2> & zero(void);
	inline Vector<T, 2> & set
	(const T & x_, const T & y_);
	inline Vector<T, 2> & set
	(const T * vec);

public:
	union { /* _anonymous */
		struct { // vector component
			T x;
			T y;
		};
		struct { // for texture coord ?? should be in vertex
			T u;
			T v;
		};
		T m_vec[ 2 ];
	};
};

template <typename T>
inline Vector<T, 2> operator-(Vector<T, 2> & v);

// ---------------------------------------------------------------------------------------------------

template <typename T>
class EW_MATHS_EXPORT Vector<T, 3>
{
public:
	inline Vector(void);
	inline Vector(const T * vec);
	inline Vector(const Vector<T, 3> & vec);
	inline Vector(const T x_, const T y_ = 0, const T z_ = 0);

	inline const T & operator[](const u32 i) const;
	inline T & operator[](const u32 i);

	inline T * GetDataPtr(void);
	inline const T * GetDataPtr(void) const;

	inline Vector<T, 3> operator-() const ;

	inline s32 operator==(const Vector<T, 3> & vec) const ;
	inline s32 operator!=(const Vector<T, 3> & vec) const ;

	inline Vector<T, 3> operator+(const Vector<T, 3> & vec) const;
	inline Vector<T, 3> operator-(const Vector<T, 3> & vec) const ;

	inline Vector<T, 3> operator*(const T & val) const;
	inline Vector<T, 3> operator/(const T & val) const;
	inline Vector<T, 3> operator+(const T & val) const;
	inline Vector<T, 3> operator-(const T & val) const;

	inline Vector<T, 3> & operator*=(const T & val);
	inline Vector<T, 3> & operator/=(const T & val);
	inline Vector<T, 3> & operator+=(const T & val);
	inline Vector<T, 3> & operator-=(const T & val);

	inline Vector<T, 3> & operator+=(const Vector<T, 3> & vec);
	inline Vector<T, 3> & operator-=(const Vector<T, 3> & vec);

	inline Vector<T, 3> & operator=(const Vector<T, 3> & vec);
	inline Vector<T, 3> & operator=(const T * vec);

	inline T Length() const;
	inline T Dot(const Vector<T, 3> & vec) const;
	inline Vector<T, 3> & Normalize();

	inline Vector<T, 3> & zero(void);
	inline Vector<T, 3> & set
	(const T x_, const T y_, const T z_, const T w_);
	inline Vector<T, 3> & set
	(const T * vec);

public:
	union { /* _anonymous */
		struct {
			T x;
			T y;
			T z;
		};
		T m_vec[ 3 ];
	};
};

template <typename T>
inline Vector<T, 3> operator-(Vector<T, 3> & v);

// --------------------------------------------------------------------

template <typename T>
class EW_MATHS_EXPORT Vector<T, 4>
{
public:
	inline Vector(void);
	inline Vector(const T * vec);
	inline Vector(const Vector<T, 4> & vec);
	inline Vector(const T x_, const T y_ = 0, const T z_ = 0, const T w_ = 0);

	inline const T & operator[](const u32 i) const;
	inline T & operator[](const u32 i);

	inline T * GetDataPtr(void);
	inline const T * GetDataPtr(void) const;

	inline s32 operator==(const Vector<T, 4> & vec);
	inline s32 operator!=(const Vector<T, 4> & vec);

	inline Vector<T, 4> operator-() const;

	inline Vector<T, 4> operator+(const Vector<T, 4> & vec) const;
	inline Vector<T, 4> operator-(const Vector<T, 4> & vec) const;
	inline Vector<T, 4> operator*(const T & val) const;
	inline Vector<T, 4> operator/(const T & val) const;

	inline Vector<T, 4> & operator*=(T val);
	inline Vector<T, 4> & operator/=(T val);
	inline Vector<T, 4> & operator+=(T val);
	inline Vector<T, 4> & operator-=(T val);

	inline Vector<T, 4> & operator+=(Vector<T, 4> & vec);
	inline Vector<T, 4> & operator-=(Vector<T, 4> & vec);

	inline Vector<T, 4> & operator=(const Vector<T, 4> & vec);
	inline Vector<T, 4> & operator=(const T * vec);

	inline T Length() const;
	inline T Dot(const Vector<T, 4> & vec) const;
	inline Vector<T, 4> & Normalize();

	inline Vector<T, 4> & zero(void);
	inline Vector<T, 4> & set
	(T x_, T y_, T z_, T w_);
	inline Vector<T, 4> & set
	(T * vec);

	//       T & x() { return x; }
	//       T & y() { return y; }
	//       T & z() { return z; }
	//       T & w() { return w; }
private:
public:
	T x;
	T y;
	T z;
	T w;
};

template <typename T>
inline Vector<T, 4> operator-(const Vector<T, 4> & v);


// ---------------------------------- Vector Operations ---------------------------------------------------
// Dot product
template <typename T, u32 size>
inline T Dot(const Vector<T, size> & v1, const Vector<T, size> & v2);

template <typename T>
inline
T Dot(const Vector<T, 2> & v1, const Vector<T, 2> & v2);

template <typename T>
inline T Dot(const Vector<T, 3> & v1, const Vector<T, 3> & v2);

template <typename T>
inline T Dot(const Vector<T, 4> & v1, const Vector<T, 4> & v2);

// --------------------------------------------------------------------------------------------------------
// Cross product

// ugly
#ifndef EW_VECTOR_CROSS_RETURN_REFERENCE

template <typename T>
inline void Cross(Vector<T, 3> & res, const Vector<T, 3> & v1, const Vector<T, 3> & v2);
#else

template <typename T>
inline Vector<T, 3> & Cross(Vector<T, 3> & res, const Vector<T, 3> & v1, const Vector<T, 3> & v2);
#endif
// slow
template <typename T>
inline Vector<T, 3> Cross(const Vector<T, 3> & v1, const Vector<T, 3> & v2);

// ----------------------------------------------------------------------------------------------------------

}
} // ! ew::maths


#include <ew/maths/vector.hxx>
