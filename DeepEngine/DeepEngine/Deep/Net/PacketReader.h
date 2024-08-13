/**
 * Packet
 */

#pragma once

#include <vector>
#include "../../Deep.h"
#include "../Math.h"
#include "../BitHelper.h"

namespace Deep {
    struct PacketReader {
        PacketReader(const uint8* data) : data(data), head(data) {
        }

        Deep_Inline uint8 ReadByte();

        Deep_Inline uint16 ReadUInt16();
        Deep_Inline uint32 ReadUInt32();
        Deep_Inline uint64 ReadUInt64();

        Deep_Inline int16 ReadInt16();
        Deep_Inline int32 ReadInt32();
        Deep_Inline int64 ReadInt64();

        Deep_Inline float32 ReadHalf();
        Deep_Inline float32 ReadFloat();

        Deep_Inline Vec3 ReadVec3();
        Quat ReadQuaternion();

        Deep_Inline Vec3 ReadHalfVec3();
        Quat ReadHalfQuaternion();

        const uint8* head;

    private:
        const uint8* const data;
    };
}

#include "./PacketReader.inl"