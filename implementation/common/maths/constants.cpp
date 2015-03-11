
#include <cmath>

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/maths/constants.hpp>


//EW_MATHS_EXPORT void ew_export_math_func(void) {}

//#pragma GCC visibility push(default)

namespace ew
{
namespace maths
{
namespace constants
{

using namespace ew::core::types;

namespace float32
{

// const f32 One_On_180 = 1.0f / 180.0f;
// const f32 Pi = 3.1415926535897932384626433832795f;
// const f32 One_On_Pi = 1.0f / Pi;
// const f32 Two_Pi = 2.0f * Pi;
// const f32 Half_Pi = 0.5f * Pi;
const f32 Sqrt_2 = sqrtf(2.0f);
const f32 Sqrt_3 = sqrtf(3.0f);
const f32 Half_Sqrt_2 = 0.5f * sqrtf(2.0f);
const f32 Half_Sqrt_3 = 0.5f * sqrtf(3.0f);
// const f32 _180_On_Pi = 180.0f / Pi;
// const f32 Pi_On_180 = Pi / 180.0f;

}

namespace Double
{

const double One_On_180 = 1.0 / 180.0;
const double Pi = (double) 3.1415926535897932384626433832795;
const double One_On_Pi = 1.0 / Pi;
const double Two_Pi = 2.0 * Pi;
const double Half_Pi = 0.5 * Pi;
const double Sqrt_2 = sqrtf(2.0);
const double Sqrt_3 = sqrtf(3.0);
const double Half_Sqrt_2 = 0.5 * sqrtf(2.0);
const double Half_Sqrt_3 = 0.5 * sqrtf(3.0);
const double _180_On_Pi = 180.0 / Pi;
const double Pi_On_180 = Pi / 180.0f;

}


namespace Long_Double
{

const long double One_On_180 = (long double) 1.0 / (long double) 180.0;
const long double Pi = 3.1415926535897932384626433832795;
const long double One_On_Pi = (long double) 1.0 / Pi;
const long double Two_Pi = (long double) 2.0 * Pi;
const long double Half_Pi = (long double) 0.5 * Pi;
const long double Sqrt_2 = sqrtf((long double) 2.0);
const long double Sqrt_3 = sqrtf((long double) 3.0);
const long double Half_Sqrt_2 = (long double) 0.5 * sqrtf((long double) 2.0);
const long double Half_Sqrt_3 = (long double) 0.5 * sqrtf((long double) 3.0);
const long double _180_On_Pi = (long double) 180.0 / Pi;
const long double Pi_On_180 = Pi / (long double) 180.0;
}

}
}
} // ! Constants

//#pragma GCC visibility pop

