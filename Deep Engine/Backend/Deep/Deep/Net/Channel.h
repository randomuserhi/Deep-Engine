#pragma once

#include "../Sock.h"

namespace Deep {
    class Channel {
    public:
        Channel(size_t bufferSize)
            : bufferSize(bufferSize), buffer(new uint8[bufferSize])
        {}

    private:
        size_t bufferSize;
        uint8* const buffer;
        UDPSocket socket;
    };
}