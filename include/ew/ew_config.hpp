/*
 *
 *  Copyright (C) Carl-Erwin Griffith
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#ifdef WIN32
# define EW_TARGET_SYSTEM_WIN32 1
# define EW_TARGET_SYSTEM_WINDOWS 1
# define EW_COMPILER_MSVC 1
#endif

#ifdef __linux__
# define EW_TARGET_SYSTEM_LINUX 1
# ifndef  EW_TARGET_SYSTEM_UNIX
#  define EW_TARGET_SYSTEM_UNIX 1
# endif
# define EW_COMPILER_GCC 1
#endif


#ifdef APPLE
# define EW_TARGET_SYSTEM_APPLE 1
# ifndef EW_TARGET_SYSTEM_UNIX
#  define EW_TARGET_SYSTEM_UNIX 1
# endif
# define EW_COMPILER_GCC 1
#endif


#ifdef __FreeBSD__
# define EW_TARGET_SYSTEM_FREEBSD 1
# define EW_TARGET_SYSTEM_UNIX 1
# define EW_COMPILER_GCC 1
#endif

#ifdef __CYGWIN__
# define EW_TARGET_SYSTEM_CYGWIN 1
# define EW_TARGET_SYSTEM_UNIX 1 // ?
# define EW_COMPILER_GCC 1
#endif

#undef  EW_EXPORT_SYM
#define EW_EXPORT_SYM

#undef  EW_IMPORT_SYM
#define EW_IMPORT_SYM

#undef  EW_ALWAYS_INLINE
#define EW_ALWAYS_INLINE

////////////////////////////////
//
// General debuging macro
//
////////////////////////////////

// #define EW_ENABLE_DEBUG

#ifdef EW_ENABLE_DEBUG
# define EW_DEBUG(...) __VA_ARGS__
#else
# define EW_DEBUG(...)
#endif


////////////////////////////////
//
// some compiler facilities
//
////////////////////////////////

#if (__GNUC__ >= 3)

# ifndef likely
#  define likely(x)   __builtin_expect(!!(x), 1)
# endif

# ifndef unlikely
#  define unlikely(x) __builtin_expect(!!(x), 0)
# endif

#else  /* ! __GNUC__ >= 3 */

# ifndef likely
#   define likely(x) (x)
#  endif

# ifndef unlikely
#  define unlikely(x) (x)
# endif

#endif  /* ! __GNUC__ >= 3 */

////////////////////////////////
//
// symbols exporting/importing
// thread local storage
// inline
//
////////////////////////////////

#ifdef EW_TARGET_SYSTEM_WIN32

# undef EW_EXPORT_SYM
# undef EW_IMPORT_SYM

# define HIDE_SYMBOL
# define SHOW_SYMBOL    __declspec(dllexport)
# define EW_EXPORT_SYM  __declspec(dllexport)
# define EW_IMPORT_SYM  __declspec(dllimport)
# define TLS_DECL       __declspec(thread)

# pragma warning (disable : 4251)
# pragma warning (disable : 4100)   // unused param
# pragma warning (disable : 4201)   // nameless struct/union
# pragma warning (disable : 4996)   // strncpy

#endif

// replace by __GCC
#ifdef EW_TARGET_SYSTEM_UNIX

# undef EW_EXPORT_SYM
# undef EW_IMPORT_SYM
# undef ALWAYS_INLINE

// we should check compiler support
# define TLS_DECL __thread
# define ALWAYS_INLINE __attribute__ ((always_inline))
# define HIDE_SYMBOL   __attribute__ ((visibility ("hidden")))
# define SHOW_SYMBOL   __attribute__ ((visibility ("default")))

# define EW_NO_EXPORT  HIDE_SYMBOL
# define EW_EXPORT_SYM SHOW_SYMBOL
# define EW_IMPORT_SYM

#endif

// ew.core
#ifdef DO_EW_CORE_EXPORT
# define EW_CORE_EXPORT EW_EXPORT_SYM
#else
# define EW_CORE_EXPORT EW_IMPORT_SYM
#endif

// ew.core
#ifdef DO_EW_CORE_SYSCALL_EXPORT
# define EW_CORE_SYSCALL_EXPORT EW_EXPORT_SYM
#else
# define EW_CORE_SYSCALL_EXPORT EW_IMPORT_SYM
#endif

// ew.core.object
#ifdef DO_EW_CORE_OBJECT_EXPORT
# define EW_CORE_OBJECT_EXPORT EW_EXPORT_SYM
#else
# define EW_CORE_OBJECT_EXPORT EW_IMPORT_SYM
#endif

// ew.core.program
#ifdef DO_EW_CORE_PROGRAM_EXPORT
# define EW_CORE_PROGRAM_EXPORT EW_EXPORT_SYM
#else
# define EW_CORE_PROGRAM_EXPORT EW_IMPORT_SYM
#endif

// ew.core.THREAD
#ifdef DO_EW_CORE_THREAD_EXPORT
# define EW_CORE_THREAD_EXPORT EW_EXPORT_SYM
#else
# define EW_CORE_THREAD_EXPORT EW_IMPORT_SYM
#endif

// ew.core.exception
#ifdef DO_EW_CORE_EXCEPTION_EXPORT
# define EW_CORE_EXCEPTION_EXPORT EW_EXPORT_SYM
#else
# define EW_CORE_EXCEPTION_EXPORT EW_IMPORT_SYM
#endif

// ew.core.MUTEX
#ifdef DO_EW_CORE_MUTEX_EXPORT
# define EW_CORE_MUTEX_EXPORT EW_EXPORT_SYM
#else
# define EW_CORE_MUTEX_EXPORT EW_IMPORT_SYM
#endif

