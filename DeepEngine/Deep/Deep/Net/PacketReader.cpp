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
        float32 norm;
        switch (i) {
        case 0:
            q.y = ReadFloat();
            q.z = ReadFloat();
            q.w = ReadFloat();
            norm = 1.0f - q.y * q.y - q.z * q.z - q.w * q.w;
            q.x = std::sqrtf(norm > 1.0f ? 1.0f : norm < 0.0f ? 0.0f : norm);
            break;
        case 1:
            q.x = ReadFloat();
            q.z = ReadFloat();
            q.w = ReadFloat();
            norm = 1.0f - q.x * q.x - q.z * q.z - q.w * q.w;
            q.y = std::sqrtf(norm > 1.0f ? 1.0f : norm < 0.0f ? 0.0f : norm);
            break;
        case 2:
            q.x = ReadFloat();
            q.y = ReadFloat();
            q.w = ReadFloat();
            norm = 1.0f - q.x * q.x - q.y * q.y - q.w * q.w;
            q.z = std::sqrtf(norm > 1.0f ? 1.0f : norm < 0.0f ? 0.0f : norm);
            break;
        case 3:
            q.x = ReadFloat();
            q.y = ReadFloat();
            q.z = ReadFloat();
            norm = 1.0f - q.x * q.x - q.y * q.y - q.z * q.z;
            q.w = std::sqrtf(norm > 1.0f ? 1.0f : norm < 0.0f ? 0.0f : norm);
            break;
        }
        return q;
    }

    Quaternion PacketReader::ReadHalfQuaternion() {
        uint8 i = ReadByte();
        Quaternion q{};
        float32 norm;
        switch (i) {
        case 0:
            q.y = ReadHalf();
            q.z = ReadHalf();
            q.w = ReadHalf();
            norm = 1.0f - q.y * q.y - q.z * q.z - q.w * q.w;
            q.x = std::sqrtf(norm > 1.0f ? 1.0f : norm < 0.0f ? 0.0f : norm);
            break;
        case 1:
            q.x = ReadHalf();
            q.z = ReadHalf();
            q.w = ReadHalf();
            norm = 1.0f - q.x * q.x - q.z * q.z - q.w * q.w;
            q.y = std::sqrtf(norm > 1.0f ? 1.0f : norm < 0.0f ? 0.0f : norm);
            break;
        case 2:
            q.x = ReadHalf();
            q.y = ReadHalf();
            q.w = ReadHalf();
            norm = 1.0f - q.x * q.x - q.y * q.y - q.w * q.w;
            q.z = std::sqrtf(norm > 1.0f ? 1.0f : norm < 0.0f ? 0.0f : norm);
            break;
        case 3:
            q.x = ReadHalf();
            q.y = ReadHalf();
            q.z = ReadHalf();
            norm = 1.0f - q.x * q.x - q.y * q.y - q.z * q.z;
            q.w = std::sqrtf(norm > 1.0f ? 1.0f : norm < 0.0f ? 0.0f : norm);
            break;
        }
        return q;
    }
}