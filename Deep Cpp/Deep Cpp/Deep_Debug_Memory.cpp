#include <unordered_map>
#include <fstream>

#include "Deep_Debug_Memory.h"

#ifdef malloc
#undef malloc
#endif

#ifdef calloc
#undef calloc
#endif

#ifdef realloc
#undef realloc
#endif

#ifdef free
#undef free
#endif

#if defined(new)
#undef new
#endif

#if defined(delete)
#undef delete
#endif

#if defined(DEEP_DEBUG_MEMORY)

namespace Deep
{
    bool DEBUG_MEMORY__SET__ = false;
    const char* DEBUG_MEMORY__FILE__ = "NULL";
    const char* DEBUG_MEMORY__FUNCTION__ = "NULL";
    int DEBUG_MEMORY__LINE__ = 0;

    bool DEBUG_MEMORY = false;
    std::ofstream DMFStream{ DEEP_DEBUG_MEMORY_LOGFILE };

    void StartMemoryDebug()
    {
        DEBUG_MEMORY = true;
    }

    void EndMemoryDebug()
    {
        DMFStream.close();
        DEBUG_MEMORY = false;
    }

    size_t DEBUG_MEMORY_FREECOUNT = 0;
    size_t DEBUG_MEMORY_MALLOCCOUNT = 0;
    size_t DEBUG_MEMORY_NEWCOUNT = 0;
    size_t DEBUG_MEMORY_DELETECOUNT = 0;

    struct DebugPtrInfo
    {
        const char* file;
        int line;
        const char* function;
    };

    std::unordered_map<void*, DebugPtrInfo> allocationMap;

    void PrintAllocationMap()
    {
        if (DEBUG_MEMORY)
        {
            std::cout << "\nRemaining pointers: [" << allocationMap.size() << "] { ";
            DMFStream << "\nRemaining pointers: [" << allocationMap.size() << "] { ";

            const char* seperator = "\n\t";
            for (auto& it : allocationMap)
            {
                std::cout << seperator << "[ " << it.second.file << " > " << it.second.function << " : line(" << it.second.line << ") { " << it.first << " } ]\n";
                DMFStream << seperator << "[ " << it.second.file << " > " << it.second.function << " : line(" << it.second.line << ") { " << it.first << " } ]\n";
                seperator = "\t";
            }

            std::cout << "}\n";
            DMFStream << "}\n";
        }
        else
        {
            std::cout << "(DEEP_DEBUG_MEMORY) Debugger is not running, use Deep::StartMemoryDebug().\n";
            DMFStream << "(DEEP_DEBUG_MEMORY) Debugger is not running, use Deep::StartMemoryDebug().\n";
        }
    }

