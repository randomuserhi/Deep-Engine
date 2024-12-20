#pragma once

namespace Deep {
    template<typename T>
    Ref<T>::Ref(T* ptr) :
        ptr(ptr) {
        Acquire();
    }

    template<typename T>
    Ref<T>::Ref(const Ref<T>& ref) :
        ptr(ref.ptr) {
        Acquire();
    }

    template<typename T>
    Ref<T>::Ref(Ref<T>&& ref) noexcept :
        ptr(ref.ptr) {
        ref.ptr = nullptr;
    }

    template<typename T>
    Ref<T>::~Ref() {
        Release();
    }

    template<typename T>
    Ref<T>& Ref<T>::operator=(T* ptr) {
        if (this->ptr != ptr) {
            Release();
            this->ptr = ptr;
            Acquire();
        }
        return *this;
    }

    template<typename T>
    Ref<T>& Ref<T>::operator=(const Ref<T>& ref) {
        if (ptr != ref.ptr) {
            Release();
            ptr = ref.ptr;
            Acquire();
        }
        return *this;
    }

    template<typename T>
    Ref<T>& Ref<T>::operator=(Ref<T>&& ref) noexcept {
        if (ptr != ref.ptr) {
            Release();
            ptr = ref.ptr;
            ref.ptr = nullptr;
        }
        return *this;
    }

    template<typename T>
    Ref<T>::operator T*() const {
        return ptr;
    }

    template<typename T>
    T* Ref<T>::operator->() const {
        Deep_Assert(ptr != nullptr, "JobHandle is nullptr.");
        return ptr;
    }

    template<typename T>
    T& Ref<T>::operator*() const {
        Deep_Assert(ptr != nullptr, "JobHandle is nullptr.");
        return *ptr;
    }

    template<typename T>
    bool Ref<T>::operator==(const T* b) const {
        return ptr == b;
    }
    template<typename T>
    Deep_Inline bool operator==(Ref<T>& a, Ref<T>& b) {
        return a.ptr == b.ptr;
    }
    template<typename T>
    bool Ref<T>::operator!=(const T* b) const {
        return !(*this == b);
    }
    template<typename T>
    Deep_Inline bool operator!=(Ref<T>& a, Ref<T>& b) {
        return !(a == b);
    }

    template<typename T>
    void Ref<T>::Acquire() {
        if (ptr != nullptr) {
            ptr->Acquire();
        }
    }

    template<typename T>
    void Ref<T>::Release() {
        if (ptr != nullptr) {
            ptr->Release();
        }
    }

    template<typename T>
    T* Ref<T>::GetPtr() const {
        return ptr;
    }
} // namespace Deep