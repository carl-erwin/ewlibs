#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>

namespace ew
{
namespace core
{
namespace objects
{

using namespace ew::core::types;

class EW_CORE_OBJECT_EXPORT read_interface
{
public:
	virtual ~read_interface()
	{

	}

	virtual bool read(u8 * buffer, u64 nrToRead , u64 * nrRead) = 0;
};

}
}
} // ! ew::core::objects
