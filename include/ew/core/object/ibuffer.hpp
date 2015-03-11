#pragma once

#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>

//                  namespace buffer ?
namespace ew
{
namespace core
{
namespace objects
{

using namespace ew::core::types;

// template ?
class EW_CORE_OBJECT_EXPORT IBuffer : virtual public ew::core::object
{
public:
	const char * class_name() const
	{
		return "ew::core::objects::IBuffer";
	};
	virtual bool alloc(u64 sz) = 0;
	virtual bool release(void) = 0;

	virtual bool realloc(u64 sz) = 0;
	virtual bool resize(u64 sz, bool clear = false) = 0;

	/// access
	// virtual const u8 * get( u64 pos , Ptr * out);
	// virtual u8 * operator[] (u64 pos) = 0;
};

}
}
}
