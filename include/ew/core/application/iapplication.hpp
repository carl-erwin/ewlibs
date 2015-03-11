#pragma once

#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>


namespace ew
{
namespace core
{
namespace application
{

using namespace ew::core::types;

class EW_CORE_EXPORT IApplication : public ew::core::object
{
public:
	IApplication();
	virtual ~IApplication();

	virtual const char * class_name() const;

	virtual bool init(u32 width, u32 height, u32 bpp, bool resize, bool fullscreen) = 0;

	virtual void run(void) = 0;
	virtual s32 quit(void) = 0;

};

}
}
} // ! ew::core::Application
