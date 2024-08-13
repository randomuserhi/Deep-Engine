#pragma once

namespace Deep {
    bool operator==(const IPv4 lhs, const IPv4 rhs) {
        return lhs.a == rhs.a &&
            lhs.b == rhs.b &&
            lhs.c == rhs.c &&
            lhs.d == rhs.d &&
            lhs.port == rhs.port;
    }
}