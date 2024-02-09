/**
 * Packet
 */

#pragma once

#include <vector>
#include "./Deep.h"
#include "../Math.h"
#include "./BitHelper.h"

namespace Deep {
    struct PacketWriter {
        PacketWriter() {
        }
        PacketWriter(size_t size) {
            buffer.reserve(size);
        }

        // TODO(randomuserhi): https://stackoverflow.com/a/51615364/9642458
        Deep_Inline const uint8* data();
        Deep_Inline size_t size();

        void Write(uint8 byte);
        void Write(const uint8* bytes, size_t numBytes);
        void Write(uint16 value);
        void Write(int16 value);
        void Write(uint32 value);
        void Write(int32 value);
        void Write(float32 value);
        void Write(Vec3 value);
        void Write(Quaternion value);

        void WriteHalf(float32 value);
        void WriteHalf(Vec3 value);
        void WriteHalf(Quaternion value);

    private:
        std::vector<uint8> buffer;
    };
    Deep_Inline const uint8* PacketWriter::data() {
        return buffer.data();
    }
    Deep_Inline size_t PacketWriter::size() {
        return buffer.size();
    }

    // TODO(randomuserhi): Important read, 
    //                     https://stackoverflow.com/questions/3022552/is-there-any-standard-htonl-like-function-for-64-bits-integers-in-c

    // TODO(randomuserhi): My netcode uses little-endian view across network as most architectures are little-endian.
    //                     Therefore I need my own implementation of htons and htonl for converting big-endian hosts to 
    //                     little endian.

    Deep_Inline void PacketWriter::Write(uint8 byte) {
        buffer.push_back(byte);
    }

    // NOTE(randomuserhi): This writes raw bytes to the buffer and does not account
    //                     for endianess. For ASCII strings or formats where data only
    //                     spans 1 byte, this works fine:
    //                     https://stackoverflow.com/questions/63974853/explaining-lack-of-endianness-as-it-applies-to-a-string
    Deep_Inline void PacketWriter::Write(const uint8* bytes, size_t numBytes) {
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

    Deep_Inline void PacketWriter::Write(uint16 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<uint16*>(buffer.data() + old) = hton(value);
    }
    Deep_Inline void PacketWriter::Write(int16 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int16*>(buffer.data() + old) = hton(value);
    }

    // Fairly sure the below reinterpret_cast's are UB to the abstract Cpp machine, 
    // since we are constructing an int object instead of an array of uint8 objects, 
    // so access to the uint8 objects via the vector is UB.
    // 
    // Reasonable compilers shouldn't care, but by the cpp standard this is UB

    Deep_Inline void PacketWriter::Write(uint32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<uint32*>(buffer.data() + old) = hton(value);
    }
    Deep_Inline void PacketWriter::Write(int32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int32*>(buffer.data() + old) = hton(value);
    }

    Deep_Inline void PacketWriter::Write(float32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int32*>(buffer.data() + old) = hton(*reinterpret_cast<uint32*>(&value));
    }
    Deep_Inline void PacketWriter::WriteHalf(float32 value) {
        float16 half = FloatToHalf(value);
        Write(half);
    }

    Deep_Inline void PacketWriter::Write(Vec3 value) {
        buffer.reserve(buffer.size() + sizeof(float32) * 3);
        Write(value.x);
        Write(value.y);
        Write(value.z);
    }
    Deep_Inline void PacketWriter::WriteHalf(Vec3 value) {
        buffer.reserve(buffer.size() + sizeof(float32) * 3);
        WriteHalf(value.x);
        WriteHalf(value.y);
        WriteHalf(value.z);
    }
}