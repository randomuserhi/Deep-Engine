/**
 * IPv4
 */

#pragma once

#include <Deep.h>

namespace Deep {
    struct IPv4 {
        IPv4() :
            a(0), b(0), c(0), d(0), port(0) {}

        IPv4(uint8 a, uint8 b, uint8 c, uint8 d, uint16 port) :
            a(a), b(b), c(c), d(d), port(port) {}

        Deep_Inline friend bool operator==(const IPv4 lhs, const IPv4 rhs);

    public:
        static const IPv4 any;

        uint8 a;
        uint8 b;
        uint8 c;
        uint8 d;
        uint16 port;
    };
} // namespace Deep

#include "IPv4.inl"