/**
 * Sock
 */

#pragma once

#include <Deep.h>

#define DEEP_SOCKET_NOERROR (0)
#define DEEP_SOCKET_ERROR (-1)
#define DEEP_SOCKET_UNKNOWN_ADDRESS_FAMILY (-2)
#define DEEP_SOCKET_INCOMPATIBLE_ADDRESS_FAMILY (-3)

#include <Deep/Sock/AF_INET.h>
#include <Deep/Net/PacketWriter.h>

#if defined(DEEP_PLATFORM_WINDOWS)

#include <Deep/Sock/Sock_Windows.h>

#endif

// TODO(randomuserhi): Refactor to UDPServer, UDPClient, TCPServer, TCPClient
//                     -> There is no reason to have my own wrapper around individual socket implementation
//                     -> Instead wrap the entirety of the server/client system
//                     -> Refactor how I approach portability in this sense
//                     -> Possibly, remove "Sock.h" and place everything inside "Net.h"

namespace Deep {
    // This namespace is used for storing OS implementation specifics
    // NOTE(randomuserhi): They can still be accessed by the user via __impl__ to allow them to write
    //                     OS specific code.
    //                     It is ill-advised to rely on this in normal use-cases as it may change
    //                     between compilation on different OS.
    //                     Only specific definitions that MUST be visible or that are useful to be exposed
    //                     for OS specific reasons will be found under __impl__ in this header
    namespace __impl__ {
        struct UDPSocket; // Stores implementation specific socket information such as Handles etc...
        struct TCPSocket; // Stores implementation specific socket information such as Handles etc...
    } // namespace __impl__

    class UDPSocket {
    public:
        UDPSocket() :
            __impl__() {}

        int32 GetSockName(IPv4& woAddress) const;

        int32 GetPeerName(IPv4& woAddress) const;

        int32 Open();
        int32 Close();

        int32 Bind(uint16 port = 0) const;
        int32 Connect(IPv4 address) const;

        // NOTE(randomuserhi): When using `Bind`, these methods will error, use SendTo instead.
        int32 Send(const uint8* data, size_t dataSize) const;
        Deep_Inline int32 Send(const PacketWriter& packet) const;

        int32 SendTo(const uint8* data, size_t dataSize, IPv4 address) const;
        Deep_Inline int32 SendTo(const PacketWriter& packet, IPv4 address) const;

        // NOTE(randomuserhi): On error, woFromAddress will remain untouched.
        int32 Receive(uint8* buffer, size_t maxBufferSize, size_t& woBytesReceived, IPv4& woFromAddress) const;

    private:
        __impl__::UDPSocket __impl__;
    };

    class TCPSocket {
    public:
        TCPSocket() :
            __impl__() {}

    private:
        __impl__::TCPSocket __impl__;
    };
} // namespace Deep

#include "Sock.inl"