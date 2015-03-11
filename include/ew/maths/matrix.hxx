#pragma once

#include <ew/maths/matrix.hpp>

namespace ew
{
    namespace maths
    {

// -------------------------------------------------------------------------------------------

// generic matrix  class

        template <typename T, u32 M, u32 N > inline Matrix<T, M, N>::Matrix(void) { }

        template <typename T, u32 M, u32 N > inline Matrix<T, M, N>::Matrix(const T & x)
        {
            memset(data, x, sizeof(T) * M * N);
        }

        template <typename T, u32 M, u32 N > inline Matrix<T, M, N>::Matrix(const T * m)
        {
            memcpy(data, m, sizeof(T) * M * N);
        }

        template <typename T, u32 M, u32 N > inline const T & Matrix<T, M, N>::operator[](const u32 i) const
        {
            return data[i];
        }

        template <typename T, u32 M, u32 N >
        inline const T & Matrix<T, M, N>::operator()(const u32 i, const u32 j) const
        {
            return (data[j * N + i]);
        }

        template <typename T, u32 M, u32 N >
        inline T & Matrix<T, M, N>::operator[](const u32 i)
        {
            return (data[i]);
        }

        template <typename T, u32 M, u32 N >
        inline T & Matrix<T, M, N>::operator()(const u32 i, const u32 j)
        {
            return (data[j * N + i]);
        }

        template <typename T, u32 M, u32 N >
        inline s32 Matrix<T, M, N>::operator==(const Matrix<T, M, N> & m0) // , const Matrix<T, M, N>& m1)
        {
            for (u32 i = 0; i < M * N; ++i)
                if (m0[i] != data[i])
                    return (0);
            return (1);
        }

        template <typename T, u32 M, u32 N >
        inline s32 Matrix<T, M, N>::operator!=(const Matrix<T, M, N> & m0) // , const Matrix<T, M, N> & m1)
        {
            for (u32 i = 0; i < M * N; ++i)
                if (m0[i] == data[i])
                    return (0);
            return (1);
        }

        template <typename T, u32 M, u32 N >
        inline Matrix< T, M, N > & Matrix<T, M, N>::operator=(const Matrix< T, M, N > & mat)
        {
            memcpy(this->d, mat.data, sizeof(T) * M * N);
            return (*this);
        }

        template <typename T, u32 M, u32 N >
        inline Matrix< T, M, N > & Matrix<T, M, N>::operator+=(const Matrix< T, M, N > & mat)
        {
            for (u32 i = 0; i < M * N; ++i)
                this->d[i] += mat.data[i];
            return (*this);
        }

        template <typename T, u32 M, u32 N >
        inline Matrix< T, M, N > & Matrix<T, M, N>::operator-=(const Matrix< T, M, N > & mat)
        {
            for (u32 i = 0; i < M * N; ++i)
                this->d[i] -= mat.data[i];
            return (*this);
        }

// Matrix< T, M, N >& operator*=(const Matrix< T, M, N > & mat) { return (*this); } // a implementer
        template <typename T, u32 M, u32 N >
        inline Matrix< T, M, N > & Matrix<T, M, N>::operator*=(const T & val)
        {
            for (u32 i = 0; i < M * N; ++i)
                this->d[i] *= val;
            return (*this);
        }

        template <typename T, u32 M, u32 N >
        inline Matrix< T, M, N > & Matrix<T, M, N>::operator/=(const T & val)
        {
            for (u32 i = 0; i < M * N; ++i)
                this->d[i] /= val;
            return (*this);
        }

        template <typename T, u32 M, u32 N >
        inline Matrix< T, M, N > & Matrix<T, M, N>::operator+=(const T & val)
        {
            for (u32 i = 0; i < M * N; ++i)
                this->d[i] += val;
            return (*this);
        }

        template <typename T, u32 M, u32 N >
        inline Matrix< T, M, N > & Matrix<T, M, N>::operator-=(const T & val)
        {
            for (u32 i = 0; i < M * N; ++i)
                this->d[i] -= val;
            return (*this);
        }

