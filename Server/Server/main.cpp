#include <iostream>

#include <Deep.h>
#include <Deep/Net.h>
#include <Deep/Math.h>

// https://stackoverflow.com/a/62047818/9642458
#include <chrono>
#include <thread>

Deep_Inline void NetTick(Deep::UDPSocket& socket) {
    constexpr const size_t bufferSize = 4096;
    uint8 buffer[bufferSize];
    size_t bytesReceived = 0;
    Deep::IPv4 fromAddress;

    while (socket.Receive(buffer, bufferSize, bytesReceived, fromAddress) == DEEP_SOCKET_NOERROR && bytesReceived > 0) {
        std::cout << "Received " << bytesReceived << " bytes\n";
    }
}

Deep_Inline void Tick(float32 dt) {}

int main() {
    // Create server socket
    Deep::InitializeSockets();
    Deep::UDPSocket socket;
    socket.Open();
    int32 res = socket.Connect(Deep::IPv4(127, 0, 0, 1, 23152));
    Deep::IPv4 address;
    socket.GetPeerName(address);
    std::cout << static_cast<uint32>(address.a)        //
              << ":" << static_cast<uint32>(address.b) //
              << ":" << static_cast<uint32>(address.c) //
              << ":" << static_cast<uint32>(address.d) //
              << ":" << address.port << "\n"
              << "conn: " << res << "\n";

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
        NetTick(socket);

        // Perform tick
        Tick(dt);

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << elapsed.count() << "ms tick time\n";

        // Spin lock for any wait time until next frame
        using namespace std::chrono_literals;
        const long long timeleft = Deep::Max(totalFrameTime - elapsed.count(), 0ll);
        while (std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())
                   .time_since_epoch()
                   .count()
               < std::chrono::time_point_cast<std::chrono::milliseconds>(end).time_since_epoch().count() + timeleft) {
            // std::this_thread::sleep_for(std::chrono::microseconds{ 1 });
        }
    };

    socket.Close();
    Deep::ShutdownSockets();
}