    void* Malloc(size_t size, const char* file, int line, const char* function)
    {
        void* ptr;
        if ((ptr = (void*)malloc(size)))
        {
            if (Deep::DEBUG_MEMORY)
            {
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
                std::cout << "malloc => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";
#endif
                DMFStream << "malloc => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";

                allocationMap[ptr].file = file;
                allocationMap[ptr].line = line;
                allocationMap[ptr].function = function;
            }
            return ptr;
        }
        else
        {
            if (Deep::DEBUG_MEMORY)
            {
                std::cout << "(DEEP_DEBUG_MEMORY) Warning, malloc failed to get memory => " << file << " > " << function << " : line(" << line << ")\n";
                DMFStream << "(DEEP_DEBUG_MEMORY) Warning, malloc failed to get memory => " << file << " > " << function << " : line(" << line << ")\n";
            }
            return nullptr;
        }

    }
    void* Calloc(size_t count, size_t typeSize, const char* file, int line, const char* function)
    {
        void* ptr;
        if ((ptr = (void*)calloc(count, typeSize)))
        {
            if (Deep::DEBUG_MEMORY)
            {
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
                std::cout << "calloc => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";
#endif
                DMFStream << "calloc => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";

                allocationMap[ptr].file = file;
                allocationMap[ptr].line = line;
                allocationMap[ptr].function = function;
            }
            return ptr;
        }
        else
        {
            if (Deep::DEBUG_MEMORY)
            {
                std::cout << "(DEEP_DEBUG_MEMORY) Warning, calloc failed to get memory => " << file << " > " << function << " : line(" << line << ")\n";
                DMFStream << "(DEEP_DEBUG_MEMORY) Warning, calloc failed to get memory => " << file << " > " << function << " : line(" << line << ")\n";
            }
            return nullptr;
        }
    }
    void* Realloc(void* ptr, size_t size, const char* file, int line, const char* function)
    {
        void* newPtr;
        if ((newPtr = (void*)realloc(ptr, size)))
        {
            if (Deep::DEBUG_MEMORY)
            {
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
                std::cout << "realloc => " << file << " > " << function << " : line(" << line << ") { .old = " << (ptr ? ptr : "nullptr") << ", .new = " << newPtr << "[" << size << "] }\n";
#endif
                DMFStream << "realloc => " << file << " > " << function << " : line(" << line << ") { .old = " << (ptr ? ptr : "nullptr") << ", .new = " << newPtr << "[" << size << "] }\n";

                allocationMap.erase(ptr);

                allocationMap[newPtr].file = file;
                allocationMap[newPtr].line = line;
                allocationMap[newPtr].function = function;
            }
            return newPtr;
        }
        else
        {
            if (Deep::DEBUG_MEMORY)
            {
                std::cout << "(DEEP_DEBUG_MEMORY) Warning, realloc returned NULL ptr => " << file << " > " << function << " : line(" << line << ")\n";
                DMFStream << "(DEEP_DEBUG_MEMORY) Warning, realloc returned NULL ptr => " << file << " > " << function << " : line(" << line << ")\n";
            }
            return nullptr;
        }
    }
    void Free(void* ptr, const char* file, int line, const char* function)
    {
        if (Deep::DEBUG_MEMORY)
        {
            if (!ptr)
            {
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
                std::cout << "free => " << file << " > " << function << " : line(" << line << ") { nullptr }\n";
#endif
                DMFStream << "free => " << file << " > " << function << " : line(" << line << ") { nullptr }\n";
            }
            else if (allocationMap.find(ptr) != allocationMap.end())
            {
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
                std::cout << "free => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";
#endif
                DMFStream << "free => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";
                allocationMap.erase(ptr);
            }
            else
            {
                std::cout << "(DEEP_DEBUG_MEMORY) Warning, invalid free => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";
                DMFStream << "free => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";
            }
        }
        
        free(ptr);
    }
}


#if defined(Deep_Compiler_MSCV)
_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
#endif
void* __cdecl operator new (size_t size)
{
    void* ptr;
    if ((ptr = (void*)malloc(size)) == nullptr)
    {
        printf("(DEEP_DEBUG_MEMORY) Fatal, ::operator new failed to get memory.\n");
        exit(1);
    }
    return ptr;
}

#if defined(Deep_Compiler_MSCV)
_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
#endif
void* __cdecl operator new[](size_t size)
{
    void* ptr;
    if ((ptr = (void*)malloc(size)) == nullptr)
    {
        printf("(DEEP_DEBUG_MEMORY) Fatal, ::operator new[] failed to get memory.\n");
        exit(1);
    }
    return ptr;
}

void* operator new (size_t size, const char* file, int line, const char* function)
{
    void* ptr;
    if ((ptr = (void*)malloc(size)) == nullptr)
    {
        if (Deep::DEBUG_MEMORY)
        {
            std::cout << "(DEEP_DEBUG_MEMORY) Warning, ::operator new failed to get memory => " << file << " > " << function << " : line(" << line << ")\n";
            Deep::DMFStream << "(DEEP_DEBUG_MEMORY) Warning, ::operator new failed to get memory => " << file << " > " << function << " : line(" << line << ")\n";
        }
        return nullptr;
    }
    if (Deep::DEBUG_MEMORY)
    {
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
        std::cout << "::operator new => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";
#endif
        Deep::DMFStream << "::operator new => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";
        Deep::allocationMap[ptr].file = file;
        Deep::allocationMap[ptr].line = line;
        Deep::allocationMap[ptr].function = function;
    }
    return ptr;
}

