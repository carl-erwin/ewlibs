#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/object/object.hpp>


namespace ew
{
namespace core
{
namespace opbjects
{

class EW_CORE_OBJECT_EXPORT noncopyable_object : virtual public ew::core::object
{
	const char * class_name() const
	{
		return "ew::core::objects::noncopyable_object";
	};
private:
	noncopyable_object(const noncopyable_object &);
	noncopyable_object & operator=(const noncopyable_object &);
};

}
}
} // ! ew::core::object
