#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/object/object.hpp>


namespace ew
{
namespace core
{
namespace objects
{

/** \class ILoadableObject
*   \brief a simple interface to present loadind methods
*   \todo add bool reload();
*/
class EW_CORE_OBJECT_EXPORT loadable_object : virtual public ew::core::object
{
public:
	virtual const char * class_name() const
	{
		return "ew::core::objects::loadables_object";
	};

	virtual bool load(void) = 0;
	virtual bool unload(void) = 0;
	virtual bool is_loaded(void) = 0;
};


}
}
}
