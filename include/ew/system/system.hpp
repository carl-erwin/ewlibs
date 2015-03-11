#pragma once

// ew
#include <ew/ew_config.hpp>
#include <ew/core/types/types.hpp>

// commons system calls
// s32 exit()
//
// Todo:
// ew::system::Program::IProgram * execute("path/to/prog")

namespace ew
{
namespace system
{

using namespace ew::core::types;

EW_SYSTEM_EXPORT void exit(ssize_t status);

}
}

