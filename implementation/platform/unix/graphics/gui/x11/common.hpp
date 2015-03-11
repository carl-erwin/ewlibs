#pragma once

// REMOVE all using ...

#include <assert.h>
#include <ew/core/threading/thread.hpp>
using namespace ew::core::threading;

#include <ew/console/console.hpp>
using ew::console::dbg;

#include <ew/codecs/text/unicode/utf8/utf8.hpp>

using namespace ew::core;

#include "../display_private_data.hpp"
#include "../x11_libX11.hpp"
#include "../libX11_wrappers.hpp"

#include "../../../core/syscall/syscall.hpp"
using namespace ew::core::syscall::unix_system;
