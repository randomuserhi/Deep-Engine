#include <winsock2.h>

#include "./Packet.h"
#include "./BitHelper.h"
#include "../Math.h"

namespace Deep {
    // TODO(randomuserhi): Important read, 
    //                     https://stackoverflow.com/questions/3022552/is-there-any-standard-htonl-like-function-for-64-bits-integers-in-c

    // TODO(randomuserhi): My netcode uses little-endian view across network as most architectures are little-endian.
    //                     Therefore I need my own implementation of htons and htonl for converting big-endian hosts to 
    //                     little endian.

    void Packet::Write(uint8 byte) {
        buffer.push_back(byte);
    }

    // NOTE(randomuserhi): This writes raw bytes to the buffer and does not account
    //                     for endianess. For ASCII strings or formats where data only
    //                     spans 1 byte, this works fine:
    //                     https://stackoverflow.com/questions/63974853/explaining-lack-of-endianness-as-it-applies-to-a-string
    void Packet::Write(const uint8* bytes, size_t numBytes) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + numBytes);
        // Fairly sure this is UB to the abstract Cpp machine, since no uint8 objects
        // are constructed and besides, we don't construct them as array elements
        // so adjacent access is still UB.
        // 
        // Reasonable compilers shouldn't care, but by the abstract cpp engine this is UB
        // 
        // NOTE(randomuserhi): Use of memcpy means there are portability issues for systems where length of byte
        //                     >8 bits, refer to Deep_Types.h
        memcpy(buffer.data() + old, bytes, numBytes);
    }

    // Fairly sure the below reinterpret_cast's are UB to the abstract Cpp machine, 
    // since we are constructing an int object instead of an array of uint8 objects, 
    // so access to the uint8 objects via the vector is UB.
    // 
    // Reasonable compilers shouldn't care, but by the cpp standard this is UB

    void Packet::Write(uint16 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<uint16*>(buffer.data() + old) = hton(value);
    }
    void Packet::Write(int16 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int16*>(buffer.data() + old) = hton(value);
    }

    // Fairly sure the below reinterpret_cast's are UB to the abstract Cpp machine, 
    // since we are constructing an int object instead of an array of uint8 objects, 
    // so access to the uint8 objects via the vector is UB.
    // 
    // Reasonable compilers shouldn't care, but by the cpp standard this is UB

    void Packet::Write(uint32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<uint32*>(buffer.data() + old) = hton(value);
    }
    void Packet::Write(int32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int32*>(buffer.data() + old) = hton(value);
    }

    void Packet::Write(float32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int32*>(buffer.data() + old) = hton(*reinterpret_cast<uint32*>(&value));
    }
    void Packet::WriteHalf(float32 value) {
        float16 half = FloatToHalf(value);
        Write(half);
    }

    void Packet::Write(Vec3 value) {
        buffer.reserve(buffer.size() + sizeof(float32) * 3);
        Write(value.x);
        Write(value.y);
        Write(value.z);
    }
    void Packet::WriteHalf(Vec3 value) {
        buffer.reserve(buffer.size() + sizeof(float32) * 3);
        WriteHalf(value.x);
        WriteHalf(value.y);
        WriteHalf(value.z);
    }

    void Packet::Write(Quaternion value) {
        buffer.reserve(buffer.size() + sizeof(uint8) + sizeof(float32) * 3);

        float32 largest = value.x;
        uint8 i = 0;
        if (value.y > largest) {
            largest = value.y;
            i = 1;
        }
        if (value.z > largest) {
            largest = value.z;
            i = 2;
        }
        if (value.w > largest) {
            largest = value.w;
            i = 3;
        }

        Write(i);
        switch (i) {
        case 0:
            if (value.x >= 0) {
                Write(value.y);
                Write(value.z);
                Write(value.w);
            } else {
                Write(-value.y);
                Write(-value.z);
                Write(-value.w);
            }
            break;
        case 1:
            if (value.y >= 0) {
                Write(value.x);
                Write(value.z);
                Write(value.w);
            } else {
                Write(-value.x);
                Write(-value.z);
                Write(-value.w);
            }
            break;
        case 2:
            if (value.z >= 0) {
                Write(value.x);
                Write(value.y);
                Write(value.w);
            } else {
                Write(-value.x);
                Write(-value.y);
                Write(-value.w);
            }
            break;
        case 3:
            if (value.w >= 0) {
                Write(value.x);
                Write(value.y);
                Write(value.z);
            } else {
                Write(-value.x);
                Write(-value.y);
                Write(-value.z);
            }
            break;
        }
    }
    void Packet::WriteHalf(Quaternion value) {
        buffer.reserve(buffer.size() + sizeof(uint8) + sizeof(float16) * 3);

        float32 largest = value.x;
        uint8 i = 0;
        if (value.y > largest) {
            largest = value.y;
            i = 1;
        }
        if (value.z > largest) {
            largest = value.z;
            i = 2;
        }
        if (value.w > largest) {
            largest = value.w;
            i = 3;
        }

        Write(i);
        switch (i) {
        case 0:
            if (value.x >= 0) {
                WriteHalf(value.y);
                WriteHalf(value.z);
                WriteHalf(value.w);
            } else {
                WriteHalf(-value.y);
                WriteHalf(-value.z);
                WriteHalf(-value.w);
            }
            break;
        case 1:
            if (value.y >= 0) {
                WriteHalf(value.x);
                WriteHalf(value.z);
                WriteHalf(value.w);
            } else {
                WriteHalf(-value.x);
                WriteHalf(-value.z);
                WriteHalf(-value.w);
            }
            break;
        case 2:
            if (value.z >= 0) {
                WriteHalf(value.x);
                WriteHalf(value.y);
                WriteHalf(value.w);
            } else {
                WriteHalf(-value.x);
                WriteHalf(-value.y);
                WriteHalf(-value.w);
            }
            break;
        case 3:
            if (value.w >= 0) {
                WriteHalf(value.x);
                WriteHalf(value.y);
                WriteHalf(value.z);
            } else {
                WriteHalf(-value.x);
                WriteHalf(-value.y);
                WriteHalf(-value.z);
            }
            break;
        }
    }
}