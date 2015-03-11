#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/device/output/ioutput_device.hpp>

namespace ew
{
namespace video
{
namespace devices
{

using namespace ew::core::types;

// extern class IGraphicCard;
// faire une super class ivideo device ??
// je pense que oui
// si on faire device camera

class EW_VIDEO_EXPORT graphic_card : public ew::core::devices::output_device
{
public:
	graphic_card(u32 index);
	virtual ~graphic_card();

	virtual bool setResolution(u32 width, u32 height, u32 bpp) = 0;
};

}
}
}
