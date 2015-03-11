#pragma once


#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>

#include <ew/ew_config.hpp>

#include <ew/core/types/types.hpp>
using namespace ew::core::types;

#include <ew/maths/convert.hpp>

#include "vector.hpp"

namespace ew
{
namespace maths
{

template <typename T, u32 M, u32 N >
class Matrix;

template <typename T>
class Matrix<T, 4, 4>;

typedef Matrix<s32, 2, 2> Matrix22i;
typedef Matrix<s32, 2, 3> Matrix23i;
typedef Matrix<s32, 3, 3> Matrix33i;
typedef Matrix<s32, 3, 4> Matrix34i;
typedef Matrix<s32, 4, 4> Matrix44i;

typedef Matrix<f32, 2, 2> Matrix22f;
typedef Matrix<f32, 2, 3> Matrix23f;
typedef Matrix<f32, 3, 3> Matrix33f;
typedef Matrix<f32, 3, 4> Matrix34f;
typedef Matrix<f32, 4, 4> Matrix44f;

typedef Matrix<double, 2, 2> Matrix22d;
typedef Matrix<double, 2, 3> Matrix23d;
typedef Matrix<double, 3, 3> Matrix33d;
typedef Matrix<double, 3, 4> Matrix34d;
typedef Matrix<double, 4, 4> Matrix44d;

// glsl ?
typedef Matrix22i mat2i;
typedef Matrix33i mat3i;
typedef Matrix44i mat4i;

typedef Matrix22f mat2f;
typedef Matrix33f mat3f;
typedef Matrix44f mat4f;

typedef Matrix22d mat2d;
typedef Matrix33d mat3d;
typedef Matrix44d mat4d;

typedef Matrix22f mat2;
typedef Matrix33f mat3;
typedef Matrix44f mat4;

// -------------------------------------------------------------------------------------------

// general class
template <typename T, u32 M, u32 N >
class EW_EXPORT_SYM Matrix
{
public:
	inline Matrix(void);
	inline Matrix(const T & x);
	inline Matrix(const T * m);
	inline const T & operator[](const u32 i) const;
	inline const T & operator()(const u32 i, const u32 j) const;
	inline T & operator[](const u32 i);
	inline T & operator()(const u32 i, const u32 j);

	inline s32 operator==(const Matrix<T, M, N> & m0);
	inline s32 operator!=(const Matrix<T, M, N> & m0);

	inline Matrix< T, M, N > & operator=(const Matrix< T, M, N > & mat);
	inline Matrix< T, M, N > & operator+=(const Matrix< T, M, N > & mat);
	inline Matrix< T, M, N > & operator-=(const Matrix< T, M, N > & mat);

	// Matrix< T, M, N >& operator*=(const Matrix< T, M, N > & mat) { return (*this); } // a implementer

	inline Matrix< T, M, N > & operator*=(const T & val);
	inline Matrix< T, M, N > & operator/=(const T & val);
	inline Matrix< T, M, N > & operator+=(const T & val);
	inline Matrix< T, M, N > & operator-=(const T & val);
	inline Matrix< T, M, N > & Zero(void);

	inline const Matrix< T, M, N > Transpose(void) const;

	inline Matrix< T, M, N > & LoadIdentity(void);

	// Determinant();
	// Inverse();

	const T * GetDataPtr(void);

private:
	T data[ M * N ];
};

// ----------------------------------------------------------------
template <typename T>
class EW_EXPORT_SYM Matrix<T, 4, 4>
{
public:

	Matrix<T, 4, 4>(void);
	Matrix<T, 4, 4>(const T & x);
	Matrix<T, 4, 4>(const T * m);
	Matrix(const Matrix< T, 3, 3 > & mat_33);

	const T & operator[](const u32 i) const;
	const T & operator()(const u32 i, const u32 j) const;
	T & operator[](const u32 i);
	T & operator()(const u32 i, const u32 j);

	Matrix< T, 4, 4 > & operator=(const Matrix< T, 4, 4 > & mat);
	Matrix< T, 4, 4 > & operator+=(const Matrix< T, 4, 4 > & mat);
	Matrix< T, 4, 4 > & operator-=(const Matrix< T, 4, 4 > & mat);
	Matrix< T, 4, 4 > operator*(const Matrix< T, 4, 4 > & mat);
	Matrix< T, 4, 4 > & operator*=(const Matrix< T, 4, 4 > & mat);
	Vector<T, 3> operator*(const Vector<T, 3> & vec);
	Matrix< T, 4, 4 > & operator*=(const T & val);
	Matrix< T, 4, 4 > & operator/=(const T & val);
	Matrix< T, 4, 4 > & operator+=(const T & val);
	Matrix< T, 4, 4 > & operator-=(const T & val);
	Matrix< T, 4, 4 > & Zero(void);

	const Matrix< T, 4, 4 > Transpose(void) const;
	Matrix<T, 4, 4> & LoadIdentity(void);
	Matrix<T, 4, 4> Inverse(void);


	void Rotation_X(const T angle);
	void Rotation_Y(const T angle);
	void Rotation_Z(const T angle);

	void Translate(const Vector<T, 3> & v);
	void Translate(const T x, const T y, const T z);

	void Scale(const Vector<T, 3> & v);
	void Scale(const T x, const T y, const T z);

	void Reflect(const Vector<T, 3> & v);
	void Reflect(const T x, const T y, const T z, const T w);

	void Look_At(const Vector<T, 3> & eye, const Vector<T, 3> & dir, const Vector<T, 3> & up);
	void Look_At(const T * eye, const T * dir, const T * up);

	const T * GetDataPtr(void);

private:
	T data[ 16 ];
};

// ----------------------------------- OSSTREAM OPERATOR ---------------------

template <typename T, u32 M, u32 N >
EW_EXPORT_SYM inline std::ostream & operator<<(std::ostream & stream, const ew::maths::Matrix<T, M, N> & mat);

} // ! Maths

} // ! Ew

#include <ew/maths/matrix.hxx>
