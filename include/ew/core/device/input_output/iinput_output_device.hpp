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
namespace input_output
{

using namespace ew::core::types;

class EW_CORE_DEVICE_EXPORT input_output_device : public ew::core::device
{
private:
	input_output_device(const input_output_device &);
	input_output_device & operator=(const input_output_device &);
public:
	input_output_device(u32 index);
};

}
}
}
}
