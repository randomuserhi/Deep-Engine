#pragma once

#include "Deep.h"

#if defined(DEEP_DEBUG_MEMORY)

Deep_Inline void* operator new (size_t size, const char* filename, int line) 
{
    void* ptr = operator new(size);
    std::cout << "ptr = " << ptr << " size = " << size << " filename = " << filename << " line = " << line << '\n';
    return ptr;
}

void operator delete (void* ptr);

#define new new(Deep__File__, Deep__Line__)

#endif
