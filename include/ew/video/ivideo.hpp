#pragma once

#include <ew/ew_config.hpp>

#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>

#include <ew/video/device/graphic_card/igraphic_card.hpp>

namespace ew
{
namespace video
{

using namespace ew::core::types;
using namespace ew::video::devices; // ::GRAPHIC_CARD; ?

class EW_VIDEO_EXPORT video_interface : public ew::core::object
{
public:
	video_interface();
	virtual ~video_interface();

	virtual u32 getNumberOfVideoDevices(void) = 0;
	// virtual IGraphicCard * getGraphicCard(u32 index) = 0;
	// virtual void releaseGraphicCard(IGraphicCard * igraphicCard) = 0;
};

}
} // ! namespace ew::Video