void* operator new[] (size_t size, const char* file, int line, const char* function)
{
    void* ptr;
    if ((ptr = (void*)malloc(size)) == nullptr)
    {
        if (Deep::DEBUG_MEMORY)
        {
            std::cout << "(DEEP_DEBUG_MEMORY) Warning, ::operator new[] failed to get memory => " << file << " > " << function << " : line(" << line << ")\n";
            Deep::DMFStream << "(DEEP_DEBUG_MEMORY) Warning, ::operator new[] failed to get memory => " << file << " > " << function << " : line(" << line << ")\n";
        }
        return nullptr;
    }
    if (Deep::DEBUG_MEMORY)
    {
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
        std::cout << "::operator new[] => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";
#endif
        Deep::DMFStream << "::operator new[] => " << file << " > " << function << " : line(" << line << ") { " << ptr << " }\n";
        Deep::allocationMap[ptr].file = file;
        Deep::allocationMap[ptr].line = line;
        Deep::allocationMap[ptr].function = function;
    }
    return ptr;
}

void operator delete (void* ptr) throw()
{
    if (Deep::DEBUG_MEMORY)
    {
        //If DEBUG_MEMORY__SET__ is not true, then delete operator was called by something not managed
        if (Deep::DEBUG_MEMORY__SET__)
        {
            Deep::DEBUG_MEMORY__SET__ = false;
            if (!ptr)
            {
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
                std::cout << "::operator delete => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { nullptr }\n";
#endif
                Deep::DMFStream << "::operator delete => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { nullptr }\n";
            }
            else if (Deep::allocationMap.find(ptr) != Deep::allocationMap.end())
            {
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
                std::cout << "::operator delete => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { " << ptr << " }\n";
#endif
                Deep::DMFStream << "::operator delete => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { " << ptr << " }\n";
                Deep::allocationMap.erase(ptr);
            }
            else
            {
                std::cout << "(DEEP_DEBUG_MEMORY) Warning, invalid ::operator delete => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { " << ptr << " }\n";
                Deep::DMFStream << "(DEEP_DEBUG_MEMORY) Warning, invalid ::operator delete => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { " << ptr << " }\n";
            }
        }
    }
    free(ptr);
}

void operator delete[] (void* ptr) throw ()
{
    if (Deep::DEBUG_MEMORY)
    {
        //If DEBUG_MEMORY__SET__ is not true, then delete operator was called by something not managed
        if (Deep::DEBUG_MEMORY__SET__)
        {
            Deep::DEBUG_MEMORY__SET__ = false;
            if (!ptr)
            {
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
                std::cout << "::operator delete[] => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { nullptr }\n";
#endif
                Deep::DMFStream << "::operator delete[] => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { nullptr }\n";
            }
            else if (Deep::allocationMap.find(ptr) != Deep::allocationMap.end())
            {
#ifdef DEEP_DEBUG_MEMORY_VERBOSE
                std::cout << "::operator delete[] => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { " << ptr << " }\n";
#endif
                Deep::DMFStream << "::operator delete[] => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { " << ptr << " }\n";
                Deep::allocationMap.erase(ptr);
            }
            else
            {
                std::cout << "(DEEP_DEBUG_MEMORY) Warning, invalid ::operator delete[] => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { " << ptr << " }\n";
                Deep::DMFStream << "(DEEP_DEBUG_MEMORY) Warning, invalid ::operator delete[] => " << Deep::DEBUG_MEMORY__FILE__ << " > " << Deep::DEBUG_MEMORY__FUNCTION__ << " : line(" << Deep::DEBUG_MEMORY__LINE__ << ") { " << ptr << " }\n";
            }
        }
    }
    free(ptr);
}

#endif
