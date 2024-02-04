/**
 * Deep
 */

#pragma once

 /*
 * Compiler Macros
 */

#if defined(__clang__)
#define Deep_Compiler_Clang

#elif defined(__GNUC__) || defined(__GNUG__)
#define Deep_Compiler_GCC

#elif defined(_MSC_VER)
#define Deep_Compiler_MSCV

#endif

 /*
 * Platform Macros
 */

#if defined(_WIN32)
#define DEEP_PLATFORM_WINDOWS

#elif defined(__APPLE__)
#define DEEP_PLATFORM_MAC

#else
#define DEEP_PLATFORM_UNIX

#endif

 /*
 * Deep Utilities
 */

#include "./Deep/Deep_Types.h"

#if defined(Deep_Compiler_Clang)
#define Deep_Inline inline
#define Deep_AlignOf(type) __alignof__(type)

#define Deep_PushPack
#define Deep_PopPack __attribute__((__packed__))

#define Deep_Unreachable __builtin_unreachable()

#define Deep__Function__ __func__
#define Deep__File__ __FILE__
#define Deep__Line__ __LINE__

#elif defined(Deep_Compiler_GCC)
#define Deep_Inline inline __attribute__((always_inline))
#define Deep_AlignOf(type) __alignof__(type)

#define Deep_PushPack
#define Deep_PopPack __attribute__((__packed__))

#define Deep_Unreachable __builtin_unreachable()

#define Deep__Function__ __func__
#define Deep__File__ __FILE__
#define Deep__Line__ __LINE__

#elif defined(Deep_Compiler_MSCV)
#define Deep_Inline __forceinline
#define Deep_AlignOf(type) _Alignof(type)

#define Deep_PushPack __pragma(pack(push, 1))
#define Deep_PopPack __pragma(pack(pop))

#define Deep_Unreachable __assume(0)

#define Deep__Function__ __FUNCTION__
#define Deep__File__ __FILE__
#define Deep__Line__ __LINE__

#endif