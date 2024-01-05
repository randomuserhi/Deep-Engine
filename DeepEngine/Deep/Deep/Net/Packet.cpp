#include <winsock2.h>
#include <cmath>

#include "./Packet.h"
#include "./BitHelper.h"
#include "../Math.h"

namespace Deep {
    Deep_Inline uint8 PacketReader::ReadByte() {
        uint8 value = *head;
        head += sizeof value;
        return ntoh(value);
    }
    Deep_Inline uint16 PacketReader::ReadUInt16() {
        uint16 value = *reinterpret_cast<const uint16*>(head);
        head += sizeof value;
        return ntoh(value);
    }
    Deep_Inline uint32 PacketReader::ReadUInt32() {
        uint32 value = *reinterpret_cast<const uint32*>(head);
        head += sizeof value;
        return ntoh(value);
    }
    Deep_Inline uint64 PacketReader::ReadUInt64() {
        uint64 value = *reinterpret_cast<const uint64*>(head);
        head += sizeof value;
        return ntoh(value);
    }

    Deep_Inline int16 PacketReader::ReadInt16() {
        int16 value = *reinterpret_cast<const int16*>(head);
        head += sizeof value;
        return ntoh(value);
    }
    Deep_Inline int32 PacketReader::ReadInt32() {
        int32 value = *reinterpret_cast<const int32*>(head);
        head += sizeof value;
        return ntoh(value);
    }
    Deep_Inline int64 PacketReader::ReadInt64() {
        int64 value = *reinterpret_cast<const int64*>(head);
        head += sizeof value;
        return ntoh(value);
    }

    Deep_Inline float32 PacketReader::ReadHalf() {
        uint16 value = *reinterpret_cast<const uint16*>(head);
        head += sizeof value;
        return HalfToFloat(ntoh(value));
    }
    Deep_Inline float32 PacketReader::ReadFloat() {
        uint32 value = *reinterpret_cast<const uint32*>(head);
        head += sizeof value;
        return AsFloat(ntoh(value));
    }

    Deep_Inline Vec3 PacketReader::ReadVec3() {
        return Vec3{
            ReadFloat(),
            ReadFloat(),
            ReadFloat()
        };
    }
    Quaternion PacketReader::ReadQuaternion() {
        uint8 i = ReadByte();
        float x = 0, y = 0, z = 0, w = 0;
        switch (i) {
        case 0:
            y = ReadFloat();
            z = ReadFloat();
            w = ReadFloat();
            x = std::sqrtf(1.0f - y * y - z * z - w * w);
            break;
        case 1:
            x = ReadFloat();
            z = ReadFloat();
            w = ReadFloat();
            y = std::sqrtf(1.0f - x * x - z * z - w * w);
            break;
        case 2:
            x = ReadFloat();
            y = ReadFloat();
            w = ReadFloat();
            z = std::sqrtf(1.0f - x * x - y * y - w * w);
            break;
        case 3:
            x = ReadFloat();
            y = ReadFloat();
            z = ReadFloat();
            w = std::sqrtf(1.0f - x * x - y * y - z * z);
            break;
        }
    }

    Deep_Inline Vec3 PacketReader::ReadHalfVec3() {
        return Vec3{
            ReadHalf(),
            ReadHalf(),
            ReadHalf()
        };
    }
    Quaternion PacketReader::ReadHalfQuaternion() {
        uint8 i = ReadByte();
        float x = 0, y = 0, z = 0, w = 0;
        switch (i) {
        case 0:
            y = ReadHalf();
            z = ReadHalf();
            w = ReadHalf();
            x = std::sqrtf(1.0f - y * y - z * z - w * w);
            break;
        case 1:
            x = ReadHalf();
            z = ReadHalf();
            w = ReadHalf();
            y = std::sqrtf(1.0f - x * x - z * z - w * w);
            break;
        case 2:
            x = ReadHalf();
            y = ReadHalf();
            w = ReadHalf();
            z = std::sqrtf(1.0f - x * x - y * y - w * w);
            break;
        case 3:
            x = ReadHalf();
            y = ReadHalf();
            z = ReadHalf();
            w = std::sqrtf(1.0f - x * x - y * y - z * z);
            break;
        }
    }

    // TODO(randomuserhi): Important read, 
    //                     https://stackoverflow.com/questions/3022552/is-there-any-standard-htonl-like-function-for-64-bits-integers-in-c

    // TODO(randomuserhi): My netcode uses little-endian view across network as most architectures are little-endian.
    //                     Therefore I need my own implementation of htons and htonl for converting big-endian hosts to 
    //                     little endian.

    Deep_Inline void Packet::Write(uint8 byte) {
        buffer.push_back(byte);
    }

    // NOTE(randomuserhi): This writes raw bytes to the buffer and does not account
    //                     for endianess. For ASCII strings or formats where data only
    //                     spans 1 byte, this works fine:
    //                     https://stackoverflow.com/questions/63974853/explaining-lack-of-endianness-as-it-applies-to-a-string
    Deep_Inline void Packet::Write(const uint8* bytes, size_t numBytes) {
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

    Deep_Inline void Packet::Write(uint16 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<uint16*>(buffer.data() + old) = hton(value);
    }
    Deep_Inline void Packet::Write(int16 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int16*>(buffer.data() + old) = hton(value);
    }

    // Fairly sure the below reinterpret_cast's are UB to the abstract Cpp machine, 
    // since we are constructing an int object instead of an array of uint8 objects, 
    // so access to the uint8 objects via the vector is UB.
    // 
    // Reasonable compilers shouldn't care, but by the cpp standard this is UB

    Deep_Inline void Packet::Write(uint32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<uint32*>(buffer.data() + old) = hton(value);
    }
    Deep_Inline void Packet::Write(int32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int32*>(buffer.data() + old) = hton(value);
    }

    Deep_Inline void Packet::Write(float32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int32*>(buffer.data() + old) = hton(*reinterpret_cast<uint32*>(&value));
    }
    Deep_Inline void Packet::WriteHalf(float32 value) {
        float16 half = FloatToHalf(value);
        Write(half);
    }

    Deep_Inline void Packet::Write(Vec3 value) {
        buffer.reserve(buffer.size() + sizeof(float32) * 3);
        Write(value.x);
        Write(value.y);
        Write(value.z);
    }
    Deep_Inline void Packet::WriteHalf(Vec3 value) {
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