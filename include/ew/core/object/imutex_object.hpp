#pragma once

// ew
#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>
#include <ew/core/object/object.hpp>
#include <ew/core/object/imutex_object.hpp>

namespace ew
{
namespace core
{
namespace objects
{

class EW_CORE_OBJECT_EXPORT mutex_object : virtual public object
{
private:
	mutex_object(const mutex_object &);
	mutex_object & operator= (const mutex_object &);

public:
	mutex_object();
	virtual ~mutex_object();

	// object
	virtual const char * get_class_name() const;

	// mutex
	virtual bool lock();
	virtual bool trylock();
	virtual bool unlock();

#if 0
private:
	class private_data;
	class private_data * const d;
#endif
};

}
}
}
