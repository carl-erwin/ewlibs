#pragma once

#include <ew/ew_config.hpp>
#include <ew/graphics/color.hpp>
#include <ew/maths/vector.hpp>

namespace ew
{
    namespace graphics
    {
        namespace colors
        {

            template <typename T>
            inline Color3<T>::Color3(const T * color) : r(color[0]), g(color[1]), b(color[2])
            {

            }

            template <typename T>
            inline Color3<T>::Color3(const Color3<T> & color) : r(color.r), g(color.g), b(color.b)
            {

            }

            template <typename T>
            inline Color3<T>::Color3(const Color4<T> & color) : r(color.r), g(color.g), b(color.b)
            {

            }

            template <typename T>
            inline Color3<T>::Color3(const T & r_, const T & g_, const T & b_) : r(r_), g(g_), b(b_)
            {

            }
//
            template <typename T>
            inline const T & Color3<T>::operator[](const u32 i) const
            {
                return (&r)[i];
            }

            template <typename T>
            inline T & Color3<T>::operator[](const u32 i)
            {
                return (&r)[i];
            }

            template <typename T>
            inline T * Color3<T>::GetDataPtr(void)
            {
                return (T *)(&r);
            }
//
            template <typename T>
            inline s32 Color3<T>::operator==(const Color3<T> & color) const
            {
                return (r == color.r && g == color.g && b == color.b);
            }

            template <typename T>
            inline s32 Color3<T>::operator!=(const Color3<T> & color) const
            {
                return (r != color.r || g != color.g || b != color.b);
            }
//
            template <typename T>
            inline Color3<T> Color3<T>::operator+(const Color3<T> & color) const
            {
                return (Color3<T>(r + color.r, g + color.g, b + color.b));
            }

            template <typename T>
            inline Color3<T> Color3<T>::operator-(const Color3<T> & color) const
            {
                return (Color3<T>(r - color.r, g - color.g, b - color.b));
            }
//
            template <typename T>
            inline Color3<T> Color3<T>::operator*(const T & val) const
            {
                return (Color3<T>(r * val, g * val, b * val));
            }


            template <typename T>
            inline Color3<T> Color3<T>::operator/(const T & val) const
            {
                return (Color3<T>(r / val, g / val, b / val));
            }

            template <typename T>
            inline Color3<T> Color3<T>::operator+(const T & val) const
            {
                return (Color3<T>(r + val, g + val, b + val));
            }

            template <typename T>
            inline Color3<T> Color3<T>::operator-(const T & val) const
            {
                return (Color3<T>(r - val, g - val, b - val));
            }
//
            template <typename T>
            inline Color3<T> & Color3<T>::operator*=(const T & val)
            {
                r *= val;
                g *= val;
                b *= val;
                return (*this);
            }

            template <typename T>
            inline Color3<T> & Color3<T>::operator/=(const T & val)
            {
                r /= val;
                g /= val;
                b /= val;
                return (*this);
            }

            template <typename T>
            inline Color3<T> & Color3<T>::operator+=(const T & val)
            {
                r += val;
                g += val;
                b += val;
                return (*this);
            }

            template <typename T>
            inline Color3<T> & Color3<T>::operator-=(const T & val)
            {
                r -= val;
                g -= val;
                b -= val;
                return (*this);
            }

            template <typename T>
            inline Color3<T> & Color3<T>::operator+=(const Color3<T> & color)
            {
                r += color.r;
                g += color.g;
                b += color.b;
                return (*this);
            }

            template <typename T>
            inline Color3<T> & Color3<T>::operator-=(const Color3<T> & color)
            {
                r -= color.r;
                g -= color.g;
                b -= color.b;
                return (*this);
            }

            template <typename T>
            inline Color3<T> & Color3<T>::operator=(const Color3<T> & color)
            {
                r = color.r;
                g = color.g;
                b = color.b;
                return (*this);
            }

            template <typename T>
            inline Color3<T> & Color3<T>::operator=(const T * color)
            {
                r = color[0];
                g = color[1];
                b = color[2];
                return (*this);
            }
//
            template <typename T>
            void Color3<T>::set(const T & r_, const T & g_, const T & b_, const T & a_)
            {
                r = r_;
                g = g_;
                b = b_;
            }

            template <typename T>
            void Color3<T>::set(const T * color)
            {
                r = color[0];
                g = color[1];
                b = color[2];
            }

            template <typename T>
            T & Color3<T>::red()
            {
                return r;
            }

            template <typename T>
            T & Color3<T>::green()
            {
                return g;
            }

            template <typename T>
            T & Color3<T>::blue()
            {
                return b;
            }

            template <typename T>
            inline Color4<T>::Color4(const T * color)
                : r(color[0]), g(color[1]), b(color[2]), a(color[3])
            {

            }

            template <typename T>
            inline Color4<T>::Color4(const Color3<T> & color)
                : r(color.r), g(color.g), b(color.b), a(1)
            {

            }

            template <typename T>
            inline Color4<T>::Color4(const Color4<T> & color)
                : r(color.r), g(color.g), b(color.b), a(color.a)
            {

            }

