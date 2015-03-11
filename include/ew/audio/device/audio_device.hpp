#pragma once

#include <ew/core/device/input_output/iinput_output_device.hpp>


namespace ew
{
namespace audio
{

using namespace ew::core::types;

// IAudioDevice ?
class EW_AUDIO_EXPORT audio_device : public ew::core::devices::input_output::input_output_device
{
public:
	audio_device(u32 index) : input_output_device(index) { }
};


}
}
