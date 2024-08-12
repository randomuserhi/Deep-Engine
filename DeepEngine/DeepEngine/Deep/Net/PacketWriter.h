/**
 * Packet
 */

#pragma once

#include <vector>
#include "../../Deep.h"
#include "../Math.h"
#include "../BitHelper.h"

namespace Deep {
    struct PacketWriter {
        PacketWriter() {
        }
        PacketWriter(size_t size) {
            buffer.reserve(size);
        }

        // TODO(randomuserhi): https://stackoverflow.com/a/51615364/9642458
        Deep_Inline const uint8* data() const;
        Deep_Inline size_t size() const;

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
    Deep_Inline const uint8* PacketWriter::data() const {
        return buffer.data();
    }
    Deep_Inline size_t PacketWriter::size() const {
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
        memcpy(buffer.data() + old, bytes, numBytes);
    }

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