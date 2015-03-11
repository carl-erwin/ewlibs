#pragma once

#include <ew/maths/vector.hpp>

namespace ew
{
    namespace maths
    {

// ----------------------------------- STREAM OP -------------------------------------------------

        template <typename T, u32 size>   inline std::ostream & operator<<(std::ostream & stream, const ew::maths::Vector<T, size> & vec)
        {
            stream << "(";
            for (u32 i = 0; i < size; ++i) {
                stream << vec[i];
                if (i < size - 1)
                    stream << ", ";
            }
            stream << ")";
            return (stream);
        }

// ---------------------------------------------------------------------------------------------
        /*
          Vector<T,size>
        */
// ---------------------------------------------------------------------------------------------


// Dot product
        template <typename T, u32 size>  inline T Dot(const Vector<T, size> & v1, const Vector<T, size> & v2)
        {
            T result(0);

            for (u32 i = 0; i < size; ++i)
                result += v1.m_vec[i] * v2.m_vec[i];

            return (result);
        }

// Cross product
#ifndef EW_VECTOR_CROSS_RETURN_REFERENCE
        template <typename T>  inline void Cross(Vector<T, 3> & res, const Vector<T, 3> & v1, const Vector<T, 3> & v2)
        {
            res.x = v1.y * v2.z - v1.z * v2.y;
            res.y = v1.z * v2.x - v1.x * v2.z;
            res.z = v1.x * v2.y - v1.y * v2.x;
        }
#else
        template <typename T>  inline Vector<T, 3> & Cross(Vector<T, 3> & res, const Vector<T, 3> & v1, const Vector<T, 3> & v2)
        {
            res.x = v1.y * v2.z - v1.z * v2.y;
            res.y = v1.z * v2.x - v1.x * v2.z;
            res.z = v1.x * v2.y - v1.y * v2.x;

            return (res);
        }
#endif

// Slow
        template<typename T>  inline Vector<T, 3> Cross(const Vector<T, 3> & v1, const Vector<T, 3> & v2)
        {
            Vector<T, 3> res;

            res.x = v1.y * v2.z - v1.z * v2.y;
            res.y = v1.z * v2.x - v1.x * v2.z;
            res.z = v1.x * v2.y - v1.y * v2.x;

            return (res);
        }

// -------------------------------------------

        template <typename T>  inline T Dot(const Vector<T, 2> & v1, const Vector<T, 2> & v2)
        {
            return (v1.x * v2.x + v1.y * v2.y);
        }

        template <typename T>  inline T Dot(const Vector<T, 3> & v1, const Vector<T, 3> & v2)
        {
            return (v1.x * v2.x +  v1.y * v2.y + v1.z * v2.z);
        }

        template <typename T>  inline T Dot(const Vector<T, 4> & v1, const Vector<T, 4> & v2)
        {
            return (v1.x * v2.x +  v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
        }

// -------------------------------------------

        template <typename T, u32 size> inline Vector<T, size>::Vector()
        {

        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>  inline Vector<T, size>::Vector(const T * vec)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] = vec[i];
        }
// --------------------------------------------------------------------------------------------

