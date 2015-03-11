#pragma once

#include <ew/core/types/types.hpp>

namespace ew
{
namespace core
{
namespace time
{

using namespace ew::core::types;

EW_CORE_TIME_EXPORT bool init(void);
EW_CORE_TIME_EXPORT bool quit(void);

EW_CORE_TIME_EXPORT size_t get_seconds_since_startup(void);
EW_CORE_TIME_EXPORT size_t get_milliseconds_since_startup(void);
EW_CORE_TIME_EXPORT size_t get_microseconds_since_startup(void);
EW_CORE_TIME_EXPORT size_t get_nanoseconds_since_startup(void);
EW_CORE_TIME_EXPORT size_t get_ticks(void);

EW_CORE_TIME_EXPORT void sleep(size_t nr_milliseconds);
EW_CORE_TIME_EXPORT void usleep(size_t nr_microseconds);
EW_CORE_TIME_EXPORT void nsleep(size_t nr_nanoseconds);

}
}
} // ! ew::core::time
