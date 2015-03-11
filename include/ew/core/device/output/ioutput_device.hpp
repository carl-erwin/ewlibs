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

using namespace ew::core::types;

class EW_CORE_DEVICE_EXPORT output_device : public ew::core::device
{
private:
	output_device(const output_device &);
	output_device & operator=(const output_device &);
public:
	output_device(u32 index);
};

}
}
}
