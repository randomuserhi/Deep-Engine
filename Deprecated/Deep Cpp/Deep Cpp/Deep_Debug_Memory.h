#pragma once

#include "Deep.h"

#if !defined(DEEP_DEBUG_MEMORY_LOGFILE)
#define DEEP_DEBUG_MEMORY_LOGFILE "memory_allocations.txt"
#endif

/*
* It should be noted that this debugger is very crude and fails to compile with placement new
* operations. (In order to do so requires #undef new, such that placement new operations may work as intended).
* As placement new is not an actual memory allocation, after undef simply redefine new using #include "Deep_Debug_Memory_Def.h"
* 
* void* ptr = malloc(sizeof(float));
* #include "Deep_Debug_Memory_Undef.h"
* float* f = new(ptr) float(20.0f);
* #include "Deep_Debug_Memory_Def.h"
* // Continue as normal
* free(ptr);
* 
* Ideally, a tool like valgrind is used as opposed to this.
*/
#if defined(DEEP_DEBUG_MEMORY)
//Based on https://www.embeddedsystemonline.com/programming-languages/cplusplus/program-to-track-memory-allocations-in-c-or-c

#if defined(Deep_Compiler_MSCV)
_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
#endif
void* __cdecl operator new (size_t size);

#if defined(Deep_Compiler_MSCV)
_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
#endif
void* __cdecl operator new[](size_t size);

void* operator new (size_t size, const char* file, int line, const char* function);
void* operator new[] (size_t size, const char* file, int line, const char* function);
void operator delete (void* ptr) throw();
void operator delete[] (void* ptr) throw ();

namespace Deep
{
	extern bool DEBUG_MEMORY__SET__;
	extern const char* DEBUG_MEMORY__FILE__;
	extern const char* DEBUG_MEMORY__FUNCTION__;
	extern int DEBUG_MEMORY__LINE__;

	void StartMemoryDebug();
	void EndMemoryDebug();

	void PrintAllocationMap();

	void* Malloc(size_t size, const char* file, int line, const char* function);
	void* Calloc(size_t count, size_t typeSize, const char* file, int line, const char* function);
	void* Realloc(void* ptr, size_t size, const char* file, int line, const char* function);
	void Free(void* ptr, const char* file, int line, const char* function);
}

#include "Deep_Debug_Memory_Def.h"

#endif
