#ifndef h_Deep_Debug_Memory
#define h_Deep_Debug_Memory

#include "Deep.h"

// https://stackoverflow.com/a/9074274/9642458
#if defined(DEEP_DEBUG_MEMORY)

#define malloc(size) Deep_Debug_Memory_Malloc(size, __FILE__, __LINE__, __FUNCTION__)
#define calloc(count, typeSize) Deep_Debug_Memory_Calloc(count, typeSize, __FILE__, __LINE__, __FUNCTION__)
#define realloc(ptr, size) Deep_Debug_Memory_Realloc(ptr, size, __FILE__, __LINE__, __FUNCTION__)
#define free(ptr) Deep_Debug_Memory_Free(ptr, __FILE__, __LINE__, __FUNCTION__)

void* Deep_Debug_Memory_Malloc(size_t size, const char* file, int line, const char* function);
void* Deep_Debug_Memory_Calloc(size_t count, size_t typeSize, const char* file, int line, const char* function);
void* Deep_Debug_Memory_Realloc(void* ptr, size_t size, const char* file, int line, const char* function);
void Deep_Debug_Memory_Free(void* ptr, const char* file, int line, const char* function);
void Deep_Debug_Memory_PrintAllocations();
void Deep_Debug_Memory_Start();
void Deep_Debug_Memory_End();

#endif

#endif
