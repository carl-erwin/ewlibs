#pragma once

// ew
#include <ew/ew_config.hpp>
#include <ew/core/threading/mutex.hpp>
#include <ew/core/object/object_locker.hpp>


namespace ew
{
namespace core
{
namespace threading
{

typedef ew::core::objects::object_locker < ew::core::threading::mutex > mutex_locker;

}
}
}
