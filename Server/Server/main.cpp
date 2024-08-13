#include <iostream>
#include "Deep.h"
#include "Deep/Net.h"
#include "Deep/Math.h"
#include "Deep/Threading.h"

// https://stackoverflow.com/a/62047818/9642458
#include <chrono>
#include <thread>

int main() {
    size_t count = 1000000000;

    Deep::Vec3* positions = new Deep::Vec3[count];
    Deep::Vec3* velocities = new Deep::Vec3[count];

    for (size_t i = 0; i < count; ++i) {
        positions[i] = Deep::Vec3::zero;
        velocities[i] = Deep::Vec3::left;
    }

    auto start = std::chrono::system_clock::now();

    for (size_t i = 0; i < count; ++i) {
        positions[i] += velocities[i];
    }

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << elapsed.count() << "ms\n";

    for (size_t i = 0; i < 5; ++i) {
        std::cout << positions[i].x << std::endl;
    }
    std::cout << "...\n";
    for (size_t i = count - 6; i < count; ++i) {
        std::cout << positions[i].x << std::endl;
    }

    /*std::cout << "Hello World!" << std::endl;
    Deep::InitializeSockets();
    Deep::UDPSocket socket;
    socket.Open();
    int32 res = socket.Connect(Deep::IPv4(127, 0, 0, 1, 23152));
    Deep::IPv4 address;
    socket.GetPeerName(address);
    std::cout << static_cast<uint32>(address.a)
        << ":" << static_cast<uint32>(address.b)
        << ":" << static_cast<uint32>(address.c)
        << ":" << static_cast<uint32>(address.d)
        << ":" << address.port << std::endl;

    const uint8 data[] = "That's crazy";

    std::cout << sizeof data << std::endl;

    Deep::Vec3 up{ 0, 1, 0 };
    Deep::Vec3 axis{ 0, 0, 1 };
    Deep::Quat rotateLeft{ axis, PI / 2.0f };
    Deep::Vec3 right = rotateLeft * up;
    std::cout << rotateLeft.w
        << "," << rotateLeft.x
        << "," << rotateLeft.y
        << "," << rotateLeft.z
        << std::endl;
    std::cout << right.x
        << "," << right.y
        << "," << right.z
        << std::endl;

    std::cout << std::endl;

    while (true) {
        Deep::PacketWriter packet;
        packet.Write(static_cast<uint16>(sizeof data));
        packet.Write(data, sizeof data);

        socket.Send(packet);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
    };

    socket.Close();
    Deep::ShutdownSockets();*/
}