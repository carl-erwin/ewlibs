#pragma once

#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

namespace ew
{
namespace core
{
namespace program
{

EW_CORE_PROGRAM_EXPORT char * getenv(const char * var);
EW_CORE_PROGRAM_EXPORT bool setenv(char * var, char * value);
EW_CORE_PROGRAM_EXPORT bool unsetenv(char * var);

}
}
}
