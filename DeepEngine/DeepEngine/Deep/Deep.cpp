#include "../Deep.h"

#include <iostream>

namespace Deep {
    bool OnAssertFailImpl(const char* expression, const char* file, uint32 line, const char* message) {
        std::cout << file << ":" << line << " Assertion Failed (" << expression << ") " << (message != nullptr ? message : "") << std::endl;

        return true;
    }

    bool (*OnAssertFail) (const char* expression, const char* file, uint32 line, const char* message) = &OnAssertFailImpl;
}