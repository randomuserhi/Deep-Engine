#include <iostream>

#include <Deep.h>
#include <Deep/Memory.h>
#include <Deep/Net.h>
#include <Deep/Math.h>

// https://stackoverflow.com/a/62047818/9642458
#include <chrono>
#include <thread>
#include <vector>

struct Net {
    // TODO(randomuserhi):
    // - Handle disconnects
    // - Design a proper protocol (currently only supports raw UDP)

    Net(int32 port, size_t bufferSize = 4096) :
        bufferSize(bufferSize), buffer(reinterpret_cast<uint8*>(Deep::Malloc(bufferSize))) {
        socket.Open();
        socket.Bind(23152);
    };

    ~Net() {
        socket.Close();
        Deep::Free(buffer);
    }

    void AddClient(Deep::IPv4 client) {
        // NOTE(randomuserhi): A linear loop is used as number of clients is low
        for (auto c : clients) {
            if (c == client) return;
        }
        clients.push_back(client);
    }

    void Send(const Deep::PacketWriter& packet, int32* woResults = nullptr) {
        for (size_t i = 0; i < clients.size(); ++i) {
            int32 result = socket.SendTo(packet.data(), packet.size(), clients[i]);
            if (woResults != nullptr) woResults[i] = result;
        }
    }

    Deep::UDPSocket socket;
    std::vector<Deep::IPv4> clients;

    const size_t bufferSize;
    uint8* const buffer;
};

static Deep_Inline void NetTick(Net& net) {
    size_t bytesReceived = 0;
    Deep::IPv4 fromAddress;

    while (net.socket.Receive(net.buffer, net.bufferSize, bytesReceived, fromAddress) == DEEP_SOCKET_NOERROR
           && bytesReceived > 0) {
        net.AddClient(fromAddress);
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
    net.socket.GetSockName(address);
    std::cout << "Server running on port " << address.port << "\n";

    // Main Tick Loop
    constexpr const int32 tickRate = 60; // Server runs at 60 ticks per second
    constexpr const int32 totalFrameTime = 1000 / tickRate;
    auto start = std::chrono::system_clock::now();
    while (true) {
        auto now = std::chrono::system_clock::now();
        float32 dt =
            static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count()) / 1000.0f;
        start = now;

        // Network tick
        NetTick(net);

        // Perform tick
        Tick(net, dt);

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        // Spin lock to wait until next frame
        using namespace std::chrono_literals;
        long long timeleft = totalFrameTime - elapsed.count();
        if (timeleft < 0) {
            std::cout << "Tick took longer than expected! [" << elapsed.count() << "ms]\n";
            timeleft = 0ll;
        }
        while (std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())
                   .time_since_epoch()
                   .count()
               < std::chrono::time_point_cast<std::chrono::milliseconds>(end).time_since_epoch().count() + timeleft) {
            // std::this_thread::sleep_for(std::chrono::microseconds{ 1 });
        }
    };

    Deep::ShutdownSockets();

    return 0;
}