        template <typename T, u32 M, u32 N >
        inline Matrix< T, M, N > & Matrix<T, M, N>::Zero(void)
        {
            memset(data, 0, sizeof(T) * M * N);
            return (*this);
        }

        template <typename T, u32 M, u32 N >
        inline const Matrix< T, M, N > Matrix<T, M, N>::Transpose(void) const
        {
            Matrix<T, N, M> result;

            for (u32 j = 0; j < M; ++j)
                for (u32 i = 0; i < N; ++i)
                    result(i, j) = (*this)(j, i);
            return (result);
        }

        template <typename T, u32 M, u32 N >
        inline Matrix< T, M, N > & Matrix<T, M, N>::LoadIdentity(void)
        {
            Zero();

            for (u32 i = 0; i < M; ++i)
                data[i * M + i] = T(1);
            return (*this);
        }

        template <typename T, u32 M, u32 N >
        const T * Matrix<T, M, N>::GetDataPtr(void)
        {
            return (data);
        }

// ----------------------------------------------------------------


        template <typename T>
        inline Matrix<T, 4, 4>::Matrix(void) { }


        template <typename T>
        inline  Matrix<T, 4, 4>::Matrix(const T & x)
        {
            data[0] = x;
            data[1] = x;
            data[2] = x;
            data[3] = x;
            data[4] = x;
            data[5] = x;
            data[6] = x;
            data[7] = x;
            data[8] = x;
            data[9] = x;
            data[10] = x;
            data[11] = x;
            data[12] = x;
            data[13] = x;
            data[14] = x;
            data[15] = x;
        }

        template <typename T>
        Matrix<T, 4, 4>::Matrix(const T * m)
        {
            // memcpy(data, m, sizeof(T) * 16);
            data[0] = m[0];
            data[4] = m[4];
            data[8]  = m[8];
            data[12] = m[12];
            data[1] = m[1];
            data[5] = m[5];
            data[9]  = m[9];
            data[13] = m[13];
            data[2] = m[2];
            data[6] = m[6];
            data[10] = m[10];
            data[14] = m[14];
            data[3] = m[3];
            data[7] = m[7];
            data[11] = m[11];
            data[15] = m[15];
        }

        template <typename T>
        Matrix<T, 4, 4>::Matrix(const Matrix< T, 3, 3 > & mat_33)
        {
            /*
            LoadIdentity();
            for (u32 j = 0; j < 3; ++j)
            for (u32 i = 0; i < 3; ++i)
            (*this)(i, j) = mat_33(i, j);
            */

            data[0] = mat_33[0];
            data[4] = mat_33[3];
            data[8]  = mat_33[6];
            data[12] = T(0);
            data[1] = mat_33[1];
            data[5] = mat_33[4];
            data[9]  = mat_33[7];
            data[13] = T(0);
            data[2] = mat_33[2];
            data[6] = mat_33[5];
            data[10] = mat_33[8];
            data[14] = T(0);
            data[3] = T(0);
            data[7] = T(0);
            data[11] = T(0);
            data[15] = T(1);

        }

        template <typename T>
        const T & Matrix<T, 4, 4>::operator[](const u32 i) const
        {
            return (data[i]);
        }


        template <typename T>
        const T & Matrix<T, 4, 4>::operator()(const u32 i, const u32 j) const
        {
            return (data[j * 4 + i]);
        }

        template <typename T>
        T & Matrix<T, 4, 4>::operator[](const u32 i)
        {
            return (data[i]);
        }

        template <typename T>
        T & Matrix<T, 4, 4>::operator()(const u32 i, const u32 j)
        {
            // #ifdef __USE_EW_MATHS_MATRIX_COLUMN_MODE__
            return data[(j * 4) + i];
            // #endif

            // #ifdef __USE_EW_MATHS_MATRIX_ROW_MODE__
            // return data[(i * 4) + j];
            // #endif
        }

