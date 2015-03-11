#pragma once

#include <ew/ew_config.hpp>

namespace ew
{
namespace core
{
namespace objects
{

template <typename T> class object_locker
{
private:
	T * _obj;
public:
	object_locker(T & obj) : _obj(&obj)
	{
		_obj->lock();
	}
	object_locker(T * obj) : _obj(obj)
	{
		_obj->lock();
	}
	~object_locker()
	{
		_obj->unlock();
	}
};

}
}
}
