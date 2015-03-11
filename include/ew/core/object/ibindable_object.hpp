#pragma once

#include <ew/ew_config.hpp>

namespace ew
{
namespace core
{
namespace objects
{

class EW_CORE_OBJECT_EXPORT bindable_object
{
public:
	virtual ~bindable_object() {}
	virtual bool bind() = 0;
	virtual bool unbind() = 0;
};

}
}
}
