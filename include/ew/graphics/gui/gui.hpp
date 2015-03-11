#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/object/object.hpp>

namespace ew
{
namespace graphics
{
namespace gui
{

/* remove this ? */
bool EW_GRAPHICS_EXPORT init();
bool EW_GRAPHICS_EXPORT quit();

bool EW_GRAPHICS_EXPORT setSinglethreadEventPollingMode();
bool EW_GRAPHICS_EXPORT singlethreadEventPollingIsEnabled();
bool EW_GRAPHICS_EXPORT multithreadEventPollingIsEnabled();

class EW_GRAPHICS_EXPORT gui : public ew::core::object
{
public:
	gui();
	virtual ~gui();

	virtual const char * class_name() const;
};

}
}
}

