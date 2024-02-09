/**
 * Packet
 */

#pragma once

#include <vector>
#include "./Deep.h"
#include "../Math.h"
#include "./BitHelper.h"

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
        Quaternion ReadQuaternion();

        Vec3 ReadHalfVec3();
        Quaternion ReadHalfQuaternion();

        const uint8* head;

    private:
        const uint8* const data;
    };

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
    Deep_Inline Vec3 PacketReader::ReadHalfVec3() {
        return Vec3{
            ReadHalf(),
            ReadHalf(),
            ReadHalf()
        };
    }
}