#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/object/object.hpp>

namespace ew
{
namespace core
{
namespace objects   // ?
{

class EW_CORE_OBJECT_EXPORT runable_object : virtual public object
{
public:
	virtual void run() = 0;
	virtual const char * get_class_name() const
	{
		return "ew::core::objects::runable_object";
	};
};

}
}
} // ! ew::core::object
