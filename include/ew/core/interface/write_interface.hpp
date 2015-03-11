#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/object/object.hpp>
#include <ew/core/object/ibuffer.hpp>

namespace ew
{
namespace core
{
namespace objects
{

class EW_CORE_OBJECT_EXPORT write_interface
{
public:
	virtual ~write_interface()
	{

	}

	virtual bool write(const u8 * buffer, u64 nr_to_write , u64 * nr_written) = 0;
};

}
}
} // ! ew::core::objects