// ew.core.dll
#ifdef DO_EW_CORE_DLL_EXPORT
# define EW_CORE_DLL_EXPORT EW_EXPORT_SYM
#else
# define EW_CORE_DLL_EXPORT EW_IMPORT_SYM
#endif

// ew.core.time
#ifdef DO_EW_CORE_TIME_EXPORT
# define EW_CORE_TIME_EXPORT EW_EXPORT_SYM
#else
# define EW_CORE_TIME_EXPORT EW_IMPORT_SYM
#endif

// ew.core.device
#ifdef DO_EW_CORE_DEVICE_EXPORT
# define EW_CORE_DEVICE_EXPORT EW_EXPORT_SYM
#else
# define EW_CORE_DEVICE_EXPORT EW_IMPORT_SYM
#endif

// ew.core.maths
#ifdef DO_EW_MATHS_EXPORT
# define EW_MATHS_EXPORT EW_EXPORT_SYM
#else
# define EW_MATHS_EXPORT EW_IMPORT_SYM
#endif

// ew.Physics
#ifdef EW_EXPORT_PHYSICS_INTERFACE
# define EW_PHYSICS_INTERFACE_DECLSPEC EW_EXPORT_SYM
#else
# define EW_PHYSICS_INTERFACE_DECLSPEC EW_IMPORT_SYM
#endif

// ew.graphics
#ifdef DO_EW_GRAPHICS_EXPORT
# define EW_GRAPHICS_EXPORT EW_EXPORT_SYM
#else
# define EW_GRAPHICS_EXPORT EW_IMPORT_SYM
#endif

// ew.Video
#ifdef DO_EW_VIDEO_EXPORT
# define EW_VIDEO_EXPORT EW_EXPORT_SYM
#else
# define EW_VIDEO_EXPORT EW_IMPORT_SYM
#endif

// ew.audio
#ifdef DO_EW_AUDIO_EXPORT
# define EW_AUDIO_EXPORT EW_EXPORT_SYM
#else
# define EW_AUDIO_EXPORT EW_IMPORT_SYM
#endif

// ew.User
#ifdef DO_EW_USER_EXPORT
# define EW_USER_EXPORT EW_EXPORT_SYM
#else
# define EW_USER_EXPORT EW_IMPORT_SYM
#endif

// ew.graphics.renderer.OpenGL
#ifdef DO_EW_GRAPHICS_RENDERING_OPENGL_EXPORT
# define EW_GRAPHICS_RENDERING_OPENGL_EXPORT EW_EXPORT_SYM
#else
# define EW_GRAPHICS_RENDERING_OPENGL_EXPORT EW_IMPORT_SYM
#endif

// ew.Platform.Unix
#ifdef DO_EW_PLATFORM_UNXI_EXPORT
# define EW_PLATFORM_UNXI_EXPORT EW_EXPORT_SYM
#else
# define EW_PLATFORM_UNXI_EXPORT EW_IMPORT_SYM
#endif

// ew.Platform.Linux
#ifdef DO_EW_PLATFORM_LINUX_EXPORT
# define EW_PLATFORM_LINUX_EXPORT EW_EXPORT_SYM
#else
# define EW_PLATFORM_LINUX_EXPORT EW_IMPORT_SYM
#endif

// ew.Platform.SDL
#ifdef DO_EW_PLATFORM_SDL_EXPORT
# define EW_PLATFORM_SDL_EXPORT EW_EXPORT_SYM
#else
# define EW_PLATFORM_SDL_EXPORT EW_IMPORT_SYM
#endif

// Platform
#ifdef DO_EW_PLATFORM_EXPORT
# define EW_PLATFORM_EXPORT EW_EXPORT_SYM
#else
# define EW_PLATFORM_EXPORT EW_IMPORT_SYM
#endif

// ew.core.system
#ifdef DO_EW_SYSTEM_EXPORT
# define EW_SYSTEM_EXPORT EW_EXPORT_SYM
#else
# define EW_SYSTEM_EXPORT EW_IMPORT_SYM
#endif

// ew.filesystem
#ifdef DO_EW_FILESYSTEM_EXPORT
# define EW_FILESYSTEM_EXPORT EW_EXPORT_SYM
#else
# define EW_FILESYSTEM_EXPORT EW_IMPORT_SYM
#endif

// ew.NETWORK
#ifdef DO_EW_NETWORK_EXPORT
# define EW_NETWORK_EXPORT EW_EXPORT_SYM
#else
# define EW_NETWORK_EXPORT EW_IMPORT_SYM
#endif

// ew.codecs.text.unicode.utf8
#ifdef DO_EW_CODECS_TEXT_UNICODE_UTF8
# define EW_CODECS_TEXT_UNICODE_UTF8_EXPORT EW_EXPORT_SYM
#else
# define EW_CODECS_TEXT_UNICODE_UTF8_EXPORT EW_IMPORT_SYM
#endif

// ew.Codecs.Text
#ifdef DO_EW_CODECS_TEXT
# define EW_CODECS_TEXT_EXPORT EW_EXPORT_SYM
#else
# define EW_CODECS_TEXT_EXPORT EW_IMPORT_SYM
#endif


// system includes for basic types
#ifdef EW_TARGET_SYSTEM_UNIX
# include <sys/types.h>
# include <stdint.h>
# include <inttypes.h>
#endif

#ifdef  EW_TARGET_SYSTEM_WINDOWS

typedef unsigned __int64 uint64_t;
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef signed __int64 int64_t;
typedef signed long int32_t;
typedef signed short int16_t;
typedef signed char int8_t;

typedef signed long ssize_t;

#endif