        template <typename T>
        inline Matrix< T, 4, 4 > & Matrix<T, 4, 4>::operator=(const Matrix< T, 4, 4 > & mat)
        {
            // memcpy(this->d, mat.data, sizeof(T) * 16);
            if (&mat != this) {
                data[0] = mat[0];
                data[1] = mat[1];
                data[2] = mat[2];
                data[3] = mat[3];
                data[4] = mat[4];
                data[5] = mat[5];
                data[6] = mat[6];
                data[7] = mat[7];
                data[8] = mat[8];
                data[9] = mat[9];
                data[10] = mat[10];
                data[11] = mat[11];
                data[12] = mat[12];
                data[13] = mat[13];
                data[14] = mat[14];
                data[15] = mat[15];
            }
            return (*this);
        }

        template <typename T>
        Matrix< T, 4, 4 > & Matrix<T, 4, 4>::operator+=(const Matrix< T, 4, 4 > & mat)
        {
            // for (u32 i = 0; i < 16; ++i) this->d[i] += mat.data[i]; return (*this);

            data[0]  += mat[0];
            data[1]  += mat[1];
            data[2]  += mat[2];
            data[3]  += mat[3];
            data[4]  += mat[4];
            data[5]  += mat[5];
            data[6]  += mat[6];
            data[7]  += mat[7];
            data[8]  += mat[8];
            data[9]  += mat[9];
            data[10] += mat[10];
            data[11] += mat[11];
            data[12] += mat[12];
            data[13] += mat[13];
            data[14] += mat[14];
            data[15] += mat[15];

            return (*this);
        }

        template <typename T>
        Matrix< T, 4, 4 > & Matrix<T, 4, 4>::operator-=(const Matrix< T, 4, 4 > & mat)
        {
            // for (u32 i = 0; i < 16; ++i) this->d[i] -= mat.data[i]; return (*this);

            data[0]  -= mat[0];
            data[1]  -= mat[1];
            data[2]  -= mat[2];
            data[3]  -= mat[3];
            data[4]  -= mat[4];
            data[5]  -= mat[5];
            data[6]  -= mat[6];
            data[7]  -= mat[7];
            data[8]  -= mat[8];
            data[9]  -= mat[9];
            data[10] -= mat[10];
            data[11] -= mat[11];
            data[12] -= mat[12];
            data[13] -= mat[13];
            data[14] -= mat[14];
            data[15] -= mat[15];

            return (*this);
        }

        template <typename T>
        Matrix< T, 4, 4 > Matrix<T, 4, 4>::operator*(const Matrix< T, 4, 4 > & mat)
        {
            Matrix<T, 4, 4> ret;

            ret[0]  = data[0] * mat[0]  + data[4] * mat[1]  + data[8]  * mat[2]  + data[12] * mat[3];
            ret[1]  = data[1] * mat[0]  + data[5] * mat[1]  + data[9]  * mat[2]  + data[13] * mat[3];
            ret[2]  = data[2] * mat[0]  + data[6] * mat[1]  + data[10] * mat[2]  + data[14] * mat[3];
            ret[3]  = data[3] * mat[0]  + data[7] * mat[1]  + data[11] * mat[2]  + data[15] * mat[3];
            ret[4]  = data[0] * mat[4]  + data[4] * mat[5]  + data[8]  * mat[6]  + data[12] * mat[7];
            ret[5]  = data[1] * mat[4]  + data[5] * mat[5]  + data[9]  * mat[6]  + data[13] * mat[7];
            ret[6]  = data[2] * mat[4]  + data[6] * mat[5]  + data[10] * mat[6]  + data[14] * mat[7];
            ret[7]  = data[3] * mat[4]  + data[7] * mat[5]  + data[11] * mat[6]  + data[15] * mat[7];
            ret[8]  = data[0] * mat[8]  + data[4] * mat[9]  + data[8]  * mat[10] + data[12] * mat[11];
            ret[9]  = data[1] * mat[8]  + data[5] * mat[9]  + data[9]  * mat[10] + data[13] * mat[11];
            ret[10] = data[2] * mat[8]  + data[6] * mat[9]  + data[10] * mat[10] + data[14] * mat[11];
            ret[11] = data[3] * mat[8]  + data[7] * mat[9]  + data[11] * mat[10] + data[15] * mat[11];
            ret[12] = data[0] * mat[12] + data[4] * mat[13] + data[8]  * mat[14] + data[12] * mat[15];
            ret[13] = data[1] * mat[12] + data[5] * mat[13] + data[9]  * mat[14] + data[13] * mat[15];
            ret[14] = data[2] * mat[12] + data[6] * mat[13] + data[10] * mat[14] + data[14] * mat[15];
            ret[15] = data[3] * mat[12] + data[7] * mat[13] + data[11] * mat[14] + data[15] * mat[15];

            return (ret);
        }

