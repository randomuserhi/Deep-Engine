/**
 * Sock
 */

#include <Deep/Sock.h>

#if defined(DEEP_PLATFORM_WINDOWS)

#include "Sock_Windows.h"

namespace Deep {
    // https://stackoverflow.com/a/17925300/9642458 => Although not allowed by the standard, target compilers
    //  support accessing inactive member of union.
    //  Additionally, the use of a union also ensures alignment.
    union SocketAddr {
        sockaddr sa;
        sockaddr_in sa_in;
    };

    typedef int32 socklen_t;

    Deep_Inline SocketAddr ToSocketAddr(IPv4 ip) {
        const uint32 bitAddress = (ip.a << 24) | (ip.b << 16) | (ip.c << 8) | ip.d;

        SocketAddr address;
        address.sa_in.sin_family = AF_INET;
        address.sa_in.sin_addr.s_addr = htonl(bitAddress);
        address.sa_in.sin_port = htons(ip.port);

        return address;
    }
    int32 FromSocketAddr(SocketAddr sockAddr, IPv4& woFormat) {
        if (sockAddr.sa.sa_family == AF_INET) {
            const uint32 bitAddress = ntohl(sockAddr.sa_in.sin_addr.s_addr);
            woFormat.a = (bitAddress & 0xFF000000) >> 24;
            woFormat.b = (bitAddress & 0x00FF0000) >> 16;
            woFormat.c = (bitAddress & 0x0000FF00) >> 8;
            woFormat.d = (bitAddress & 0x000000FF);
            woFormat.port = ntohs(sockAddr.sa_in.sin_port);

            return DEEP_SOCKET_NOERROR;
        }

        return DEEP_SOCKET_INCOMPATIBLE_ADDRESS_FAMILY;
    }

    int32 InitializeSockets() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR) {
            return DEEP_SOCKET_NOERROR;
        }
        return DEEP_SOCKET_ERROR;
    }

    int32 ShutdownSockets() {
        const int32 result = WSACleanup();
        return result == SOCKET_ERROR ? DEEP_SOCKET_ERROR : DEEP_SOCKET_NOERROR;
    }

    int32 UDPSocket::GetSockName(IPv4& woAddress) const {
        SocketAddr sockAddr;
        socklen_t assignedAddressLen = sizeof sockAddr;
        if (getsockname(__impl__.socketFD, &sockAddr.sa, &assignedAddressLen) != NO_ERROR) return DEEP_SOCKET_ERROR;
        return FromSocketAddr(sockAddr, woAddress);
    }

    int32 UDPSocket::GetPeerName(IPv4& woAddress) const {
        SocketAddr sockAddr;
        socklen_t assignedAddressLen = sizeof sockAddr;
        if (getpeername(__impl__.socketFD, &sockAddr.sa, &assignedAddressLen) != NO_ERROR) return DEEP_SOCKET_ERROR;
        return FromSocketAddr(sockAddr, woAddress);
    }

    int32 UDPSocket::Open() {
        SOCKET& socketFD = __impl__.socketFD;

        if (socketFD != INVALID_SOCKET) {
            Close();
        }

        socketFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (socketFD == INVALID_SOCKET) {
            return DEEP_SOCKET_ERROR;
        }

        // Set socket to be non-blocking
        DWORD nonBlocking = 1;
        if (ioctlsocket(socketFD, FIONBIO, &nonBlocking) == SOCKET_ERROR) {
            // Failed to set socket to non-blocking
            return DEEP_SOCKET_ERROR;
        }

// Need to be aware of https://stackoverflow.com/questions/34242622/windows-udp-sockets-recvfrom-fails-with-error-10054
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)
        bool bNewBehavior = false;
        DWORD dwBytesReturned = 0;
        WSAIoctl(socketFD, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);

        return DEEP_SOCKET_NOERROR;
    }

    int32 UDPSocket::Close() {
        SOCKET& socketFD = __impl__.socketFD;
        if (socketFD == INVALID_SOCKET) {
            // Socket already closed / SocketFD is invalid
            return DEEP_SOCKET_NOERROR;
        }

        const int32 result = closesocket(socketFD);
        if (result == SOCKET_ERROR) {
            return DEEP_SOCKET_ERROR;
        }
        socketFD = INVALID_SOCKET;
        return DEEP_SOCKET_NOERROR;
    }

    int32 UDPSocket::Bind(uint16 port) const {
        const SOCKET& socketFD = __impl__.socketFD;

        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        Deep_Assert(port < USHRT_MAX, "Port must be a ushort value.");
        address.sin_port = htons(port);

        if (bind(socketFD, (struct sockaddr*)&address, sizeof address) == SOCKET_ERROR) {
            return DEEP_SOCKET_ERROR;
        }

        return DEEP_SOCKET_NOERROR;
    }

    int32 UDPSocket::Connect(IPv4 address) const {
        const SOCKET& socketFD = __impl__.socketFD;

        const SocketAddr sockAddr = ToSocketAddr(address);
        if (connect(socketFD, &sockAddr.sa, sizeof sockAddr) != NO_ERROR) {
            // Failed to connect socket with error
            return DEEP_SOCKET_ERROR;
        }
        return DEEP_SOCKET_NOERROR;
    }

    int32 UDPSocket::Send(const uint8* data, size_t dataSize) const {
        Deep_Assert(dataSize < INT_MAX, "Size of buffer cannot be larger than INT_MAX.");

        const SOCKET& socketFD = __impl__.socketFD;
        const int32 sentBytes = send(socketFD, reinterpret_cast<const char*>(data), static_cast<int>(dataSize), 0);
        if (sentBytes == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
            return DEEP_SOCKET_ERROR;
        }
        return DEEP_SOCKET_NOERROR;
    }

    int32 UDPSocket::SendTo(const uint8* data, size_t dataSize, IPv4 address) const {
        Deep_Assert(dataSize < INT_MAX, "Size of buffer cannot be larger than INT_MAX.");

        const SOCKET& socketFD = __impl__.socketFD;

        const SocketAddr sockAddr = ToSocketAddr(address);
        const int32 sentBytes = sendto(socketFD, reinterpret_cast<const char*>(data), static_cast<int>(dataSize), 0,
                                       &sockAddr.sa, sizeof sockAddr);
        if (sentBytes == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
            return DEEP_SOCKET_ERROR;
        }
        return DEEP_SOCKET_NOERROR;
    }

    int32 UDPSocket::Receive(uint8* buffer, size_t maxBufferSize, size_t& woBytesReceived, IPv4& woFromAddress) const {
        Deep_Assert(maxBufferSize < INT_MAX, "Size of buffer cannot be larger than INT_MAX.");

        const SOCKET& socketFD = __impl__.socketFD;

        SocketAddr fromSockAddr;
        socklen_t fromLength = sizeof fromSockAddr;

        int32 result = recvfrom(socketFD, reinterpret_cast<char*>(buffer), static_cast<int>(maxBufferSize), 0,
                                &fromSockAddr.sa, &fromLength);

        woBytesReceived = Deep::Max(0, result);

        if (result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
            return DEEP_SOCKET_ERROR;
        }

        FromSocketAddr(fromSockAddr, woFromAddress);

        return DEEP_SOCKET_NOERROR;
    }
} // namespace Deep

#endif