#ifndef h_Deep
#define h_Deep

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
* ============== /  Library Format  \ ==============
* 
* -> All Deep library functions are prefixed by "Deep"
* 
* -> Private library implementation functions / variables 
*    are prefixed by "_Deep"
* 
* ============== | Templated Format | ==============
* 
* -> Type safe functions are formatted using:
*        <function name>_<type1>_<seperator phrase>_<type2>_...
*    -> e.g Deep_DoSomething_int_To_float()
* 
* -> Type safe functions that operate on a "this" type are formatted:
*        <this type>_<type1>_<seperator phrase>_<type2>_..._<function name>
*    -> e.g Deep_DynArray_int_Push or Deep_Dictionary_int_To_float_Insert()
* 
* -> All containers that have type safety also expose the raw version of 
*    themselves where type safety is not present. These can be used to
*    reduce code size if you don't want to Decl several times.
* 
* -> Some headers will Decl some templated types. To avoid duplicate declarations,
*    Decl in headers are wrapped in an ifndef using their tag prefixed with the
*    Decl type:
*    
*	 #ifndef Deep_DynArray_Decl_longlong
*    #define Deep_DynArray_Decl_longlong
* 
*	 Deep_DynArray_Decl(long long, longlong)
* 
*    #endif
* 
*    -> It is advised that the user is to do the same when using Decl in headers.
* 
* ============== \ ================ / ==============
*/

/*
* Deep Utilities
*/

#if defined(Deep_Compiler_Clang)
#define Deep_Inline inline
#define Deep_AlignOf(type) __alignof__(type)

#define Deep_Unreachable __builtin_unreachable()

#elif defined(Deep_Compiler_GCC)
#define Deep_Inline inline __attribute__((always_inline))
#define Deep_AlignOf(type) __alignof__(type)

#define Deep_Unreachable __builtin_unreachable()

#elif defined(Deep_Compiler_MSCV)
#define Deep_Inline inline
#define Deep_AlignOf(type) _Alignof(type)

#define Deep_Unreachable __assume(0)

#endif

#define Deep_MallocFreeTracker

// https://stackoverflow.com/a/9074274/9642458
#if defined(Deep_MallocFreeTracker)

#define malloc(size) Deep_Malloc(size, __FILE__, __LINE__, __FUNCTION__)
#define free(ptr) Deep_Free(ptr, __FILE__, __LINE__, __FUNCTION__)

extern size_t mallocCount;
extern size_t freeCount;

void* Deep_Malloc(size_t size, const char* file, int line, const char* function);
void Deep_Free(void* ptr, const char* file, int line, const char* function);

#endif

/*
* Deep Macros
*/

#if INTPTR_MAX == INT64_MAX
#define Deep_SizeOf_SizeT 8

#elif INTPTR_MAX == INT32_MAX
#define Deep_SizeOf_SizeT 4

#else
#error Unknown pointer size or missing size macros!
#endif

/*
* Deep Utility Functions
*/

void Deep_CheckMaxAllocationSize();

#endif

