#pragma once

#include "../../Deep.h"

namespace Deep {
    // Class for automatic referencing, calls `Acquire()` and `Release()` on the target class T
    // whenever a reference is acquired or released.
    //
    // NOTE(randomuserhi): class T must implement `Acquire` and `Release` methods or this class will not compile.
    //
    // Implementation based on Jolt: https://github.com/jrouwe/JoltPhysics/blob/master/Jolt/Core/Reference.h
    template<class T>
    class Ref {
    public:
        Deep_Inline Ref() = default;
        Deep_Inline Ref(T* ptr);
        Deep_Inline Ref(const Ref& ref);
        Deep_Inline Ref(Ref&& ref) noexcept;
        Deep_Inline ~Ref();

        // Assignment operators
        Deep_Inline Ref& operator=(T* ptr);
        Deep_Inline Ref& operator=(const Ref& ref);
        Deep_Inline Ref& operator=(Ref&& ref) noexcept;

        // Casting operators
        Deep_Inline operator T*() const;

        // Pointer access
        Deep_Inline T* operator->() const;
        Deep_Inline T& operator*() const;

        // Comparison
        Deep_Inline bool operator==(const T* b) const;
        friend bool operator==(Ref& a, Ref& b);
        Deep_Inline bool operator!=(const T* b) const;
        friend bool operator!=(Ref& a, Ref& b);

        Deep_Inline T* GetPtr() const;

    private:
        // NOTE(randomuserhi): Use `ref = nullptr` to release then object, do not call these functions.
        //                     These are used to acquire and release the underlying target of the ref.
        Deep_Inline void Acquire();
        Deep_Inline void Release();

    protected:
        T* ptr = nullptr;
    };
} // namespace Deep

#include "./Reference.inl"