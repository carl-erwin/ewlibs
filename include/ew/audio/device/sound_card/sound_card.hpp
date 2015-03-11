
#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/audio/device/audio_device.hpp>


namespace ew
{
namespace audio
{
namespace devices
{

using namespace ew::core::types;

class EW_AUDIO_EXPORT sound_card : public ew::audio::audio_device
{
public:
	sound_card(u32 index) : audio_device(index) { }
};

}
}
}

