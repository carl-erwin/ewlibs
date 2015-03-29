#pragma once

#ifdef EDITOR_EXPORT
#error "EDITOR_EXPORT is already defined"
#endif

#ifdef WIN32 // find better define

# define SHOW_SYMBOL    __declspec(dllexport)

#else

// gcc / clang
# define EDITOR_EXPORT   __attribute__ ((visibility ("default")))

#endif
