#pragma once

// le probleme c que avec s8  la plage pour la composante alpha n'est pas au max dans le constructeur
// faire une specialisation ??
// definir tous les operators pour Color3<> avec Color4<> en param et vice versa

#include <iostream>

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/maths/vector.hpp>


namespace ew
{
namespace graphics
{
namespace colors
{

using namespace ew::core::types;

template <typename T> class Color3;
template <typename T> class Color4;

typedef Color3<s8>  color3b;
typedef Color3<u8>  color3ub;
typedef Color3<s16> color3s;
typedef Color3<u16> color3us;
typedef Color3<s32> color3i;
typedef Color3<u32> color3ui;
typedef Color3<f32> color3f;
typedef Color3<double> color3d;

typedef Color4<s8>  color4b;
typedef Color4<u8>  color4ub;
typedef Color4<s16> color4s;
typedef Color4<u16> color4us;
typedef Color4<s32> color4i;
typedef Color4<u32> color4ui;
typedef Color4<f32> color4f;

typedef Color4<double> color4d;


typedef color3f color3;
typedef color4f color4;

// ---------------------------------

template <typename T>
class Color3
{
public:
	inline Color3<T>(const T * color);
	inline Color3<T>(const Color3<T> & color);
	inline Color3<T>(const Color4<T> & color);
	inline Color3<T>(const T & r_ = 0, const T & g_ = 0, const T & b_ = 0);
	inline const T & operator[](const u32 i) const;
	inline T & operator[](const u32 i);
	inline T * GetDataPtr(void);
	inline s32 operator==(const Color3<T> & color) const;
	inline s32 operator!=(const Color3<T> & color) const;
	inline Color3<T> operator+(const Color3<T> & color) const;
	inline Color3<T> operator-(const Color3<T> & color) const;
	inline Color3<T> operator*(const T & val) const;
	inline Color3<T> operator/(const T & val) const;
	inline Color3<T> operator+(const T & val) const;
	inline Color3<T> operator-(const T & val) const;
	inline Color3<T> & operator*=(const T & val);
	inline Color3<T> & operator/=(const T & val);
	inline Color3<T> & operator+=(const T & val);
	inline Color3<T> & operator-=(const T & val);
	inline Color3<T> & operator+=(const Color3<T> & color);
	inline Color3<T> & operator-=(const Color3<T> & color);
	inline Color3<T> & operator=(const Color3<T> & color);
	inline Color3<T> & operator=(const T * color);
	void set(const T & r_ = 0, const T & g_ = 0, const T & b_ = 0, const T & a_ = 1);
	void set(const T * color);
	T & red();
	T & green();
	T & blue();
public:
	T r;
	T g;
	T b;
};


// ----------------------------------------

template <typename T>
class Color4
{
public:
	inline Color4<T>(const T * color);
	inline Color4<T>(const Color3<T> & color);
	inline Color4<T>(const Color4<T> & color);
	inline Color4<T>(const T & r_ = 0, const T & g_ = 0, const T & b_ = 0, const T & a_ = 1);
	inline T & operator[](const u32 i);
	inline T * GetDataPtr(void);
	inline s32 operator==(const Color4<T> & color) const;
	inline s32 operator!=(const Color4<T> & color) const;
	inline Color4<T> operator+(const Color4<T> & color) const;
	inline Color4<T> operator-(const Color4<T> & color) const;
	inline Color4<T> operator*(const T & val) const;
	inline Color4<T> operator/(const T & val) const;
	inline Color4<T> operator+(const T & val) const;
	inline Color4<T> operator-(const T & val) const;
	inline Color4<T> & operator*=(const T & val);
	inline Color4<T> & operator/=(const T & val);
	inline Color4<T> & operator+=(const T & val);
	inline Color4<T> & operator-=(const T & val);
	inline Color4<T> & operator+=(const Color4<T> & color);
	inline Color4<T> & operator-=(const Color4<T> & color);
	inline Color4<T> & operator=(const Color4<T> & color);
	inline Color4<T> & operator=(const Color3<T> & color);
	inline Color4<T> & operator=(const T * color);
	void set(const T & r_ = 0, const T & g_ = 0, const T & b_ = 0, const T & a_ = 1);
	void set(const T * color);
	T & red();
	T & green();
	T & blue();
	T & alpha();
public:
	T r;
	T g;
	T b;
	T a;
};

// ----------------------------------------- STREAM -------------------------------------------

template <typename T>
inline std::ostream & operator<<(std::ostream & stream, const ew::graphics::colors::Color3<T> & color);

template <typename T>
inline std::ostream & operator<<(std::ostream & stream, const ew::graphics::colors::Color4<T> & color);

// ---------------------------------------------------------------------------------------------

}
}
}

#include <ew/graphics/color.hxx>
