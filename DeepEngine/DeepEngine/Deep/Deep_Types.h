/**
 * Deep Types
 */

#pragma once

#include <cstdint>

 /// Rules on size_t 
 /// - Try to avoid implicit type conversions (if a type returns size_t, use size_t indexing etc...)
 ///   - If one is necessary, make it explicit -> for (int32 i = static_cast<int32>(size)-1; i >= 0; --i)
 /// - Be aware of boundary conditions -> for (size_t i = size-1; i >= 0; --i) will never end due to unsigned nature of size_t
 /// - size_t is typically used for sizes, prefer int32 for indexing unless you wish to avoid an implicity type conversion from STL containers
 /// - optionally, prefer ptrdiff_t rather than int32 for requiring signed indexing for large values on 64 bit systems

 /// Explicit Width writing rules:
 /// - If performance is super tight and the alignment of memory isn't to bad you can use int_fastXX_t / uint_fastXX_t
 ///     - Very rare that this is the case though
 /// - If a built-in function uses unspecified int type / long type, implict/explicit casting to the built-in function is allowed
 ///     - Use of static_cast<> for explicit casting may be used
 ///     - Compilers will throw warnings for mismatch in types if a given system has one
 /// - Otherwise if something requires X bits use intX/uintX
 /// 
 /// By this guideline we prefer:
 /// - int8/uint8 over char type
 /// - int16/uint16 over short type
 /// - int32/uint32 over int type
 /// - int64/uint64 over longlong type
 ///
 /// 
 /// Notes on "Byte":
 /// - When on a 32/64 bit system it is safe to assume a byte is 8 bits and thus terminology like numBytes is synonymous with number
 ///   of int8_t/uint8_t values and it is safe for byte dependent operations
 /// - If not, then it is important to take care of using int8/uint8 interchangeably with a byte/char/uchar types and byte dependent
 ///   operations.
 ///   - E.g using memcpy to copy a buffer of bytes into a network buffer:
 ///         In this scenario, if the host byte is >8 bits in length then memcopying the bytes directly into the network buffer
 ///         leaves padding which is incorrect. You would have to write into the buffer manually to eliminate such padding
 ///         and ensure a buffer of uint8_t for networking.

 /// Loose definitions that follow standard
 /// https://en.cppreference.com/w/cpp/language/types#:~:text=the%20same%20type.-,Properties,-The%20following%20table
 /// 
 /// Note for int type on 32/64 bit systems it is safe to assume that it is atleast 32 bits
 /// https://en.cppreference.com/w/cpp/language/types#:~:text=Integer%20types-,Standard%20integer%20types,-int%20%E2%80%94%20basic%20integer
 ///
 /// For long, it is always atleast 32 bits. If you wish to guarantee atleast 64 bits, use long long
 /// https://en.cppreference.com/w/cpp/language/types#:~:text=target%20type%20will%20have%20width%20of%20at%20least%2064%20bits

using longlong = long long;

using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;
using ulonglong = unsigned long long;

using half = unsigned short;

/// strict definitions => short hand for intXX_t
/// Like above the type is atleast XX number of bits large
///
/// int_fastXX_t can be used for loose definitions.

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

/// Floating point explicit width definitions
/// 
/// Guarantees:
/// - float16 is exactly 16 bits -> NOTE(randomuserhi) Interpretation is not built-in due to most systems not supporting 16 bit floats
/// - float/float32 is exactly 32 bits as per IEEE-754 binary32 format
/// - double/float64 is exactly 64 bits as per IEEE-754 binary64 format
/// - long double/float128 is exactly 128 bits as per IEEE-754 binary128 format
/// https://en.cppreference.com/w/cpp/language/types#:~:text=for%20every%20type.-,Floating%2Dpoint%20types,-Standard%20floating%2Dpoint

using float16 = uint16_t;

using float32 = float;
using float64 = double;
using float128 = long double;