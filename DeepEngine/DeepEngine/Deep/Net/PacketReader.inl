#pragma once

namespace Deep {
    uint8 PacketReader::ReadByte() {
        uint8 value = *head;
        head += sizeof value;
        return ntoh(value);
    }
    uint16 PacketReader::ReadUInt16() {
        uint16 value = *reinterpret_cast<const uint16*>(head);
        head += sizeof value;
        return ntoh(value);
    }
    uint32 PacketReader::ReadUInt32() {
        uint32 value = *reinterpret_cast<const uint32*>(head);
        head += sizeof value;
        return ntoh(value);
    }
    uint64 PacketReader::ReadUInt64() {
        uint64 value = *reinterpret_cast<const uint64*>(head);
        head += sizeof value;
        return ntoh(value);
    }

    int16 PacketReader::ReadInt16() {
        int16 value = *reinterpret_cast<const int16*>(head);
        head += sizeof value;
        return ntoh(value);
    }
    int32 PacketReader::ReadInt32() {
        int32 value = *reinterpret_cast<const int32*>(head);
        head += sizeof value;
        return ntoh(value);
    }
    int64 PacketReader::ReadInt64() {
        int64 value = *reinterpret_cast<const int64*>(head);
        head += sizeof value;
        return ntoh(value);
    }

    float32 PacketReader::ReadHalf() {
        uint16 value = *reinterpret_cast<const uint16*>(head);
        head += sizeof value;
        return HalfToFloat(ntoh(value));
    }
    float32 PacketReader::ReadFloat() {
        uint32 value = *reinterpret_cast<const uint32*>(head);
        head += sizeof value;
        return AsFloat(ntoh(value));
    }

    Vec3 PacketReader::ReadVec3() {
        return Vec3{ ReadFloat(), ReadFloat(), ReadFloat() };
    }
    Vec3 PacketReader::ReadHalfVec3() {
        return Vec3{ ReadHalf(), ReadHalf(), ReadHalf() };
    }
} // namespace Deep