/**
 * Deep
 */

#pragma once

/*
* Compiler Macros
*/

#if defined(__clang__)
#define DEEP_COMPILER_CLANG

#elif defined(__GNUC__) || defined(__GNUG__)
#define DEEP_COMPILER_GCC

#elif defined(_MSC_VER)
#define DEEP_COMPILER_MSVC

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

#if defined(DEEP_COMPILER_CLANG)
#define Deep_Inline inline
#define Deep_AlignOf(type) __alignof__(type)

#define Deep_PushPack
#define Deep_PopPack __attribute__((__packed__))

#define Deep_Unreachable __builtin_unreachable()

#define Deep__Function__ __func__
#define Deep__File__ __FILE__
#define Deep__Line__ __LINE__

#elif defined(DEEP_COMPILER_GCC)
#define Deep_Inline inline __attribute__((always_inline))
#define Deep_AlignOf(type) __alignof__(type)

#define Deep_PushPack
#define Deep_PopPack __attribute__((__packed__))

#define Deep_Unreachable __builtin_unreachable()

#define Deep__Function__ __func__
#define Deep__File__ __FILE__
#define Deep__Line__ __LINE__

#elif defined(DEEP_COMPILER_MSVC)
#define Deep_Inline __forceinline
#define Deep_AlignOf(type) alignof(type)

#define Deep_PushPack __pragma(pack(push, 1))
#define Deep_PopPack __pragma(pack(pop))

#define Deep_Unreachable __assume(0)

#define Deep__Function__ __FUNCTION__
#define Deep__File__ __FILE__
#define Deep__Line__ __LINE__

#endif

#ifdef DEEP_COMPILER_MSVC
// NOTE(randomuserhi): Undef MSVC pre-processor macros: https://stackoverflow.com/questions/21483038/undefining-min-and-max-macros
#undef min
#undef max
#endif

// Cache line size used to align to cache line
// Can be defined prior header to set a custom cache line size
#ifndef DEEP_CACHE_LINE_SIZE
#define DEEP_CACHE_LINE_SIZE 64
#endif

/*
* Vectorised Instructions
*/

#if !defined(DEEP_DONT_USE_SIMD_INTRINSICS)

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)

#define DEEP_CPU_X86

#define DEEP_USE_SSE

// Detect enabled instruction sets
#if defined(__AVX512F__) && defined(__AVX512VL__) && defined(__AVX512DQ__) && !defined(DEEP_USE_AVX512)
#define DEEP_USE_AVX512
#endif
#if (defined(__AVX2__) || defined(DEEP_USE_AVX512)) && !defined(DEEP_USE_AVX2)
#define DEEP_USE_AVX2
#endif
#if (defined(__AVX__) || defined(DEEP_USE_AVX2)) && !defined(DEEP_USE_AVX)
#define DEEP_USE_AVX
#endif
#if (defined(__SSE4_2__) || defined(DEEP_USE_AVX)) && !defined(DEEP_USE_SSE4_2)
#define DEEP_USE_SSE4_2
#endif
#if (defined(__SSE4_1__) || defined(DEEP_USE_SSE4_2)) && !defined(DEEP_USE_SSE4_1)
#define DEEP_USE_SSE4_1
#endif
#if (defined(__F16C__) || defined(DEEP_USE_AVX2)) && !defined(DEEP_USE_F16C)
#define DEEP_USE_F16C
#endif
#if (defined(__LZCNT__) || defined(DEEP_USE_AVX2)) && !defined(DEEP_USE_LZCNT)
#define DEEP_USE_LZCNT
#endif
#if (defined(__BMI__) || defined(DEEP_USE_AVX2)) && !defined(DEEP_USE_TZCNT)
#define DEEP_USE_TZCNT
#endif

#else

#error Unsupported CPU architecture

#endif

#endif

/*
* Asserts
*/
#ifndef Deep_Break
#define Deep_Break __debugbreak()
#endif

#include <cassert>

namespace Deep {
    bool OnAssertFailImpl(const char* expression, const char* roFile, uint32 line, const char* message);
    extern bool (*OnAssertFail) (const char* expression, const char* file, uint32 line, const char* message);
    Deep_Inline bool AssertFailed(const char* expression, const char* file, uint32 line, const char* message) {
        if (OnAssertFail == nullptr) return true;
        else return OnAssertFail(expression, file, line, message);
    }
}

#ifdef DEEP_ENABLE_ASSERTS
#define Deep_Assert(expression, message) do { if (!(expression) && ::Deep::AssertFailed(#expression, __FILE__, static_cast<uint32>(__LINE__), message)) { Deep_Break; } } while (false)
#else
#define Deep_Assert(...) ((void)0)
#endif

// TODO(randomuserhi): Move style guide elsewhere -> include types in style guide
/*
* Style Guide
*
* - Function parameters that include the "rw" prefix annotate that the parameter is read-write and the function both reads to the parameter as well as writes to it. Commonly known as an in-out-param.
* - Function parameters that include the "wo" prefix annotate that the parameter is write-only and the function only writes to the parameter. Commonly known as an out-param.
* - Function parameters that do not start with a prefix are always read only.
*/