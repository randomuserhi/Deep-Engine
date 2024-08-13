#pragma once

namespace Deep {
    // Inheriting this class makes the parent non-copyable 
    class NonCopyable {
    public:
        NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
        void operator = (const NonCopyable&) = delete;
    };
}