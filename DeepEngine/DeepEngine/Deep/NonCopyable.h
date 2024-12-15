#pragma once

namespace Deep {
    // Inheriting this class makes the parent non-copyable
    //
    // Implementation based on Jolt: https://github.com/jrouwe/JoltPhysics/blob/master/Jolt/Core/NonCopyable.h
    class NonCopyable {
    public:
        NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
        void operator=(const NonCopyable&) = delete;
    };
} // namespace Deep