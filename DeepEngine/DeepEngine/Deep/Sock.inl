#pragma once

namespace Deep {
    int32 UDPSocket::Send(const PacketWriter& packet) {
        return Send(packet.data(), packet.size());
    }
    int32 UDPSocket::SendTo(const PacketWriter& packet) {
        return Send(packet.data(), packet.size());
    }
} // namespace Deep