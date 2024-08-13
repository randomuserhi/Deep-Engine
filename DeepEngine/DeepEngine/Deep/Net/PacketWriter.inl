#pragma once

namespace Deep {
    const uint8* PacketWriter::data() const {
        return buffer.data();
    }
    size_t PacketWriter::size() const {
        return buffer.size();
    }

    // TODO(randomuserhi): Important read, 
    //                     https://stackoverflow.com/questions/3022552/is-there-any-standard-htonl-like-function-for-64-bits-integers-in-c

    // TODO(randomuserhi): My netcode uses little-endian view across network as most architectures are little-endian.
    //                     Therefore I need my own implementation of htons and htonl for converting big-endian hosts to 
    //                     little endian.

    void PacketWriter::Write(uint8 byte) {
        buffer.push_back(byte);
    }

    // NOTE(randomuserhi): This writes raw bytes to the buffer and does not account
    //                     for endianess. For ASCII strings or formats where data only
    //                     spans 1 byte, this works fine:
    //                     https://stackoverflow.com/questions/63974853/explaining-lack-of-endianness-as-it-applies-to-a-string
    void PacketWriter::Write(const uint8* bytes, size_t numBytes) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + numBytes);
        memcpy(buffer.data() + old, bytes, numBytes);
    }

    void PacketWriter::Write(uint16 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<uint16*>(buffer.data() + old) = hton(value);
    }
    void PacketWriter::Write(int16 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int16*>(buffer.data() + old) = hton(value);
    }

    void PacketWriter::Write(uint32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<uint32*>(buffer.data() + old) = hton(value);
    }
    void PacketWriter::Write(int32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int32*>(buffer.data() + old) = hton(value);
    }

    void PacketWriter::Write(float32 value) {
        size_t old = buffer.size();
        buffer.resize(buffer.size() + sizeof value);
        *reinterpret_cast<int32*>(buffer.data() + old) = hton(*reinterpret_cast<uint32*>(&value));
    }
    void PacketWriter::WriteHalf(float32 value) {
        float16 half = FloatToHalf(value);
        Write(half);
    }

    void PacketWriter::Write(Vec3 value) {
        buffer.reserve(buffer.size() + sizeof(float32) * 3);
        Write(value.x);
        Write(value.y);
        Write(value.z);
    }
    void PacketWriter::WriteHalf(Vec3 value) {
        buffer.reserve(buffer.size() + sizeof(float32) * 3);
        WriteHalf(value.x);
        WriteHalf(value.y);
        WriteHalf(value.z);
    }
}