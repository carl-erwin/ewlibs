#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/device/idevice.hpp>

namespace ew
{
namespace core
{
namespace devices
{
namespace input
{

using namespace ew::core::types;

class EW_CORE_DEVICE_EXPORT input_device : public ew::core::device
{
public:
	input_device(u32 index);
};

}
}
}
}