            template <typename T>
            inline Color4<T>::Color4(const T & r_, const T & g_, const T & b_, const T & a_)
                : r(r_), g(g_), b(b_), a(a_)
            {

            }

//
            template <typename T>
            inline T & Color4<T>::operator[](const u32 i)
            {
                return ((&r)[i]);
            }

            template <typename T>
            inline T * Color4<T>::GetDataPtr(void)
            {
                return ((T *)(&r));
            }

            template <typename T>
            inline s32 Color4<T>::operator==(const Color4<T> & color) const
            {
                return (r == color.r && g == color.g && b == color.b && a == color.a);
            }

            template <typename T>
            inline s32 Color4<T>::operator!=(const Color4<T> & color) const
            {
                return (r != color.r || g != color.g || b != color.b || a != color.a);
            }

            template <typename T>
            inline Color4<T> Color4<T>::operator+(const Color4<T> & color) const
            {
                return (Color4<T>(r + color.r, g + color.g, b + color.b, a + color.a));
            }

            template <typename T>
            inline Color4<T> Color4<T>::operator-(const Color4<T> & color) const
            {
                return (Color4<T>(r - color.r, g - color.g, b - color.b, a - color.a));
            }

            template <typename T>
            inline Color4<T> Color4<T>::operator*(const T & val) const
            {
                return (Color4<T>(r * val, g * val, b * val, a * val));
            }

            template <typename T>
            inline Color4<T> Color4<T>::operator/(const T & val) const
            {
                return (Color4<T>(r / val, g / val, b / val, a / val));
            }

            template <typename T>
            inline Color4<T> Color4<T>::operator+(const T & val) const
            {
                return (Color4<T>(r + val, g + val, b + val, a + val));
            }

            template <typename T>
            inline Color4<T> Color4<T>::operator-(const T & val) const
            {
                return (Color4<T>(r - val, g - val, b - val, a - val));
            }

            template <typename T>
            inline Color4<T> & Color4<T>::operator*=(const T & val)
            {
                r *= val;
                g *= val;
                b *= val;
                a *= val;
                return (*this);
            }

            template <typename T>
            inline Color4<T> & Color4<T>::operator/=(const T & val)
            {
                r /= val;
                g /= val;
                b /= val;
                a /= val;
                return (*this);
            }

            template <typename T>
            inline Color4<T> & Color4<T>::operator+=(const T & val)
            {
                r += val;
                g += val;
                b += val;
                a += val;
                return (*this);
            }

            template <typename T>
            inline Color4<T> & Color4<T>::operator-=(const T & val)
            {
                r -= val;
                g -= val;
                b -= val;
                a -= val;
                return (*this);
            }

            template <typename T>
            inline Color4<T> & Color4<T>::operator+=(const Color4<T> & color)
            {
                r += color.r;
                g += color.g;
                b += color.b;
                a += color.a;
                return (*this);
            }

            template <typename T>
            inline Color4<T> & Color4<T>::operator-=(const Color4<T> & color)
            {
                r -= color.r;
                g -= color.g;
                b -= color.b;
                a -= color.a;
                return (*this);
            }

            template <typename T>
            inline Color4<T> & Color4<T>::operator=(const Color4<T> & color)
            {
                r = color.r;
                g = color.g;
                b = color.b;
                a = color.a;
                return (*this);
            }

            template <typename T>
            inline Color4<T> & Color4<T>::operator=(const Color3<T> & color)
            {
                r = color.r;
                g = color.g;
                b = color.b;
                return (*this);
            }

            template <typename T>
            inline Color4<T> & Color4<T>::operator=(const T * color)
            {
                r = color[0];
                g = color[1];
                b = color[2];
                a = color[3];
                return (*this);
            }

            template <typename T>
            T & Color4<T>::red()
            {
                return r;
            }

            template <typename T>
            T & Color4<T>::green()
            {
                return g;
            }

            template <typename T>
            T & Color4<T>::blue()
            {
                return b;
            }

            template <typename T>
            T & Color4<T>::alpha()
            {
                return a;
            }

//
            template <typename T>
            void Color4<T>::set(const T & r_, const T & g_, const T & b_, const T & a_)
            {
                r = r_;
                g = g_;
                b = b_;
                a = a_;
            }

            template <typename T>
            void Color4<T>::set(const T * color)
            {
                r = color[0];
                g = color[1];
                b = color[2];
                a = color[3];
            }
//
// ----------------------------------------- STREAM -------------------------------------------

            template <typename T>
            inline std::ostream & operator<<(std::ostream & stream, const ew::graphics::colors::Color3<T> & color)
            {
                stream << "( " << color.r << " " << color.g << " " << color.b << ")";
                return (stream);
            }

            template <typename T>
            inline std::ostream & operator<<(std::ostream & stream, const ew::graphics::colors::Color4<T> & color)
            {
                stream << "( " << color.r << " " << color.g << " " << color.b << " " << color.a << " )";
                return (stream);
            }

        }
    }
}