        template <typename T>
        Matrix< T, 4, 4 > & Matrix<T, 4, 4>::operator*=(const Matrix< T, 4, 4 > & mat)
        {
            *this = (*this) * mat;
            return (*this);
        }

        template <typename T>
        Vector<T, 3> Matrix<T, 4, 4>::operator*(const Vector<T, 3> & vec)
        {
            Vector<T, 3> ret;

            ret[0] = data[0] * vec[0] + data[4] * vec[1] + data[8]  * vec[2] + data[12];
            ret[1] = data[1] * vec[0] + data[5] * vec[1] + data[9]  * vec[2] + data[13];
            ret[2] = data[2] * vec[0] + data[6] * vec[1] + data[10] * vec[2] + data[14];

            return (ret);
        }

        template <typename T>
        Matrix< T, 4, 4 > & Matrix<T, 4, 4>::operator*=(const T & val)
        {
            for (u32 i = 0; i < 16; ++i) this->d[i] *= val;
            return (*this);
        }

        template <typename T>
        Matrix< T, 4, 4 > & Matrix<T, 4, 4>::operator/=(const T & val)
        {
            for (u32 i = 0; i < 16; ++i) this->d[i] /= val;
            return (*this);
        }

        template <typename T>
        Matrix< T, 4, 4 > & Matrix<T, 4, 4>::operator+=(const T & val)
        {
            for (u32 i = 0; i < 16; ++i) this->d[i] += val;
            return (*this);
        }

        template <typename T>
        Matrix< T, 4, 4 > & Matrix<T, 4, 4>::operator-=(const T & val)
        {
            for (u32 i = 0; i < 16; ++i) this->d[i] -= val;
            return (*this);
        }

        template <typename T>
        Matrix< T, 4, 4 > & Matrix<T, 4, 4>::Zero(void)
        {
            memset(data, 0, sizeof(T) * 16);
            return (*this);
        }

        template <typename T>
        const Matrix< T, 4, 4 > Matrix<T, 4, 4>::Transpose(void) const
        {
            Matrix<T, 4, 4> result;

            for (u32 j = 0; j < 4; ++j)
                for (u32 i = 0; i < 4; ++i)
                    result(i, j) = this->operator()(j, i);
            return (result);
        }

        template <typename T>
        Matrix<T, 4, 4> & Matrix<T, 4, 4>::LoadIdentity(void)
        {
            /*
            Zero();
            for (u32 i = 0; i < 4; ++i)
            data[i * 4 + i] = T(1);
            */

            data[0] = 1;
            data[4] = 0;
            data[8]  = 0;
            data[12] = 0;
            data[1] = 0;
            data[5] = 1;
            data[9]  = 0;
            data[13] = 0;
            data[2] = 0;
            data[6] = 0;
            data[10] = 1;
            data[14] = 0;
            data[3] = 0;
            data[7] = 0;
            data[11] = 0;
            data[15] = 1;

            return (*this);
        }

