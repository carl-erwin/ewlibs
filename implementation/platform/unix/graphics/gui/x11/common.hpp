#pragma once

// REMOVE all using ...

#include <assert.h>
#include <thread>


#include <iostream>
using std::cerr;

#include <ew/codecs/text/unicode/utf8/utf8.hpp>

using namespace ew::core;

#include "../display_private_data.hpp"
#include "../x11_libX11.hpp"
#include "../libX11_wrappers.hpp"

#include "../../../core/syscall/syscall.hpp"
using namespace ew::core::syscall::unix_system;