        template <typename T, u32 size>  inline Vector<T, size>::Vector(const Vector<T, size> & vec)
        {
            if (this != &vec) {
                for (u32 i = 0; i < size; ++i)
                    m_vec[i] = vec[i];
            }
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>  inline const T & Vector<T, size>::operator[](const u32 i) const
        {
            return m_vec[i];
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline T & Vector<T, size>::operator[](const u32 i)
        {
            return m_vec[i];
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline T * Vector<T, size>::GetDataPtr(void)
        {
            return (T *)m_vec;
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline s32 Vector<T, size>::operator==(const Vector<T, size> & vec)
        {
            for (u32 i = 0; i < size; ++i)
                if (m_vec[i] != vec[i])
                    return (0);

            return (1);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline s32 Vector<T, size>::operator!=(const Vector<T, size> & vec)
        {
            for (u32 i = 0; i < size; ++i)
                if (m_vec[i] == vec[i])
                    return (0);

            return (1);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> Vector<T, size>::operator+(const Vector<T, size> & vec)
        {
            Vector<T, size> res;

            for (u32 i = 0; i < size; ++i)
                res[i] = m_vec[i] + vec[i];

            return (res);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> Vector<T, size>::operator-(const Vector<T, size> & vec)
        {
            Vector<T, size> res;

            for (u32 i = 0; i < size; ++i)
                res[i] = m_vec[i] - vec[i];

            return (res);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> Vector<T, size>::operator*(const T & val)
        {
            Vector<T, size> res;

            for (u32 i = 0; i < size; ++i)
                res[i] = m_vec[i] * val;

            return (res);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> Vector<T, size>::operator/(const T & val)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] *= val;

            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> Vector<T, size>::operator+(const T & val)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] *= val;

            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> Vector<T, size>::operator-(const T & val)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] *= val;

            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> & Vector<T, size>::operator*=(const T & val)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] *= val;

            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> & Vector<T, size>::operator/=(const T & val)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] /= val;

            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> & Vector<T, size>::operator+=(const T & val)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] += val;

            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> & Vector<T, size>::operator-=(const T & val)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] -= val;

            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> & Vector<T, size>::operator+=(const Vector<T, size> & vec)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] += vec[i];

            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> & Vector<T, size>::operator-=(const Vector<T, size> & vec)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] -= vec[i];

            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> & Vector<T, size>::operator=(const Vector<T, size> & vec)
        {
            if (this != &vec) {
                for (u32 i = 0; i < size; ++i)
                    m_vec[i] = vec[i];
            }
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> & Vector<T, size>::operator=(const T * vec)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] = vec[i];

            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> operator+(const Vector<T, size> & v0, const Vector<T, size> & v1)
        {
            Vector<T, size> res;

            for (u32 i = 0; i < size; ++i)
                res[i] = v0[i] + v1[i];

            std::cerr << "OK" << "\n";

            return (res);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> operator-(const Vector<T, size> & v0, const Vector<T, size> & v1)
        {
            Vector<T, size> res;

            for (u32 i = 0; i < size; ++i)
                res[i] = v0[i] - v1[i];

            std::cerr << " GEN VECTOR" << std::endl;

            return (res);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> operator/(const Vector<T, size> & v0, const T & val)
        {
            Vector<T, size> res;

            for (u32 i = 0; i < size; ++i)
                res[i] = v0[i] / val;

            return (res);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline T  Vector<T, size>::Length() const
        {
            T length(0);

            for (u32 i = 0; i < size; ++i)
                length += m_vec[i] * m_vec[i];

            return ((T)sqrt((double)length));
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline T  Vector<T, size>::Dot(const Vector<T, size> & vec)
        {
            T dot(0);

            for (u32 i = 0; i < size; ++i)
                dot += m_vec[i] * vec[i];

            return (dot);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size> & Vector<T, size>::Normalize()
        {
            T length =  Vector<T, size>::Length();

            for (u32 i = 0; i < size; ++i)
                m_vec[i] /= length;

            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T, u32 size>
        inline Vector<T, size>  & Vector<T, size>::zero(void)
        {
            for (u32 i = 0; i < size; ++i)
                m_vec[i] = 0;

            return (*this);
        }


// -------------------------------------------------------------------------------------------------
        /*
          Vector<T, 2>
        */
// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2>::Vector(void)
        {

        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2>::Vector(const T * vec)
// x(vec[0])
        {
            x = vec[0];
            y = vec[1];
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2>::Vector(const Vector<T, 2> & vec) : x(vec.x), y(vec.y)
        {

        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2>::Vector(const T x_, const T y_) : x(x_), y(y_)
        {

        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline const T & Vector<T, 2>::operator[](const u32 i) const
        {
            return m_vec[i];
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline T & Vector<T, 2>::operator[](const u32 i)
        {
            return m_vec[i];
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline T * Vector<T, 2>::GetDataPtr(void)
        {
            return (T *)m_vec;
        }

// -------------------------------------------------------------------------------------------------
        template <typename T>      inline const T * Vector<T, 2>::GetDataPtr(void) const
        {
            return (T *)m_vec;
        }

// -------------------------------------------------------------------------------------------------
//  inline operator T*();
//  inline operator const T*() const;
// -------------------------------------------------------------------------------------------------

        template <typename T>      inline s32 Vector<T, 2>::operator==(const Vector<T, 2> & vec)
        {
            return (x == vec.x && y == vec.y);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline s32 Vector<T, 2>::operator!=(const Vector<T, 2> & vec)
        {
            return (x != vec.x || y != vec.y);
        }

// -------------------------------------------------------------------------------------------------


        template <typename T>      inline Vector<T, 2> Vector<T, 2>::operator-() const
        {
            return (Vector<T, 2>(-x, -y));
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> Vector<T, 2>::operator+(const Vector<T, 2> & vec) const
        {
            return (Vector<T, 2>(x + vec.x, y + vec.y));
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> Vector<T, 2>::operator-(const Vector<T, 2> & vec) const
        {
            return (Vector<T, 2>(x - vec.x, y - vec.y));
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> Vector<T, 2>::operator*(const T & val) const
        {
            return (Vector<T, 2>(x * val, y * val));
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> Vector<T, 2>::operator/(const T & val) const
        {
            return (Vector<T, 2>(x / val, y / val));
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> Vector<T, 2>::operator+(const T & val) const
        {
            return (Vector<T, 2>(x + val, y + val));
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> Vector<T, 2>::operator-(const T & val) const
        {
            return (Vector<T, 2>(x - val, y - val));
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::operator*=(const T & val)
        {
            x *= val;
            y *= val;
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::operator/=(const T & val)
        {
            x /= val;
            y /= val;
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::operator+=(const T & val)
        {
            x += val;
            y += val;
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::operator-=(const T & val)
        {
            x -= val;
            y -= val;
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::operator+=(const Vector<T, 2> & vec)
        {
            x += vec.x;
            y += vec.y;
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::operator-=(const Vector<T, 2> & vec)
        {
            x -= vec.y;
            y -= vec.y;
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::operator=(const Vector<T, 2> & vec)
        {
            x = vec.x;
            y = vec.y;
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::operator=(const T * vec)
        {
            x = vec[0];
            y = vec[1];
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline T Vector<T, 2>::Length() const
        {
            return ((T)sqrt(x * x + y * y));
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline T Vector<T, 2>::Dot(const Vector<T, 2> & vec) const
        {
            return (x * vec.x + y * vec.y);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::Normalize()
        {
            T len = Length();

            if (len) {
                x /= len;
                y /= len;
            }
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::zero(void)
        {
            x = 0;
            y = 0;
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::set(const T & x_, const T & y_)
        {
            x = x_;
            y = y_;
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> & Vector<T, 2>::set(const T * vec)
        {
            x = vec[0];
            y = vec[1];
            return (*this);
        }

// -------------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 2> operator-(Vector<T, 2> & v)
        {
            return (Vector<T, 2>(-v.x, -v.y));
        }

// -------------------------------------------------------------------------------------------------
        /*
          Vector<T,3>
        */
// --------------------------------------------------------------------------------------------------
        template <typename T>      inline Vector<T, 3>::Vector(void)
        {
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3>::Vector(const T * vec) : x(vec[0]), y(vec[1]), z(vec[2])
        {
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3>::Vector(const Vector<T, 3> & vec) : x(vec.x), y(vec.y), z(vec.z)
        {
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3>::Vector(const T x_, const T y_ , const T z_) : x(x_), y(y_), z(z_)
        {
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline const T & Vector<T, 3>::operator[](const u32 i) const
        {
            return m_vec[i];
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline T & Vector<T, 3>::operator[](const u32 i)
        {
            return m_vec[i];
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline T * Vector<T, 3>::GetDataPtr(void)
        {
            return (T *)m_vec;
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline const T * Vector<T, 3>::GetDataPtr(void) const
        {
            return (T *)m_vec;
        }

// ---------------------------------------------------------------------------------------------
//  inline operator T*();
//  inline operator const T*() const;
// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> Vector<T, 3>::operator-() const
        {
            return (Vector<T, 3>(-x, -y, -z));
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline s32 Vector<T, 3>::operator==(const Vector<T, 3> & vec) const
        {
            return (x == vec.x && y == vec.y && z == vec.z);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline s32 Vector<T, 3>::operator!=(const Vector<T, 3> & vec) const
        {
            return (x != vec.x || y != vec.y || z != vec.z);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> Vector<T, 3>::operator+(const Vector<T, 3> & vec) const
        {
            return (Vector<T, 3>(x + vec.x, y + vec.y, z + vec.z));
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> Vector<T, 3>::operator-(const Vector<T, 3> & vec) const
        {
            return (Vector<T, 3>(x - vec.x, y - vec.y, z - vec.z));
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> Vector<T, 3>::operator*(const T & val) const
        {
            return (Vector<T, 3>(x * val, y * val, z * val));
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> Vector<T, 3>::operator/(const T & val) const
        {
            return (Vector<T, 3>(x / val, y / val, z / val));
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> Vector<T, 3>::operator+(const T & val) const
        {
            return (Vector<T, 3>(x + val, y + val, z + val));
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> Vector<T, 3>::operator-(const T & val) const
        {
            return (Vector<T, 3>(x - val, y - val, z - val));
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::operator*=(const T & val)
        {
            x *= val;
            y *= val;
            z *= val;
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::operator/=(const T & val)
        {
            x /= val;
            y /= val;
            z /= val;
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::operator+=(const T & val)
        {
            x += val;
            y += val;
            z += val;
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::operator-=(const T & val)
        {
            x -= val;
            y -= val;
            z -= val;
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::operator+=(const Vector<T, 3> & vec)
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::operator-=(const Vector<T, 3> & vec)
        {
            x -= vec.y;
            y -= vec.y;
            z -= vec.z;
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::operator=(const Vector<T, 3> & vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::operator=(const T * vec)
        {
            x = vec[0];
            y = vec[1];
            z = vec[2];
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline T Vector<T, 3>::Length() const
        {
            return ((T)sqrt(x * x + y * y + z * z));
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline T Vector<T, 3>::Dot(const Vector<T, 3> & vec) const
        {
            return (x * vec.x + y * vec.y + z * vec.z);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::Normalize()
        {
            T len = Length();

            if (len) {
                x /= len;
                y /= len;
                z /= len;
            }
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

// misc
        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::zero(void)
        {
            x = 0;
            y = 0;
            z = 0;
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::set(const T x_, const T y_, const T z_, const T w_)
        {
            x = x_;
            y = y_;
            z = z_;
            return (*this);
        }

// ---------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 3> & Vector<T, 3>::set(const T * vec)
        {
            x = vec[0];
            y = vec[1];
            z = vec[2];
            return (*this);
        }

// ----------------------------------------------------------------------------------------------


        template <typename T>      inline Vector<T, 3> operator-(Vector<T, 3> & v)
        {
            return (Vector<T, 3>(-v.x, -v.y, -v.z));
        }

// ----------------------------------------------------------------------------------------------
        /*
          Vector<T,4>
        */
// ----------------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4>::Vector(void)
            :
            x(0),
            y(0),
            z(0),
            w(0)
        {
            std::cerr << "inline Vector<T, 4>::Vector(void)" << "\n";
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4>::Vector(const T * vec)
            :
            x(vec[0]),
            y(vec[1]),
            z(vec[2]),
            w(vec[3])
        {
        }

//-----------------------------------------------------------------------------------------

        template <typename T> inline Vector<T, 4>::Vector(const Vector<T, 4> & vec)
            :
            x(vec.x),
            y(vec.y),
            z(vec.z),
            w(vec.w)
        {
        }

//-----------------------------------------------------------------------------------------

        template <typename T> inline Vector<T, 4>::Vector(const T x_, const T y_, const T z_, const T w_)
            :
            x(x_),
            y(y_),
            z(z_),
            w(w_)
        {
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline const T & Vector<T, 4>::operator[](const u32 i) const
        {
            return (&x)[i];
        }
//-----------------------------------------------------------------------------------------

        template <typename T>      inline T & Vector<T, 4>::operator[](const u32 i)
        {
            return (&x)[i];
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline T * Vector<T, 4>::GetDataPtr(void)
        {
            return (T *)(&x);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline const T * Vector<T, 4>::GetDataPtr(void) const
        {
            return (const T *)(&x);
        }

//-----------------------------------------------------------------------------------------
//  inline operator T*();
//  inline operator const T*() const;
//-----------------------------------------------------------------------------------------

        template <typename T>      inline s32 Vector<T, 4>::operator==(const Vector<T, 4> & vec)
        {
            return (x == vec.x && y == vec.y && z == vec.z && w == vec.w);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline s32 Vector<T, 4>::operator!=(const Vector<T, 4> & vec)
        {
            return (x != vec.x || y != vec.y || z != vec.z || w == vec.w);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> Vector<T, 4>::operator-() const
        {
            return (Vector<T, 4>(-x, -y, -z, -w));
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> Vector<T, 4>::operator+(const Vector<T, 4> & vec) const
        {
            return (Vector<T, 4>(x + vec.x, y + vec.y, z + vec.z, w + vec.w));
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> Vector<T, 4>::operator-(const Vector<T, 4> & vec) const
        {
            return (Vector<T, 4>(x - vec.x, y - vec.y, z - vec.z,  w - vec.w));
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> Vector<T, 4>::operator*(const T & val) const
        {
            return (Vector<T, 4>(x * val, y * val, z * val, w * val));
        }
//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> Vector<T, 4>::operator/(const T & val) const
        {
            return (Vector<T, 4>(x / val, y / val, z / val, w / val));
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> & Vector<T, 4>::operator*=(T val)
        {
            x *= val;
            y *= val;
            z *= val;
            w *= val;
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> & Vector<T, 4>::operator/=(T val)
        {
            x /= val;
            y /= val;
            z /= val;
            z /= val;
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> & Vector<T, 4>::operator+=(T val)
        {
            x += val;
            y += val;
            z += val;
            w += val;
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> & Vector<T, 4>::operator-=(T val)
        {
            x -= val;
            y -= val;
            z -= val;
            w -= val;
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> & Vector<T, 4>::operator+=(Vector<T, 4> & vec)
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            w += vec.w;
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> & Vector<T, 4>::operator-=(Vector<T, 4> & vec)
        {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            w -= vec.w;
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> & Vector<T, 4>::operator=(const Vector<T, 4> & vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            w = vec.w;
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> & Vector<T, 4>::operator=(const T * vec)
        {
            x = vec[0];
            y = vec[1];
            z = vec[2];
            w = vec[3];
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline T Vector<T, 4>::Length() const
        {
            return ((T)sqrt(x * x + y * y + z * z + w * w));
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline T Vector<T, 4>::Dot(const Vector<T, 4> & vec) const
        {
            return (x * vec.x + y * vec.y + z * vec.z + w * vec.w);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>      inline Vector<T, 4> & Vector<T, 4>::Normalize()
        {
            T len = Length();

            if (len) {
                x /= len;
                y /= len;
                z /= len;
                w /= len;
            }
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>     inline Vector<T, 4> & Vector<T, 4>::zero(void)
        {
            x = 0;
            y = 0;
            z = 0;
            w = 0;
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>     inline Vector<T, 4> & Vector<T, 4>::set(T x_, T y_, T z_, T w_)
        {
            x = x_;
            y = y_;
            z = z_;
            w = w_;
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>    inline Vector<T, 4> & Vector<T, 4>::set(T * vec)
        {
            x = vec[0];
            y = vec[1];
            z = vec[2];
            w = vec[3];
            return (*this);
        }

//-----------------------------------------------------------------------------------------

        template <typename T>    inline  Vector<T, 4> operator-(Vector<T, 4> & v)
        {
            return (Vector<T, 4>(-v.x, -v.y, -v.z, -v.w));
        }

//-----------------------------------------------------------------------------------------

    }
} // ! ew::maths

