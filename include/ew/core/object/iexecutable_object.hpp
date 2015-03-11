#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/object/object.hpp>

namespace ew
{
namespace core
{
namespace objects
{

class EW_CORE_OBJECT_EXPORT executable_object : virtual public ew::core::object
{
public:
	virtual bool exec(void) = 0;
};

}
}
} // ! ew::core::objects
