#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

// to force creation of ew.core.maths.lib

//EW_MATHS_EXPORT void ew_export_math_func(void);

using namespace ew::core::types;

namespace ew
{
namespace maths
{
namespace constants
{

namespace F32
{
#undef EW_MATHS_EXPORT
#define EW_MATHS_EXPORT
extern EW_MATHS_EXPORT const f32 One_On_180;
extern EW_MATHS_EXPORT const f32 Pi;
extern EW_MATHS_EXPORT const f32 One_On_Pi;
extern EW_MATHS_EXPORT const f32 Two_Pi;
extern EW_MATHS_EXPORT const f32 Half_Pi;
extern EW_MATHS_EXPORT const f32 Sqrt_2;
extern EW_MATHS_EXPORT const f32 Sqrt_3;
extern EW_MATHS_EXPORT const f32 Half_Sqrt_2;
extern EW_MATHS_EXPORT const f32 Half_Sqrt_3;
extern EW_MATHS_EXPORT const f32 _180_On_Pi;
extern EW_MATHS_EXPORT const f32 Pi_On_180;

}

namespace Double
{

extern EW_MATHS_EXPORT const double One_On_180;
extern EW_MATHS_EXPORT const double Pi;
extern EW_MATHS_EXPORT const double One_On_Pi;
extern EW_MATHS_EXPORT const double Two_Pi;
extern EW_MATHS_EXPORT const double Half_Pi;
extern EW_MATHS_EXPORT const double Sqrt_2;
extern EW_MATHS_EXPORT const double Sqrt_3;
extern EW_MATHS_EXPORT const double Half_Sqrt_2;
extern EW_MATHS_EXPORT const double Half_Sqrt_3;
extern EW_MATHS_EXPORT const double _180_On_Pi;
extern EW_MATHS_EXPORT const double Pi_On_180;

}

namespace Long_Double
{

extern EW_MATHS_EXPORT const long double One_On_180;
extern EW_MATHS_EXPORT const long double Pi;
extern EW_MATHS_EXPORT const long double One_On_Pi;
extern EW_MATHS_EXPORT const long double Two_Pi;
extern EW_MATHS_EXPORT const long double Half_Pi;
extern EW_MATHS_EXPORT const long double Sqrt_2;
extern EW_MATHS_EXPORT const long double Sqrt_3;
extern EW_MATHS_EXPORT const long double Half_Sqrt_2;
extern EW_MATHS_EXPORT const long double Half_Sqrt_3;
extern EW_MATHS_EXPORT const long double _180_On_Pi;
extern EW_MATHS_EXPORT const long double Pi_On_180;

}

}
}
} // ! Constants
