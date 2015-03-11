#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/object/object.hpp>

namespace ew
{
namespace core
{
namespace objects
{

class EW_CORE_OBJECT_EXPORT lock_interface
{
public:
	virtual ~lock_interface()
	{

	}

	const char * get_class_name() const
	{
		return "ew::core::objects::lock_interface";
	};

	virtual bool lock(void)  = 0;
	virtual bool unlock(void) = 0;
	virtual bool trylock(void) = 0;
};

}
}
}