        template <typename T>
        Matrix<T, 4, 4> Matrix<T, 4, 4>::Inverse(void)
        {
            Matrix<T, 4, 4> ret;

            T det;

            det  = data[0] * data[5] * data[10];
            det += data[4] * data[9] * data[2];
            det += data[8] * data[1] * data[6];
            det -= data[8] * data[5] * data[2];
            det -= data[4] * data[1] * data[10];
            det -= data[0] * data[9] * data[6];


            // force double/f32 ??
            if (det)
                det = 1.0 / det;
            /*
            else
            return ; // ???
            */

            ret[0] = (data[5] * data[10] - data[9] * data[6]) * det;
            ret[1] = -(data[1] * data[10] - data[9] * data[2]) * det;
            ret[2] = (data[1] * data[6]  - data[5] * data[2]) * det;
            ret[3] = T(0);
            ret[4] = -(data[4] * data[10] - data[8] * data[6]) * det;
            ret[5] = (data[0] * data[10] - data[8] * data[2]) * det;
            ret[6] = -(data[0] * data[6]  - data[4] * data[2]) * det;
            ret[7] = T(0);
            ret[8] = (data[4] * data[9] - data[8] * data[5])  * det;
            ret[9] = -(data[0] * data[9] - data[8] * data[1])  * det;
            ret[10] = (data[0] * data[5] - data[4] * data[1])  * det;
            ret[11] = T(0);
            ret[12] = -(data[12] * ret[0] + data[13] * ret[4] + data[14] * ret[8]);
            ret[13] = -(data[12] * ret[1] + data[13] * ret[5] + data[14] * ret[9]);
            ret[14] = -(data[12] * ret[2] + data[13] * ret[6] + data[14] * ret[10]);
            ret[15] = T(1);

            return (ret);
        }

        template <typename T>
        void Matrix<T, 4, 4>::Rotation_X(const T angle)
        {
            T rad = DegToRad(angle);
            T c = cos(rad);
            T s = sin(rad);

            data[0] = 1.0;
            data[4] = 0.0;
            data[8]  =  0.0;
            data[12] = 0.0;
            data[1] = 0.0;
            data[5] = c;
            data[9]  = -s;
            data[13] = 0.0;
            data[2] = 0.0;
            data[6] = s;
            data[10] =  c;
            data[14] = 0.0;
            data[3] = 0.0;
            data[7] = 0.0;
            data[11] =  0.0;
            data[15] = 1.0;
        }

        template <typename T>
        void Matrix<T, 4, 4>::Rotation_Y(const T angle)
        {
            T rad = DegToRad(angle);
            T c = cos(rad);
            T s = sin(rad);

            data[0] =  c;
            data[4] = 0.0;
            data[8]  = s;
            data[12] = 0.0;
            data[1] =  0.0;
            data[5] = 1.0;
            data[9]  = 0.0;
            data[13] = 0.0;
            data[2] = -s;
            data[6] = 0.0;
            data[10] = c;
            data[14] = 0.0;
            data[3] =  0.0;
            data[7] = 0.0;
            data[11] = 0.0;
            data[15] = 1.0;
        }

        template <typename T>
        void Matrix<T, 4, 4>::Rotation_Z(const T angle)
        {
            T rad = DegToRad(angle);
            T c = cos(rad);
            T s = sin(rad);

            data[0] = c;
            data[4] = -s;
            data[8]  = 0.0;
            data[12] = 0.0;
            data[1] = s;
            data[5] =  c;
            data[9]  = 0.0;
            data[13] = 0.0;
            data[2] = 0.0;
            data[6] =  0.0;
            data[10] = 1.0;
            data[14] = 0.0;
            data[3] = 0.0;
            data[7] =  0.0;
            data[11] = 0.0;
            data[15] = 1.0;
        }

        template <typename T>
        void Matrix<T, 4, 4>::Translate(const Vector<T, 3> & v)
        {
            data[0] = 1.0;
            data[4] = 0.0;
            data[8]  = 0.0;
            data[12] = v[0];
            data[1] = 0.0;
            data[5] = 1.0;
            data[9]  = 0.0;
            data[13] = v[1];
            data[2] = 0.0;
            data[6] = 0.0;
            data[10] = 1.0;
            data[14] = v[2];
            data[3] = 0.0;
            data[7] = 0.0;
            data[11] = 0.0;
            data[15] = 1.0;
        }

        template <typename T>
        void Matrix<T, 4, 4>::Translate(const T x, const T y, const T z)
        {
            Translate(Vector<T, 3>(x, y, z));
        }

