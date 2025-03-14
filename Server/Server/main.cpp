#include <iostream>

#include <Deep.h>
#include <Deep/Memory.h>
#include <Deep/Net.h>
#include <Deep/Math.h>
#include <Deep/Time.h>

// https://stackoverflow.com/a/62047818/9642458
#include <chrono>
#include <thread>
#include <vector>

class Net {
    // TODO(randomuserhi):
    // - Handle disconnects
    // - Design a proper protocol (currently only supports raw UDP)

private:
    struct Client {
        Client(Deep::IPv4 address) :
            address(address) {};

        Deep::IPv4 address;
        uint16 sequence = 0;
        uint16 remoteSequence = 0;
        long long lastReceivedTimestamp = 0;
    };

public:
    Net(int32 port, size_t bufferSize = 4096) :
        bufferSize(bufferSize), buffer(reinterpret_cast<uint8*>(Deep::Malloc(bufferSize))) {
        socket.Open();
        socket.Bind(23152);
    };

    ~Net() {
        socket.Close();
        Deep::Free(buffer);
    }

    void Send(const Deep::PacketWriter& packet, int32* woResults = nullptr) {
        for (size_t i = 0; i < clients.size(); ++i) {
            int32 result = SendTo(packet.data(), packet.size(), clients[i]);
            if (woResults != nullptr) woResults[i] = result;
        }
    }

    Deep_Inline int32 SendTo(const uint8* data, size_t size, Client& rwClient) {
        return socket.SendTo(data, size, rwClient.address);
    }

    Deep_Inline int32 Receive(size_t woBytesReceived, Deep::IPv4 woFromAddress) {
        int32 result = socket.Receive(buffer, bufferSize, woBytesReceived, woFromAddress);
        if (result != DEEP_SOCKET_NOERROR) return result;

        Deep::PacketReader packet{ buffer };
        uint16 sequenceNumber = packet.ReadUInt16();
        uint16 ackNumber = packet.ReadUInt16();
        uint32 ackBitField = packet.ReadUInt32();

        Client& client = AddClient(woFromAddress);
        client.lastReceivedTimestamp = Deep::Epoch();

        // TODO(randomuserhi): update based on recieved packet header...
        // TODO(randomuserhi): copy packet contents to application side (don't pass internal buffer around)

        return result;
    }

    Deep_Inline int32 GetSockAddress(Deep::IPv4& woAddress) const {
        return socket.GetSockName(woAddress);
    }

private:
    Client& AddClient(Deep::IPv4 client) {
        // NOTE(randomuserhi): A linear loop is used as number of clients is low
        for (Client& c : clients) {
            if (c.address == client) return c;
        }
        clients.emplace_back(client);
        return clients[clients.size() - 1];
    }

    Deep::UDPSocket socket;
    std::vector<Client> clients;

    const size_t bufferSize;
    uint8* const buffer;
};

static Deep_Inline void NetTick(Net& net) {
    size_t bytesReceived = 0;
    Deep::IPv4 fromAddress;

    while (net.Receive(bytesReceived, fromAddress) == DEEP_SOCKET_NOERROR && bytesReceived > 0) {
        std::cout << "Received " << bytesReceived << " bytes.\n";
    }
}

static Deep_Inline void Tick(Net& net, float32 dt) {
    Deep::PacketWriter packet{};
    const uint8 msg[] = "hello";
    packet.Write(msg, sizeof msg);
    net.Send(packet);
}

int main() {
    Deep::InitializeSockets();

    Net net{ 23152 };
    Deep::IPv4 address;
    net.GetSockAddress(address);
    std::cout << "Server running on port " << address.port << "\n";

    // Main Tick Loop
    constexpr const int32 tickRate = 60; // Server runs at 60 ticks per second
    constexpr const int32 totalFrameTime = 1000 / tickRate;
    long long start = Deep::Epoch();
    while (true) {
        long long now = Deep::Epoch();
        float32 dt = static_cast<float>(now - start) / 1000.0f;
        start = now;

        // Network tick
        NetTick(net);

        // Perform tick
        Tick(net, dt);

        long long end = Deep::Epoch();
        long long elapsed = end - start;

        // Spin lock to wait until next frame
        using namespace std::chrono_literals;
        long long timeleft = totalFrameTime - elapsed;
        if (timeleft < 0) {
            std::cout << "Tick took longer than expected! [" << elapsed << "ms]\n";
            timeleft = 0ll;
        }
        while (Deep::Epoch() < end + timeleft) {
            // std::this_thread::sleep_for(std::chrono::microseconds{ 1 });
        }
    };

    Deep::ShutdownSockets();

    return 0;
}