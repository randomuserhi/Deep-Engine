/**
 * Sock
 */

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

namespace Deep {
    int32 InitializeSockets();
    int32 ShutdownSockets();

    namespace __impl__ {
        struct UDPSocket {
            SOCKET socketFD = INVALID_SOCKET;
        };

        struct TCPSocket {
            SOCKET socketFD = INVALID_SOCKET;
        };

        Deep_Inline int32 GetWSAError() {
            return WSAGetLastError();
        }
    } // namespace __impl__
} // namespace Deep