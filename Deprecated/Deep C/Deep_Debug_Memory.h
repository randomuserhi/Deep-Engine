#ifndef h_Deep_Debug_Memory
#define h_Deep_Debug_Memory

#include "Deep.h"

// https://stackoverflow.com/a/9074274/9642458
#if defined(DEEP_DEBUG_MEMORY)

#define malloc(size) Deep_Debug_Memory_Malloc(size, Deep__File__, Deep__Line__, Deep__Function__)
#define calloc(count, typeSize) Deep_Debug_Memory_Calloc(count, typeSize, Deep__File__, Deep__Line__, Deep__Function__)
#define realloc(ptr, size) Deep_Debug_Memory_Realloc(ptr, size, Deep__File__, Deep__Line__, Deep__Function__)
#define free(ptr) Deep_Debug_Memory_Free(ptr, Deep__File__, Deep__Line__, Deep__Function__)

void* Deep_Debug_Memory_Malloc(size_t size, const char* file, int line, const char* function);
void* Deep_Debug_Memory_Calloc(size_t count, size_t typeSize, const char* file, int line, const char* function);
void* Deep_Debug_Memory_Realloc(void* ptr, size_t size, const char* file, int line, const char* function);
void Deep_Debug_Memory_Free(void* ptr, const char* file, int line, const char* function);
void Deep_Debug_Memory_PrintAllocations();
void Deep_Debug_Memory_Start();
void Deep_Debug_Memory_End();

#endif

#endif
