#pragma once

namespace Deep {
    int32 UDPSocket::Send(const PacketWriter& packet) const {
        return Send(packet.data(), packet.size());
    }
    int32 UDPSocket::SendTo(const PacketWriter& packet, IPv4 address) const {
        return SendTo(packet.data(), packet.size(), address);
    }
} // namespace Deep