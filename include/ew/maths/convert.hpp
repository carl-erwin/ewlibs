#pragma once

#include <ew/maths/constants.hpp>

namespace ew
{
namespace maths
{
namespace constants
{

namespace F32
{

template <typename T>
inline T Rad2Deg(T value)
{
	return (value * Pi_On_180);
}

template <typename T>
inline T Deg2Rad(T value)
{
	return (value * _180_On_Pi);
}

} // ! ew::F32

namespace Double
{

template <typename T>
inline T Rad2Deg(T value)
{
	return (value * Pi_On_180);
}

template <typename T>
inline T Deg2Rad(T value)
{
	return (value * _180_On_Pi);
}

}

namespace Long_Double
{

template <typename T>
inline T Rad2Deg(T value)
{
	return (value * Pi_On_180);
}

template <typename T>
inline T Deg2Rad(T value)
{
	return (value * _180_On_Pi);
}

}


}
}
} // ! ew::constants
