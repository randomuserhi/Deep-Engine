/**
 * BitHelper
 */

#pragma once

#include "../Deep.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#endif

// Bitcast support for different Cpp versions
#if __cplusplus >= 202002L
#include <bit>
#elif defined(DEEP_COMPILER_MSVC)
#include <type_traits>
#endif
namespace Deep {
    #if __cplusplus >= 202002L

    template <class To, class From>
    constexpr Deep_Inline To BitCast(const From& value) {
        return std::bit_cast<To>(value);
    }

    #elif defined(DEEP_COMPILER_MSVC)

    template <class To, class From>
    constexpr Deep_Inline To BitCast(const From& value) {
        return std::_Bit_cast<To>(value);
    }

    #else

    // NOTE(randomuserhi): Can't be constexpr as the active member of union cannot be changed
    //                     in a constexpr
    template <class To, class From>
    Deep_Inline To BitCast(const From& value) {
        static_assert(std::is_trivially_constructible_v<To>, "Is supposed to be a trivial type!");
        static_assert(sizeof(From) == sizeof(To), "Invalid bit cast as types are not of same size.");

        union FromTo {
            To			mTo;
            From		mFrom;
        };

        FromTo convert;
        convert.mFrom = value;
        return convert.mTo;
    }

    #endif
}

namespace Deep {
    Deep_Inline bool IsBigEndian() {
        uint x = 1;
        uint8* c = reinterpret_cast<uint8*>(&x);
        return static_cast<uint8>(*c) == 0; // 0 if big endian, 1 if little endian
    }

    // Check if value is a power of 2
    template <typename T>
    constexpr bool IsPowerOf2(const T value) {
        return (value & (value - 1)) == 0;
    }

    // Check if the given pointer is aligned to the specified alignment
    template <typename T>
    inline bool IsAligned(const T pointer, const uint64 alignment) {
        static_assert(std::is_pointer<T>::value, "Expected type T to be a pointer.");
        Deep_Assert(IsPowerOf2(alignment), "Alignment should be a power of 2.");
        return (static_cast<uint64>(pointer) & (alignment - 1)) == 0;
    }

    // Compute number of trailing zero bits (how many low bits are zero)
    inline uint NumTrailingZeros(uint32 inValue) {
        #if defined(DEEP_CPU_X86)
        #if defined(DEEP_USE_TZCNT)
        return _tzcnt_u32(inValue);
        #elif defined(DEEP_COMPILER_MSVC)
        if (inValue == 0) {
            return 32;
        }
        unsigned long result;
        _BitScanForward(&result, inValue);
        return result;
        #else
        if (inValue == 0) {
            return 32;
        }
        return __builtin_ctz(inValue);
        #endif
        #else
        #error Unsupported Architecture
        #endif
    }

    Deep_Inline uint32 RotateLeft(const uint32 value, const int32 offset) {
        return (value << offset) | (value >> (32 - offset));
    }

    Deep_Inline uint32 RotateRight(const uint32 value, const int32 offset) {
        return (value >> offset) | (value << (32 - offset));
    }

    Deep_Inline uint16 ReverseEndianness(const uint16 value) {
        return static_cast<uint16>((value >> 8) + (value << 8));
    }

    Deep_Inline uint32 ReverseEndianness(const uint32 value) {
        return RotateRight(value & 0x00FF00FFu, 8) + RotateLeft(value & 0xFF00FF00u, 8);
    }

    Deep_Inline uint64 ReverseEndianness(const uint64 value) {
        return (static_cast<uint64>(ReverseEndianness(static_cast<uint32>(value))) << 32)
            + static_cast<uint64>(ReverseEndianness(static_cast<uint32>(value >> 32)));
    }

    Deep_Inline int16 ReverseEndianness(const int16 value) {
        return static_cast<int16>(ReverseEndianness(static_cast<uint16>(value)));
    }

    Deep_Inline int32 ReverseEndianness(const int32 value) {
        return static_cast<int32>(ReverseEndianness(static_cast<uint32>(value)));
    }

    Deep_Inline int64 ReverseEndianness(const int64 value) {
        return static_cast<int64>(ReverseEndianness(static_cast<uint64>(value)));
    }

    Deep_Inline uint32 AsUInt(const float32 x) {
        return Deep::BitCast<const uint32>(x);
    }
    Deep_Inline float32 AsFloat(const uint x) {
        return Deep::BitCast<const float32>(x);
    }

    Deep_Inline float32 HalfToFloat(const uint16 x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
        const uint32 e = (x & 0x7C00) >> 10; // exponent
        const uint32 m = (x & 0x03FF) << 13; // mantissa
        const uint32 v = AsUInt((float32)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
        return AsFloat((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
    }
    Deep_Inline float16 FloatToHalf(const float32 x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
        const uint32 b = AsUInt(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
        const uint32 e = (b & 0x7F800000) >> 23; // exponent
        const uint32 m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
        return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
    }

    // Host to Network conversion methods

    Deep_Inline uint16 hton(const uint16 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }

    Deep_Inline uint32 hton(const uint32 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }

    Deep_Inline uint64 hton(const uint64 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }

    Deep_Inline int16 hton(const int16 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }

    Deep_Inline int32 hton(const int32 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }

    Deep_Inline int64 hton(const int64 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }

    // Network to Host conversion methods

    Deep_Inline uint16 ntoh(const uint16 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }

    Deep_Inline uint32 ntoh(const uint32 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }

    Deep_Inline uint64 ntoh(const uint64 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }

    Deep_Inline int16 ntoh(const int16 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }

    Deep_Inline int32 ntoh(const int32 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }

    Deep_Inline int64 ntoh(const int64 value) {
        return !IsBigEndian() ? value : ReverseEndianness(value);
    }
}