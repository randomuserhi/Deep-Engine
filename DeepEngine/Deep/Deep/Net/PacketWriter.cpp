/**
 * Packet
 */

#include <winsock2.h>
#include <cmath>

#include "./PacketWriter.h"
#include "../Math.h"

namespace Deep {
    void PacketWriter::Write(Quaternion value) {
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
    void PacketWriter::WriteHalf(Quaternion value) {
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