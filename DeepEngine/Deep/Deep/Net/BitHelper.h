/**
 * BitHelper
 */

#include "../../Deep.h"

namespace Deep {
    // NOTE(randomuserhi): I technically should pack and unpack floating point types
    //                     and encode integers to a known binary format as described by beej:
    //                     https://beej.us/guide/bgnet/examples/ieee754.c
    //                     https://www.beej.us/guide/bgnet/html/#:~:text=on%20for%20details!-,7.5%20Serialization%E2%80%94How%20to%20Pack%20Data,-It%E2%80%99s%20easy%20enough
    //                     - However most systems this should be fine (just handling endianness)

    Deep_Inline bool IsBigEndian() {
        uint x = 1;
        uint8* c = (uint8*)&x;
        return (uint8)*c == 0; // 0 if big endian, 1 if little endian
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
        return RotateRight(value & 0x00FF00FFu, 8)
            + RotateLeft(value & 0xFF00FF00u, 8);
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
        return *reinterpret_cast<const uint32*>(&x);
    }
    Deep_Inline float32 AsFloat(const uint x) {
        return *reinterpret_cast<const float32*>(&x);
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

    Deep_Inline uint16 hton(const uint16 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }

    Deep_Inline uint32 hton(const uint32 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }

    Deep_Inline uint64 hton(const uint64 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }

    Deep_Inline int16 hton(const int16 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }

    Deep_Inline int32 hton(const int32 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }

    Deep_Inline int64 hton(const int64 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }

    Deep_Inline uint16 ntoh(const uint16 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }

    Deep_Inline uint32 ntoh(const uint32 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }

    Deep_Inline uint64 ntoh(const uint64 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }

    Deep_Inline int16 ntoh(const int16 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }

    Deep_Inline int32 ntoh(const int32 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }

    Deep_Inline int64 ntoh(const int64 value) {
        if (!IsBigEndian()) return value;
        else return ReverseEndianness(value);
    }
}