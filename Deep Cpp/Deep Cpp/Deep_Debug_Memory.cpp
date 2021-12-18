#include "Deep_Debug_Memory.h"

#if defined(DEEP_DEBUG_MEMORY)

void operator delete (void* ptr)
{
    std::cout << "ptr = " << ptr << '\n';
    free(ptr);
}

#endif
