/**
 * Packet
 */

#include <winsock2.h>
#include <cmath>

#include "./PacketReader.h"
#include "../Math.h"

namespace Deep {
    Quaternion PacketReader::ReadQuaternion() {
        uint8 i = ReadByte();
        Quaternion q{};
        switch (i) {
        case 0:
            q.y = ReadFloat();
            q.z = ReadFloat();
            q.w = ReadFloat();
            q.x = std::sqrtf(1.0f - q.y * q.y - q.z * q.z - q.w * q.w);
            break;
        case 1:
            q.x = ReadFloat();
            q.z = ReadFloat();
            q.w = ReadFloat();
            q.y = std::sqrtf(1.0f - q.x * q.x - q.z * q.z - q.w * q.w);
            break;
        case 2:
            q.x = ReadFloat();
            q.y = ReadFloat();
            q.w = ReadFloat();
            q.z = std::sqrtf(1.0f - q.x * q.x - q.y * q.y - q.w * q.w);
            break;
        case 3:
            q.x = ReadFloat();
            q.y = ReadFloat();
            q.z = ReadFloat();
            q.w = std::sqrtf(1.0f - q.x * q.x - q.y * q.y - q.z * q.z);
            break;
        }
        return q;
    }

    Quaternion PacketReader::ReadHalfQuaternion() {
        uint8 i = ReadByte();
        Quaternion q{};
        switch (i) {
        case 0:
            q.y = ReadHalf();
            q.z = ReadHalf();
            q.w = ReadHalf();
            q.x = std::sqrtf(1.0f - q.y * q.y - q.z * q.z - q.w * q.w);
            break;
        case 1:
            q.x = ReadHalf();
            q.z = ReadHalf();
            q.w = ReadHalf();
            q.y = std::sqrtf(1.0f - q.x * q.x - q.z * q.z - q.w * q.w);
            break;
        case 2:
            q.x = ReadHalf();
            q.y = ReadHalf();
            q.w = ReadHalf();
            q.z = std::sqrtf(1.0f - q.x * q.x - q.y * q.y - q.w * q.w);
            break;
        case 3:
            q.x = ReadHalf();
            q.y = ReadHalf();
            q.z = ReadHalf();
            q.w = std::sqrtf(1.0f - q.x * q.x - q.y * q.y - q.z * q.z);
            break;
        }
        return q;
    }
}