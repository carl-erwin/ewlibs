#pragma once

namespace eedit
{

// replace by dll laoding + "C" int module_init(); -> register dll fn
namespace core
{
void text_mode_register_modules_function();
}

}