        template <typename T>
        void Matrix<T, 4, 4>::Scale(const Vector<T, 3> & v)
        {
            data[0] = v[0];
            data[4] = 0.0;
            data[8]  = 0.0;
            data[12] = 0.0;
            data[1] = 0.0;
            data[5] = v[1];
            data[9]  = 0.0;
            data[13] = 0.0;
            data[2] = 0.0;
            data[6] = 0.0;
            data[10] = v[2];
            data[14] = 0.0;
            data[3] = 0.0;
            data[7] = 0.0;
            data[11] = 0.0;
            data[15] = 1.0;
        }

        template <typename T>
        void Matrix<T, 4, 4>::Scale(const T x, const T y, const T z)
        {
            Scale(Vector<T, 3>(x, y, z));
        }

        template <typename T>
        void Matrix<T, 4, 4>::Reflect(const Vector<T, 3> & v)
        {
            T x = v.x;
            T y = v.y;
            T z = v.z;
            T x2 = x * 2.0;
            T y2 = y * 2.0;
            T z2 = z * 2.0;

            data[0] = 1.0 - x * x2;
            data[4] = -y * x2;
            data[8] = -z * x2;
            data[12] = -v.w * x2;
            data[1] = -x  * y2;
            data[5] = 1.0 - y * y2;
            data[9] = -z * y2;
            data[13] = -v.w * y2;
            data[2] = -x  * z2;
            data[6] = -y * z2;
            data[10] = 1.0 - z * z2;
            data[14] = -v.w * z2;
            data[3] = 0.0;
            data[7] = 0.0;
            data[11] = 0.0;
            data[15] = 1.0;
        }

        template <typename T>
        void Matrix<T, 4, 4>::Reflect(const T x, const T y, const T z, const T w)
        {
            Reflect(Vector<T, 4>(x, y, z, w));
        }

        template <typename T>
        void Matrix<T, 4, 4>::Look_At(const Vector<T, 3> & eye, const Vector<T, 3> & dir, const Vector<T, 3> & up)
        {
            Vector<T, 3> x, y, z;

            Matrix<T, 4, 4> m0, m1;

            z = eye - dir;

            z.Normalize();
            x = Cross(up, z);
            x.Normalize();
            y = Cross(z, x);
            y.Normalize();

            m0[0] = x[0];
            m0[4] = x[1];
            m0[8] = x[2];
            m0[12] = 0.0;
            m0[1] = y[0];
            m0[5] = y[1];
            m0[9] = y[2];
            m0[13] = 0.0;
            m0[2] = z[0];
            m0[6] = z[1];
            m0[10] = z[2];
            m0[14] = 0.0;
            m0[3] = 0.0;
            m0[7] = 0.0;
            m0[11] = 0.0;
            m0[15] = 1.0;

            m1.Translate(-eye);

            *this = m0 * m1;
        }

        template <typename T>
        void Matrix<T, 4, 4>::Look_At(const T * eye, const T * dir, const T * up)
        {
            Vector<T, 3> * vEye = reinterpret_cast<Vector<T, 3> *>(eye);
            Vector<T, 3> * vDir = reinterpret_cast<Vector<T, 3> *>(dir);
            Vector<T, 3> * vUp  = reinterpret_cast<Vector<T, 3> *>(up);

            Look_At(*vEye, *vDir, *vUp);
        }

        template <typename T>
        const T * Matrix<T, 4, 4>::GetDataPtr(void)
        {
            return (data);
        }

//  Matrix44 inline implementation

// ----------------------------------- STREAM OPERATOR ---------------------

        template <typename T, u32 M, u32 N >
        EW_EXPORT_SYM inline std::ostream & operator<<(std::ostream & stream, const ew::maths::Matrix<T, M, N> & mat)
        {
            for (u32 i = 0; i < M; ++i) {
                stream << "(";
                for (u32 j = 0; j < N; ++j) {
                    stream << mat[j * N + i];
                    if (j < N - 1)
                        stream << " , ";
                }
                stream << ")\n";
            }
            return (stream);
        }

    }
} // ! ew::maths
