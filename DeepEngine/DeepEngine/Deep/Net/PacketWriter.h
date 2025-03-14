/**
 * Packet
 */

#pragma once

#include <vector>
#include <Deep.h>
#include <Deep/Math.h>
#include <Deep/BitHelper.h>

namespace Deep {
    struct PacketWriter {
        PacketWriter() {}
        explicit PacketWriter(size_t size) {
            buffer.reserve(size);
        }

        // TODO(randomuserhi): https://stackoverflow.com/a/51615364/9642458
        Deep_Inline const uint8* data() const;
        Deep_Inline size_t size() const;

        Deep_Inline void Write(uint8 byte);
        Deep_Inline void Write(const uint8* bytes, size_t numBytes);
        Deep_Inline void Write(uint16 value);
        Deep_Inline void Write(int16 value);
        Deep_Inline void Write(uint32 value);
        Deep_Inline void Write(int32 value);
        Deep_Inline void Write(float32 value);
        Deep_Inline void Write(Vec3 value);
        void Write(Quat value);

        Deep_Inline void WriteHalf(float32 value);
        Deep_Inline void WriteHalf(Vec3 value);
        void WriteHalf(Quat value);

        Deep_Inline void Clear();

    private:
        std::vector<uint8> buffer;
    };
} // namespace Deep

#include "PacketWriter.inl"