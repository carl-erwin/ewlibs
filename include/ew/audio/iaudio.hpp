#pragma once

#include <ew/ew_config.hpp>

#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>

#include <ew/audio/device/sound_card/sound_card.hpp>


namespace ew
{
namespace audio
{

using namespace ew::core::types;

class EW_AUDIO_EXPORT audio_interface : public ew::core::object
{
public:
	audio_interface();

	virtual ~audio_interface();

	virtual u32 getNumberOfAudioDevices(void) = 0;
	virtual ew::audio::devices::sound_card * getSoundCard(u32 index) = 0;
	virtual void releaseSoundCard(ew::audio::devices::sound_card * soundCard) = 0;
};

}
} // ! namespace ew::audio

