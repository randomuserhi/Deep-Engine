#include "../Sock.h"

namespace Deep {
    bool IsBigEndian() {
        uint x = 1;
        uint8* c = (uint8*)&x;
        return (uint8)*c == 0; // 0 if big endian, 1 if little endian
    }
}